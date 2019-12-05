#include "lib.h"
ftp_t ftp;
url_t url;


int receive_msg(int fd, char* msg)
{
    bzero(msg, MAX_SIZE);
    read(fd, msg, MAX_SIZE);
    printf("< Received: %s\n", msg);
    return atoi(msg);
}
// ----------------------------------------

int send_msg(int fd, char *msg)
{
    printf("> Sent: %s\n", msg);
    int r = write(fd, msg, strlen(msg));
    return r;
}
// ----------------------------------------

int connect_data(struct sockaddr_in* server_addr, char data[][MAX_SIZE])
{
    char div = ".";
    ftp.data_server_addr = malloc(MAX_SIZE);
    bzero(ftp.data_server_addr, MAX_SIZE);

    //build server address string
    strcpy(ftp.data_server_addr, data[0]);
    strcat(ftp.data_server_addr, div);
    strcat(ftp.data_server_addr, data[1]);
    strcat(ftp.data_server_addr, div);
    strcat(ftp.data_server_addr, data[2]);
    strcat(ftp.data_server_addr, div);
    strcat(ftp.data_server_addr, data[3]);

    ftp.data_server_port = (atoi(data[4]) * 0x100) + atoi(data[5]);

    printf("Data server address: %s\n", ftp.data_server_addr);
    printf("Data server port: %d\n", ftp.data_server_port);


    //server address handling
    bzero((char *)server_addr, sizeof(server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = inet_addr(ftp.data_server_addr);  // 32 bit Internet address network byte ordered
    server_addr->sin_port = htons(ftp.data_server_port);             // server TCP port must be network byte ordered

    //open a TCP socket
    if ((ftp.datafd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket()");
        exit(0);
    }

    //connect to the server
    if (connect(ftp.datafd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0){
        perror("connect()");
        exit(0);
    }

    return ftp.datafd;
}
// ----------------------------------------

int dismantle(char* buf, char data[][MAX_SIZE], char tokens[3])
{
	int i=0, j=0, k=0;
    char init  = tokens[0]; // open bracket
    char end   = tokens[1]; // close bracket
    char split = tokens[2]; // comma
  
	for(i=0; i < MAX_SIZE; i++)
        if(buf[i] == init) {
           i++;    //next position after
           break;  //leave cycle upon init char found
        }


    for(j=0; i < MAX_SIZE && j < MAX_SIZE; i++, j++) //split
    {
		if(buf[i] == split)
        {
			data[k][j] = 0;
			j = 0;
			k++;
			continue; //skip when split
		}
        //leave cycle upon end char found
		if(buf[i] == end) break; 
		data[k][j] = buf[i];
	}

	return 0;
}
// ----------------------------------------

int download_file(int fd, char *filename)
{
    
}
