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
       (strcmp("/dev/pts/1", argv[1]) != 0) &&
       (strcmp("/dev/pts/3", argv[1]) != 0) &&
       (strcmp("/dev/pts/7", argv[1]) != 0)))
  {
    printf("USAGE EX: ./sender /dev/ttyS0 <FileToSend>\n");
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
  long int fileSize = ftell(filefd);
  fseek(filefd, 0, SEEK_SET);

  printf("Opened file with name %s and size %ld bytes\n\n", argv[2], fileSize);
  printf("Sender program started ...\n\n");

  ////////////////////////
  // OPEN COMMUNICATION //
  ////////////////////////
  int fd = llopen(argv[1], TRANSMITTER);

  if (fd == -1)
    return 1;

  printf("\n#####################\n");
  printf("Conection established\n");
  printf("#####################\n\n");

  ////////////////////////
  //  SEND INFORMATION  //
  ////////////////////////

  // Create Start and End control package
  char startControl[10] = "20";
  char endControl[10] = "30";
  int controlLengt;
  /**
   *  2^8 = 256
   *  2^16 = 65536
   *  2^32 = 16777215
   */


  if (fileSize < 256)
  {
    append(startControl, '1');
    append(startControl, (unsigned char)fileSize);
    append(endControl, '1');
    append(endControl, (unsigned char)fileSize);
    controlLengt = 4;
  }
  else if (fileSize < 65536 && fileSize >= 256)
  {
    append(startControl, '2');
    append(startControl, (unsigned char)(fileSize >> 8));
    append(startControl, (unsigned char)fileSize);
    append(endControl, '2');
    append(endControl, (unsigned char)(fileSize >> 8));
    append(endControl, (unsigned char)fileSize);
    controlLengt = 5;
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
    controlLengt = 6;
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

  int llwrite_return_value;

  llwrite_return_value = llwrite(fd, startControl, controlLengt);
    if(llwrite_return_value == -1)
    {
        printf("Timeout!\n");
        return -1;
    }

  // Send file information

  // Get File Bytes
  char *fileInfo = (char *)malloc(fileSize);
  fread(fileInfo, sizeof(char), fileSize, filefd);

  // Se enviarmos 256 bytes de cada vez
  // TO DO - substituir 256 por constante definida em data_macros.h
  int nrIterationsNeeded = fileSize / 256;
  int remainderBytes = fileSize % 256;

  int i = 0;
  int nrFileByte = 0;

  // Send file data
  for (; i < nrIterationsNeeded; i++)
  {
    char data[MAX_LEN];
    data[0] = 1;
    data[1] = i % 255;
    data[2] = 1; // é preciso alterar de acordo com valor definido em cima (256)
    data[3] = 0;

    for (int j = 0; j < 256; j++) // é preciso alterar de acordo com valor definido em cima (256)
    {
      data[4 + j] = fileInfo[nrFileByte];
      nrFileByte++;
    }

    llwrite_return_value = llwrite(fd, data, 260); // é preciso alterar de acordo com valor definido em cima (256)

    if(llwrite_return_value == -1)
    {
        printf("Timeout!\n");
        return -1;
    }
  }

  // Send remainder of bytes
  if (remainderBytes)
  {
    char data[remainderBytes + 10];
    data[0] = 1;
    data[1] = i % 255;
    data[2] = 0; // é preciso alterar de acordo com valor definido em cima (256)
    data[3] = remainderBytes;

    for (int j = 0; j < remainderBytes; j++)
    {
      data[4 + j] = fileInfo[nrFileByte];
      nrFileByte++;
    }

    llwrite_return_value = llwrite(fd, data, 4 + remainderBytes);

    if(llwrite_return_value == -1)
    {
        printf("Timeout!\n");
        return -1;
    }
  }

  // Send End Control Package
  llwrite_return_value = llwrite(fd, endControl, controlLengt);
    
  if(llwrite_return_value == -1)
    {
        printf("Timeout!\n");
        return -1;
    }


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
