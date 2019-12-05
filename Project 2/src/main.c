#include "lib.h"
extern ftp_t ftp;
extern url_t url;
/* Client FTP application */


int main(int argc, char* argv[])
{
	int	bytes;
	char msg[MAX_SIZE];
	char read_buf[MAX_SIZE];
	char write_buf[MAX_SIZE];
	struct hostent *h;
	struct sockaddr_in server_addr, data_server_addr;

    if(check_usage(argc, argv[0])) return -1;
    parse(argc, argv);
	ftp.server_port = 21;


    if((h=gethostbyname(url.host)) == NULL) {
        herror("gethostbyname()");
        return 0;
    } 
    else {
        ftp.server_addr = inet_ntoa(*( (struct in_addr*) h->h_addr_list[0]));
    	printf("Connecting ----- %s:%d\n",ftp.server_addr, ftp.server_port);
    }

	
	//server address handling
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ftp.server_addr);  // 32 bit Internet address network byte ordered
    server_addr.sin_port = htons(ftp.server_port);		          // server TCP port must be network byte ordered
    
	//open an TCP socket
	if((ftp.sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    	perror("socket()");
        exit(0);
    }

	//connect to the server
    if(connect(ftp.sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("connect()");
		exit(0);
	}

    //continue if ready 
	if(receive_msg(read_buf,ftp.sockfd) != READY) return 1;

    //
	if(ftp.authenticate)
    {
		strcpy(msg,"USER ");
		strcat(msg, url.username);
		strcat(msg,"\n");

		if(send_msg(msg,ftp.sockfd) <= 0) return 1;
		if(receive_msg(read_buf,ftp.sockfd) != PASSWORD_REQUIRED) return 1;
		
		strcpy(msg,"PASS ");
		strcat(msg, url.password);
		strcat(msg,"\n");

		if(send_msg(msg,ftp.sockfd) <= 0) return 1;
		if(receive_msg(read_buf,ftp.sockfd) != LOGGED_IN) return 1;
	}

	if(send_msg("PASV\n",ftp.sockfd) <= 0) return 1;
	if(receive_msg(read_buf,ftp.sockfd) != ENTERED_PASV_MODE) return 1;

	char reply_buf[6][MAX_SIZE];
    char tokens[3] = "(),";
	dismantle(read_buf, reply_buf, tokens);
	connectToData(reply_buf, &data_server_addr);

	strcpy(msg, "RETR ");
	strcat(msg, url.filepath);
	strcat(msg, "\n");

	if(send_msg(msg, ftp.sockfd) <= 0) return 1;
	if(receive_msg(read_buf,ftp.sockfd) != FILE_STATUS_OK) return 1;
	
	downloadFile(ftp.datafd,url.filepath);

    //closing connection
	if(receive_msg(read_buf,ftp.sockfd) != CLOSE_DATA_CONNECTION) return 1;
	if(send_msg("QUIT\n",ftp.sockfd) <= 0) return 1;
	if(receive_msg(read_buf,ftp.sockfd) != CLOSE_CONTROL_CONNECTION) return 1;

	close(ftp.sockfd);

	return 0;
}