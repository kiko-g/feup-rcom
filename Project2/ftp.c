#include "lib.h"

// ---------------------------------------------------
static int connect_socket(int port, const char *ip)
{
    int sockfd;
    struct sockaddr_in server_addr;

    // server address handling
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;                   // SOCKET.H
    server_addr.sin_addr.s_addr = inet_addr(ip);        // 32 bit Internet address network byte ordered
    server_addr.sin_port = htons(port);                 // server TCP port must be network byte ordered/

    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        //open an TCP socket
        perror("socket()");
        return -1;
    }


    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        //connect to the server
        perror("connect()");
        return -1;
    }



    return sockfd;
}
// ---------------------------------------------------
// ---------------------------------------------------
int read_FTP(ftp_t* ftp, char* s)
{
    FILE *fp = fdopen(ftp->control_socket_fd, "r");

    while (('1' > s[0] || s[0] > '5') || s[3] != ' ') {
        memset(s, 0, sizeof(s));
        s = fgets(s, sizeof(s), fp);
    }

    return 0;
}
// ---------------------------------------------------
// ---------------------------------------------------
int connect_FTP(ftp_t* ftp, int port, const char* ip)
{
	int socketfd;
	char s[1024];
    if ((socketfd = connect_socket(ip, port)) < 0) {
        printf("Failed while connecting socket\n");
		return -1;
	}


	ftp->control_socket_fd = socketfd;
	ftp->data_socket_fd = 0;


    if(read_FTP(ftp, s) != 0) {
        printf("Failed while reading ftp\n");
		return -1;
	}


	return 0;
}
// ---------------------------------------------------