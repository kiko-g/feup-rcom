/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "data.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */

int main(int argc, char **argv)
{
if ((argc < 2) ||
        ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
        (strcmp("/dev/ttyS1", argv[1]) != 0)))
    {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

    printf("Receiver program started ...\n");

     // OPEN COMMUNICATION
   int fd = llopen(argv[1], RECEIVER);
    
    if (fd == -1)
        return 1;


    // READ & SEND INFORMATION


     //END COMMUNICATION
      if (llclose(fd) == -1)
      return 1;


    sleep(1);

    // llclose
    tcsetattr(fd, TCSANOW, &oldtio);
    close(fd);
    return 0;
}