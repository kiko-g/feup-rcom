#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int checkUsage(int n, char* str);
int main(int argc, char **argv)
{
    if(checkUsage(argc, argv[0])) return 1;

}





int checkUsage(int n, char* str)
{
    if (n != 2)
    {
        printf("Wrong executable call (number of arguments must be 2).\n");
        printf("====== USAGE ======\nNORMAL\t %s ftp://[<user>:<password>@]<host>/<url-path>\n", str);
        printf("ANONYMOUS\t %s ftp://<host>/<url-path>\n\n\n", str);
        return 1;
    }
    
    return 0;
}