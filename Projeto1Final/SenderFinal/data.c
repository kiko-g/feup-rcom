//Ligacao de Dados
#include "data.h"
#include "states.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define BAUDRATE B38400
struct termios newtio, oldtio;
enum openAS modo;
int Ns = 0, Nr = 1;
int disconnect;
int dataTransferOver = FALSE;

// ALARME
int flag = FALSE, conta = 1;
void atende(int sig) // atende alarme
{
  printf("alarme # %d\n", conta);
  flag = TRUE;
  conta++;
}

int llopen(char *porta, enum openAS modoAbertura)
{
  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  modo = modoAbertura;
  int fd = open(porta, O_RDWR | O_NOCTTY);
  if (fd < 0)
  {
    perror(porta);
    exit(-1);
  }

  if (modo == TRANSMITTER)
  {

    if (tcgetattr(fd, &oldtio) == -1)
    { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = OPOST;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 0;  /* blocking read until 5 chars received */

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
      perror("tcsetattr");
      exit(-1);
    }

    char SET[5];
    SET[0] = FLAG;
    SET[1] = A_ANS;
    SET[2] = C_SET;
    SET[3] = (A_ANS ^ C_SET);
    SET[4] = FLAG;

    unsigned char byte;
    (void)signal(SIGALRM, atende); // instala  rotina que atende interrupcao

    do
    {
      // SEND SET
      for (size_t i = 0; i < 5; i++)
        write(fd, &SET[i], 1);

      printf("SET sent\n");

      alarm(3);

      flag = FALSE;
      int state = Start;

      while (state != STOP && !flag)
      {
        read(fd, &byte, 1); // read byte
        handleStateOfEstablishment(&state, byte, C_UA);
      }
    } while (flag && conta < 4);



    if (flag && conta == 4) return -1;
    else
    {
      printf("UA received\n");
      alarm(0);
      flag = FALSE;
      conta = 1;
    }
  }

  else if (modo == RECEIVER)
  {
    tcgetattr(fd, &oldtio);

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;

    newtio.c_oflag = 0;
    newtio.c_lflag = 0;     /* set input mode (non-canonical, no echo,...) */

    newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 1;  /* blocking read until 5 chars received */

    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);

    // RECEIVE SET
    unsigned char byte;
    int state = Start;

    while (state != STOP)
    {
      read(fd, &byte, 1); // read byte
      handleStateOfEstablishment(&state, byte, C_SET);
    }

    printf("SET received\n");

    // SEND UA
    char UA[5];
    UA[0] = FLAG;
    UA[1] = A_ANS;
    UA[2] = C_UA;
    UA[3] = (A_ANS ^ C_UA);
    UA[4] = FLAG;

    for (size_t i = 0; i < 5; i++)
      write(fd, &UA[i], 1);

    printf("UA sent\n");
  }
  else return -1;

  return fd;
}

int llclose(int fd)
{
  if (modo == RECEIVER)
  {
    int state = Start;
    unsigned char byte;

    // SEND DISC
    char DISC[5];
    DISC[0] = FLAG;
    DISC[1] = A_SND;
    DISC[2] = C_DISC;
    DISC[3] = DISC[1] ^ DISC[2];
    DISC[4] = FLAG;

    for (size_t i = 0; i < 5; i++)
      write(fd, &DISC[i], 1);

    printf("DISC sent\n");

    // RECEIVE UA
    state = Start;
    while (state != STOP)
    {
      read(fd, &byte, 1); // read byte
      handleStateOfTermination(&state, byte, C_UA);
    }
    printf("UA received\n");

    sleep(1);

    tcsetattr(fd, TCSANOW, &oldtio);
    close(fd);

    return fd;
  }
  else if (modo == TRANSMITTER)
  {
    // SEND DISC
    char DISC[5];
    DISC[0] = FLAG;
    DISC[1] = A_ANS;
    DISC[2] = C_DISC;
    DISC[3] = DISC[1] ^ DISC[2];
    DISC[4] = FLAG;

    for (size_t i = 0; i < 5; i++)
      write(fd, &DISC[i], 1);

    printf("DISC sent\n");

    int state = Start;
    unsigned char byte;

    // RECEIVE DISC
    while (state != STOP)
    {
      read(fd, &byte, 1); // read byte
      handleStateOfTermination(&state, byte, C_DISC);
    }
    printf("DISC received\n");

    // SEND UA
    char UA[5];
    UA[0] = FLAG;
    UA[1] = A_SND;
    UA[2] = C_UA;
    UA[3] = UA[1] ^ UA[2];
    UA[4] = FLAG;

    for (size_t i = 0; i < 5; i++)
      write(fd, &UA[i], 1);

    printf("UA sent\n");

    sleep(1);

    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
      perror("tcsetattr");
      exit(-1);
    }
    return fd;
  }
  else return -1;
}

int llwrite(int fd, char *buffer, int length)
{
  // BYTE STUFFING com buffer
  size_t dataNewSize = 0;
  size_t BCC2NewSize = 0;

  unsigned char *BCC2 = (unsigned char *)malloc(sizeof(unsigned char *));
  char *dataStuffed = stuff(buffer, length, &dataNewSize, BCC2);
  char *newBCC2 = stuffBCC2(*BCC2, &BCC2NewSize);

  // SEND I
  int tramaSize = 5 + dataNewSize + BCC2NewSize;
  char I[tramaSize];
  I[0] = FLAG;
  I[1] = A_ANS;

  if ((Ns % 2) == 0) I[2] = C_Ns0;
  else I[2] = C_Ns1;

  I[3] = I[1] ^ I[2];

  for (size_t i = 0; i < dataNewSize; i++)
    I[4 + i] = dataStuffed[i];

  for (size_t i = 0; i < BCC2NewSize; i++)
    I[4 + dataNewSize + i] = newBCC2[i];

  I[tramaSize - 1] = FLAG;



  // SEND I Trama
  int tramaSent = FALSE;
  (void)signal(SIGALRM, atende); //instala rotina que atende interrupcao


  do
  {
    if(conta > 3)
        return -1;

    for (size_t i = 0; i < tramaSize; i++)
      write(fd, &I[i], 1);

    printf("I trama sent with Ns = %d\n", Ns);
    alarm(3);
    flag = FALSE;

    // RECEIVE C
    unsigned char byte;
    int state = Start;

    if ((Ns % 2) == 0)
    {
      while (state != STOP && !flag)
      {
          read(fd, &byte, 1); // read byte
          handleStateOfTransmissionSender(&state, byte, C_RR_Nr1, C_REJ_Nr1);
          if (state == C_RR)
          {
              printf("RR received\n");
              tramaSent = TRUE;
          }

          if (state == C_REJ)
          {
              printf("REJ received\n");
              flag = TRUE;
          }
      }
    }
    else
    {
       while (state != STOP && !flag)
       {
           read(fd, &byte, 1); // read byte
           handleStateOfTransmissionSender(&state, byte, C_RR_Nr0, C_REJ_Nr0);
           if (state == C_RR){
               printf("RR received\n");
               tramaSent = TRUE;
           }

           if (state == C_REJ){
               printf("REJ received\n");
               flag = TRUE;
           }
       }
    }

  } while (!tramaSent || (flag && conta < 4));

  if(conta > 3) return -1;
  else{
    alarm(0);
    flag = FALSE;
    conta = 1;
  }

  printf("\n");
  Ns++;
  return length;
}





int llread(int fd, char *buffer)
{
  // RECEIVE I
  unsigned char byte;
  int state = Start;
  char data[MAX_LEN] = "";
  size_t dataSize = 0;

  while (state != STOP)
  {
    read(fd, &byte, 1); // read byte

    if (state == DATA && byte != FLAG){
        data[dataSize] = byte;
        dataSize++;
    }

    if((Nr % 2) == 1) handleStateOfTransmissionReceiver(&state, byte, C_Ns0);
    else handleStateOfTransmissionReceiver(&state, byte, C_Ns1);


    if (dataTransferOver){
        if (byte == C_DISC){
        disconnect = 1;
        printf("DISC received\n");
      }
      else printf("# FINAL BYTE: 0x%02x\n", byte);

    }
  }

  if (disconnect == 1) return 0;
  else
  {
    // destuffing data
    size_t dataNewSize = 0;
    char *dataDeStuffed = deStuff(data, dataSize, &dataNewSize);
    char BCC2Received = dataDeStuffed[dataNewSize - 1];
    char BCC2Calculated = calcBCC2(dataDeStuffed, dataNewSize - 1);

    // Se BCC2 valid
    if (BCC2Received == BCC2Calculated)
    {
      // enviar RR
      unsigned char RR[5];
      RR[0] = FLAG;
      RR[1] = A_ANS;

      if ((Nr % 2) == 1)
        RR[2] = C_RR_Nr1;
      else
        RR[2] = C_RR_Nr0;

      RR[3] = RR[1] ^ RR[2];
      RR[4] = FLAG;

      // printf("Sending RR...\n");
      for (size_t i = 0; i < 5; i++)
        write(fd, &RR[i], 1);

      printf("RR sent\t [NR: %d]\n", Nr);

      for (int i = 0; i < dataNewSize - 1; i++)
        buffer[i] = dataDeStuffed[i];

      if (buffer[0] == '3')
        dataTransferOver = TRUE;

      Nr++;
	  return dataNewSize - 1;
    }

    else
    {
      // enviar REJ
      unsigned char REJ[5];
      REJ[0] = FLAG;
      REJ[1] = A_ANS;

      if ((Nr % 2) == 1)
        REJ[2] = C_REJ_Nr1;
      else
        REJ[2] = C_REJ_Nr0;

      REJ[3] = REJ[1] ^ REJ[2];
      REJ[4] = FLAG;

      for (size_t i = 0; i < 5; i++)
        write(fd, &REJ[i], 1);

      printf("REJ sent [NR: %d]\n", Nr);
	  return REJ_SENT;
    }


  }
}





void append(char *s, unsigned char c)
{
    int len = strlen(s);
    s[len] = c;
    s[len + 1] = '\0';
}

char *stuff(const char *dados, size_t currentSize, size_t *newSize, unsigned char *BCC2)
{
  char *temp = (char *)malloc(2 * currentSize * sizeof(char));
  size_t t = 0;
  *BCC2 = dados[0];

  for (size_t i = 0; i < currentSize; i++)
  {
    if (i != 0)
      *BCC2 ^= dados[i];

    char byteAtual = dados[i];

    if (byteAtual == FLAG)
    {
      temp[t] = ESCAPE;
      temp[t + 1] = xorFLAG;
      t += 2;
    }
    else if (byteAtual == ESCAPE)
    {
      temp[t] = ESCAPE;
      temp[t + 1] = xorESCAPE;
      t += 2;
    }
    else
    {
      temp[t] = byteAtual;
      t++;
    }
  }

  *newSize = t;
  dados++;
  return temp;
}

char *stuffBCC2(const unsigned char BCC2, size_t *newSize)
{
  char *temp;

  if (BCC2 == FLAG)
  {
    *newSize = 2;
    temp = (char *)malloc(2 * sizeof(char *));
    temp[0] = ESCAPE;
    temp[1] = xorFLAG;
  }
  else if (BCC2 == ESCAPE)
  {
    *newSize = 2;
    temp = (char *)malloc(2 * sizeof(char *));
    temp[0] = ESCAPE;
    temp[1] = xorESCAPE;
  }
  else
  {
    *newSize = 1;
    temp = (char *)malloc(sizeof(char *));
    temp[0] = (char)BCC2;
  }

  return temp;
}

char *deStuff(const char *dados, size_t currentSize, size_t *newSize)
{
  char *temp = (char *)malloc(currentSize * sizeof(char));
  size_t t = 0;

  for (size_t i = 0; i < currentSize; i++)
  {
    char byteAtual = dados[i];

    if (byteAtual == ESCAPE)
    {
      char proximoByte = dados[i + 1];

      if (proximoByte == xorFLAG) //It was a FLAG
      {
        // temp = (char *) malloc (sizeof(char *));
        temp[t] = FLAG;
        t++;
      }
      else if (proximoByte == xorESCAPE) //It was a ESCAPE
      {
        // temp = (char *) malloc (sizeof(char *));
        temp[t] = ESCAPE;
        t++;
      }

      i++;
    }
    else
    {
      // temp = (char *) malloc (sizeof(char *));
      temp[t] = byteAtual;
      t++;
    }
  }
  *newSize = t;
  return temp;
}

unsigned char *deStuffBCC2(const unsigned char *BCC2)
{
  unsigned char *temp;
  unsigned char primeiroByte = BCC2[0];

  if (primeiroByte == ESCAPE)
  {
    unsigned char segundoByte = BCC2[1];

    if (segundoByte == xorFLAG) //It was a FLAG
    {
      temp = (unsigned char *)malloc(sizeof(unsigned char *));
      temp[0] = FLAG;
    }
    else if (BCC2[1] == xorESCAPE) //It was a ESCAPE
    {
      temp = (unsigned char *)malloc(sizeof(unsigned char *));
      temp[0] = ESCAPE;
    }
  }

  else
  {
    temp = (unsigned char *)malloc(sizeof(unsigned char *));
    temp[0] = primeiroByte;
  }

  return temp;
}

char calcBCC2(const char *dados, size_t currentSize)
{
  char BCC2 = dados[0];
  for (size_t i = 1; i < currentSize; i++) BCC2 ^= dados[i];

  return BCC2;
}
