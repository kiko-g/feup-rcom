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
         (strcmp("/dev/pts/2", argv[1]) != 0) &&
         (strcmp("/dev/pts/4", argv[1]) != 0) &&
         (strcmp("/dev/pts/8", argv[1]) != 0)))
    {
        printf("USAGE EX: ./receiver /dev/ttyS0 <newFileName>\n");
        exit(1);
    }

    // Create file
    FILE *filefd = fopen(argv[2], "w+");
    printf("Created file with name %s\n\n", argv[2]);
    printf("Receiver program started...\n\n");

    // OPEN COMMUNICATION
    int fd = llopen(argv[1], RECEIVER);
    if (fd == -1) return 1;

    printf("\n#####################\n");
    printf("Connection established\n");
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

		    if(llread_return_value != REJ_SENT)
		     if (buffer[0] == 1) // Write data to file
		      for(int i = 4; i < llread_return_value; i++)
		        fwrite(&buffer[i], 1, 1, filefd);

        if (llread_return_value == -1)
			return 1;

    } while (llread_return_value != 0);

    fclose(filefd);

    printf("\n#####################\n");
    printf("    Reading Over\n");
    printf("#####################\n\n");

    //END COMMUNICATION
    if (llclose(fd) == -1)
        return 1;

    printf("\n#####################\n");
    printf("  Connection ended\n");
    printf("#####################\n\n");

    close(fd);
    return 0;
}
