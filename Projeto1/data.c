//Ligacao de Dados

#include "data.h"

int llopen(int porta, enum openAS modoAbertura)
{
    int fd,c, res;
    struct termios oldtio,newtio;

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

    fd = open(porta, O_RDWR | O_NOCTTY );
    if (fd <0) {perror(porta); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    if (modoAbertura == RECEIVER) {
        
        // RECEIVE SET
        unsigned char byte;
        int state = Start;

        while (state != STOP)
        {
            read(porta, &byte, 1); // read byte
            handleStateOf_S_and_U_Trama(byte, C_SET);
        }

        printf("SET received\n");

        // SEND UA
        char UA[5];
        UA[0] = FLAG;
        UA[1] = A_ANS;
        UA[2] = C_UA;
        UA[3] = BCC_UA;
        UA[4] = FLAG;

        for (size_t i = 0; i < 5; i++)
            write(fd, &UA[i], 1);

        printf("UA sent\n");

        return fd;
    }
    else if (modoAbertura == TRANSMITTER) {

      // SEND SET
      char SET[5];
      SET[0] = FLAG;
      SET[1] = A_ANS;
      SET[2] = C_SET;
      SET[3] = BCC_SET;
      SET[4] = FLAG;

      for (size_t i = 0; i < 5; i++)
        write(fd, &SET[i], 1);

      printf("SET sent\n");

        // RECEIVE UA
        unsigned char byte;
        int state = Start;

        while (state != STOP)
        {
            read(porta, &byte, 1); // read byte
            handleStateOf_S_and_U_Trama(byte, C_UA);
        }

        printf("UA received\n");

        return fd;
    }
    else return -1;
}

int llclose(int porta, enum openAS modoAbertura)
{
    if (modoAbertura == RECEIVER) {
        
        // RECEIVE SET
        unsigned char byte;
        int state = Start;

        while (state != STOP)
        {
            read(porta, &byte, 1); // read byte
            handleStateOf_S_and_U_Trama(byte, C_SET);
        }

        printf("SET received\n");

        // SEND UA
        char UA[5];
        UA[0] = FLAG;
        UA[1] = A_ANS;
        UA[2] = C_UA;
        UA[3] = BCC_UA;
        UA[4] = FLAG;

        for (size_t i = 0; i < 5; i++)
            write(fd, &UA[i], 1);

        printf("UA sent\n");

        return porta;
    }
    else if (modoAbertura == TRANSMITTER) {

      // SEND SET
      char SET[5];
      SET[0] = FLAG;
      SET[1] = A_ANS;
      SET[2] = C_SET;
      SET[3] = BCC_SET;
      SET[4] = FLAG;

      for (size_t i = 0; i < 5; i++)
        write(fd, &SET[i], 1);

      printf("SET sent\n");

        // RECEIVE UA
        unsigned char byte;
        int state = Start;

        while (state != STOP)
        {
            read(porta, &byte, 1); // read byte
            handleStateOf_S_and_U_Trama(byte, C_UA);
        }

        printf("UA received\n");

        return porta;
    }
    else return -1;
}
