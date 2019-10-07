/*Non-Canonical Input Processing*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

void handleState(unsigned char byte);

#define FLAG 0x7e
#define A_ANS 0x03
#define A_SND 0x01
#define C_SET 0x03
#define C_UA 0x07
unsigned char BCC = A_ANS ^ C_UA;
unsigned char BCC_SET = A_ANS ^ C_SET;

#define Start 0
#define FLAG_RCV 1
#define A_RCV 2
#define C_RCV 3
#define BCC_OK 4
#define STOP 5

unsigned int state = Start;

int main(int argc, char **argv)
{
    int fd, c, res;
    struct termios oldtio, newtio;
    char buf[255];
    int i, sum = 0, speed = 0;

    if ((argc < 2) ||
        ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
        (strcmp("/dev/ttyS1", argv[1]) != 0) &&
        (strcmp("/dev/pts/5", argv[1]) != 0)))
    {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

      /*
        Open serial port device for reading and writing and not as controlling tty
        because we don't want to get killed if linenoise sends CTRL-C.
      */

    fd = open(argv[1], O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(argv[1]);
        exit(-1);
    }

    if (tcgetattr(fd, &oldtio) == -1)
    { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 5;  /* blocking read until 5 chars received */

    /* 
      VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
      leitura do(s) pr�ximo(s) caracter(es)
    */

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");
    

    /* O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar 
      o indicado no gui�o  */

      
    // SEND 
    char SET[5];
    SET[0] = FLAG;
    SET[1] = A_ANS;
    SET[2] = C_SET;
    SET[3] = BCC_SET;
    SET[4] = FLAG;

    for (size_t i = 0; i < 5; i++)	
      write(fd, &SET[i], 1);

      printf("Signal sent\n");


      //RECEIVE
      unsigned char byte;
      
      while (state != STOP)
      {
        read(fd, &byte, 1); // read byte
        handleState(byte);
      }

      printf("Signal received\n");

      
      /* 
        O ciclo WHILE deve ser alterado de modo a respeitar o indicado no gui�o 
      */

      sleep(1);

      tcsetattr(fd, TCSANOW, &oldtio);
      close(fd);
      return 0;
  }

  void handleState(unsigned char byte)
  {
    switch (state) {
      case Start:
        if (byte == FLAG)
          state = FLAG_RCV;
        break;

      case FLAG_RCV:
        if (byte == A_ANS)
          state = A_RCV;
        else if (byte == FLAG)
          state = FLAG_RCV;
        else
          state = Start;
        break;
    
      case A_RCV:
        if (byte == C_UA)
          state = C_RCV;
        else if (byte == FLAG)
          state = FLAG_RCV;
        else
          state = Start;
        break;

      case C_RCV:
        if (byte == BCC)
          state = BCC_OK;
        else if (byte == FLAG)
          state = FLAG_RCV;
        else
          state = Start;
        break;

      case BCC_OK:
        if (byte == FLAG)
          state = STOP;
        else
          state = Start;
        break;		
    }
}
