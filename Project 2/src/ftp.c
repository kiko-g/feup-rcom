#include "lib.h"
ftp_t ftp;
url_t url;


//n is argc and s is argv[0]
int check_usage(int n, char* s)
{
    if(n!=2)
    {
        printf("\nWrong executable call (number of arguments must be 2)\n\n");
        printf("NORMAL\t\t%s ftp://[<user>:<password>@]<host>/<url_path>\n", s);
        printf("ANONYMOUS\t%s ftp://<host>/<url_path>\n\n\n", s);
        return 1;
    }
    return 0;
}


int parse(int argc, char *argv[])
{
    if (sscanf(argv[1], "ftp://%[^:]%*[:]%[^@]%*[@]%[^/]%*[/]%s", url.username, url.password, url.host, url.filepath) == 4)
    {
        ftp.authenticate = true;
        char hidden_pass[PASSWORD_MAX_LENGTH] = "";
        for(int i=0; i < strlen(url.password); ++i) strcat(hidden_pass, "*");

        printf("Parsed url and authentication\n\n");
        printf("Username:  %s\n", url.username);
        printf("Password:  %s\n", hidden_pass);
        printf("Host:      %s\n", url.host);
        printf("Filepath:  %s\n\n", url.filepath);
    }

    else if (sscanf(argv[1], "ftp://%[^/]%*[/]%s", url.host, url.filepath) == 2)
    {
        ftp.authenticate = false;
        printf("Parsed url with no authentication\n\n");
        printf("Host:      %s\n", url.host);
        printf("Filepath:  %s\n\n", url.filepath);
    }

    else{
        printf("Error while trying to parse url and/or authentication\n");
        return 1;
    }

    return 0;
}


int receive_msg(int fd, char* msg)
{
    bzero(msg, MAX_SIZE);
    read(fd, msg, MAX_SIZE);
    printf("> Received: %s\n", msg);
    return atoi(msg);
}


int send_msg(int fd, char *msg)
{
    printf("> Sent: %s\n", msg);
    int r = write(fd, msg, strlen(msg));
    return r;
}


int connect_data(struct sockaddr_in* server_addr, char data[][MAX_SIZE])
{
    ftp.data_server_addr = malloc(MAX_SIZE);
    bzero(ftp.data_server_addr, MAX_SIZE);

    //build server address string using "." division
    strcpy(ftp.data_server_addr, data[0]);
    strcat(ftp.data_server_addr, ".");
    strcat(ftp.data_server_addr, data[1]);
    strcat(ftp.data_server_addr, ".");
    strcat(ftp.data_server_addr, data[2]);
    strcat(ftp.data_server_addr, ".");
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


int dismantle(char* buf, char data[][MAX_SIZE], char tokens[3])
{
    //tokens[0] = (
    //tokens[1] = )
    //tokens[2] = ,
	int i=0, j=0, k=0;

    for(i=0; i < MAX_SIZE; i++)
        if(buf[i] == tokens[0]) {
           i++;    //next position after
           break;  //leave cycle upon init char found
        }


    for(j=0; i < MAX_SIZE && j < MAX_SIZE; i++, j++) //split
    {
		if(buf[i] == tokens[2])
        {
			data[k][j] = 0;
			j = 0;
			k++;
			continue; 
            //skip when split character is found
		}
        //leave cycle upon end char found
		if(buf[i] == tokens[1]) break; 
		data[k][j] = buf[i];
	}

	return 0;
}


int download_file(int fd, char *filename)
{
	int bytes;
	int file = open(filename, O_WRONLY | O_APPEND | O_CREAT | O_EXCL, 0666);

	while(bytes > 0)
    {
		char buf[MAX_SIZE];
        bzero(buf, MAX_SIZE);

        if ((bytes = read(fd, buf, MAX_SIZE)) < 0)
            printf("Error while reading file from server");

        if (write(file, buf, bytes) < 0)
            printf("Error while writing to file\n");
	}

	return 0;    
}

