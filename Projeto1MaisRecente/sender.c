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

#define _POSIX_SOURCE 1 /* POSIX compliant source */
extern int Ns;

int main(int argc, char **argv)
{
  if ((argc < 3) ||
      ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
       (strcmp("/dev/ttyS1", argv[1]) != 0) &&
       (strcmp("/dev/pts/2", argv[1]) != 0)))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1 fileName\n");
    exit(1);
  }

  // Open file
  FILE *filefd = fopen(argv[2], "r");

  if (filefd == NULL)
  {
    printf("File with name %s doesn't exist\n", argv[2]);
    return 1;
  }

  // Get file Size
  fseek(filefd, 0, SEEK_END);
  int fileSize = ftell(filefd);
  fseek(filefd, 0, SEEK_SET);

  printf("================================================\n");
  printf("Opened file with name %s and size %d\n\n", argv[2], fileSize);

  printf("Sender program started ...\n\n");

  ////////////////////////
  // OPEN COMMUNICATION
  ////////////////////////
  int fd = llopen(argv[1], TRANSMITTER);

  if (fd == -1)
    return 1;

  printf("\n#####################\n");
  printf("Conection established\n");
  printf("#####################\n\n");

  ////////////////////////
  // SEND INFORMATION
  ////////////////////////
  // Create Start and End control package
  char startControl[10] = "20";
  char endControl[10] = "30";

  if (fileSize < 256)
  {
    append(startControl, '1');
    append(startControl, (unsigned char)fileSize);
    append(endControl, '1');
    append(endControl, (unsigned char)fileSize);
  }
  else if (fileSize < 65536 && fileSize >= 256)
  {
    append(startControl, '2');
    append(startControl, (unsigned char)(fileSize >> 8));
    append(startControl, (unsigned char)fileSize);
    append(endControl, '2');
    append(endControl, (unsigned char)(fileSize >> 8));
    append(endControl, (unsigned char)fileSize);
  }
  else if (fileSize < 16777215 && fileSize >= 65536)
  {
    append(startControl, '3');
    append(startControl, (unsigned char)(fileSize >> 16));
    append(startControl, (unsigned char)(fileSize >> 8));
    append(startControl, (unsigned char)fileSize);
    append(endControl, '3');
    append(endControl, (unsigned char)(fileSize >> 16));
    append(endControl, (unsigned char)(fileSize >> 8));
    append(endControl, (unsigned char)fileSize);
  }
  else
  {
    printf("File Size too big!\n");
    return 1;
  }

  printf("\n#####################\n");
  printf("  Writing started\n");
  printf("#####################\n\n");

  // Send Start Control Package
  llwrite(fd, startControl, strlen(startControl));

  // Send End Control Package
  llwrite(fd, endControl, strlen(endControl));

  printf("\n#####################\n");
  printf("    Writing over\n");
  printf("#####################\n\n");

  ////////////////////////
  //END COMMUNICATION
  ////////////////////////
  if (llclose(fd) == -1)
    return 1;

  printf("\n#####################\n");
  printf("   Conection ended\n");
  printf("#####################\n\n");

  close(fd);
  return 0;
}