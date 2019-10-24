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
extern int Nr;

int main(int argc, char **argv)
{
    if ((argc < 3) ||
        ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
         (strcmp("/dev/ttyS1", argv[1]) != 0) &&
         (strcmp("/dev/pts/3", argv[1]) != 0)))
    {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1 newFileName\n");
        exit(1);
    }

    // Create file
    //FILE *filefd = fopen(argv[2], "w+");
    printf("Created file with name %s\n\n", argv[2]);
    printf("Receiver program started ...\n\n");

    // OPEN COMMUNICATION
    int fd = llopen(argv[1], RECEIVER);

    if (fd == -1)
        return 1;

    printf("\n#####################\n");
    printf("Conection established\n");
    printf("#####################\n\n");

    // READ INFORMATION
    char buffer[MAX_LEN];
    int llread_return_value; // if == 0, DISC was received

    printf("\n#####################\n");
    printf("   Reading Started\n");
    printf("#####################\n\n");

    // Get Start Control Package
    llread_return_value = llread(fd, buffer);

    do
    {
        llread_return_value = llread(fd, buffer);

        if (llread_return_value > 0)
            for (int i = 0; i < llread_return_value; i++)
                printf("Char: %x\n", buffer[i]);

        if (llread_return_value == -1)
            return 1;
    } while (llread_return_value != 0);

    printf("\n#####################\n");
    printf("    Reading Over\n");
    printf("#####################\n\n");

    //END COMMUNICATION
    if (llclose(fd) == -1)
        return 1;

    printf("\n#####################\n");
    printf("   Conection ended\n");
    printf("#####################\n\n");

    close(fd);
    return 0;
}
