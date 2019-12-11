#include "lib.h"

// -----------------------------------------------------------------
int check_usage(int argc, char** argv)
{
    if(argc != 2) {
        printf("\nWrong executable call (number of arguments must be 2)\n");
        print_usage(argv[0]);
        return 1;
    }
    else if(argc == 2 && (strncmp("ftp://", argv[1], 6) != 0)) { print_usage(argv[0]); return 1; }
    return 0;
}


// -----------------------------------------------------------------
void print_usage(char* s)
{
    printf("\nThe section in [ ] is optional\n");
    printf("ANONYMOUS     \t%s ftp://<host>/<url_path>\n", s);
    printf("AUTHENTICATION\t%s ftp://[<user>:<password>@]<host>/<url_path>\n\n\n", s);
}


// -----------------------------------------------------------------
int url_parser(url_t* url, char* link)
{
	url->port = 21;
	memset(url->username, 0, sizeof(char) * URL_STRLEN);
	memset(url->password, 0, sizeof(char) * URL_STRLEN);
    memset(url->ip,       0, sizeof(char) * URL_STRLEN);
	memset(url->host,     0, sizeof(char) * URL_STRLEN);
	memset(url->filepath, 0, sizeof(char) * URL_STRLEN);
	memset(url->filename, 0, sizeof(char) * URL_STRLEN);

    if (sscanf(link, "ftp://%[^:]%*[:]%[^@]%*[@]%[^/]%*[/]%s", url->username, url->password, url->host, url->filepath) == 4) 
    {
        split_path(url->filepath, url->filename);
        printf("Parsed url and authentication (anonymous)\n\n");
        printf("Host:      %s\n", url->host);
        printf("Path:      %s\n", url->filepath);
        printf("Filename:  %s\n\n", url->filename);
    }

    else if (sscanf(link, "ftp://%[^/]%*[/]%s", url->host, url->filepath) == 2)
    {
        split_path(url->filepath, url->filename);
        strcpy(url->username, "anonymous");
        strcpy(url->password, ""); //will be altered
        printf("Parsed url with no authentication\n\n");
        printf("Host:      %s\n", url->host);
        printf("Path:      %s\n", url->filepath);
        printf("Filename:  %s\n\n", url->filename);
    }

    else {
        printf("Error while trying to parse url\n");
        print_usage(link);
        return 1;
    }

    return 0;    
}

int split_path(char* path, char* file)
{
    strcpy(file, path);
    size_t len = strlen(path);
    const char* aux = path;
    char final[len];
    int pos = len-1;
    for(size_t i=0; i<len; i++) {
        if(path[pos] == '/') break;
        pos--;
    }

    len = len-pos; //length of filename
    pos++;         //now pos is first char of filename

    for(size_t i=0; i<len; i++)
        file[i] = file[pos+i];

    strncpy(final, aux, pos-1); final[pos-1] = 0;
    strcpy(path, final);

    return 0;
}
// -----------------------------------------------------------------
char* check_password(char* pass)
{
	char* pw;
	if(strlen(pass)) pw = pass;
    else {
		char aux[30];
		printf("Using anonymous mode\nInput your 9 digit UP number as password: ");
		while (strlen(fgets(aux, 30, stdin)) != 10)
			printf("\nInvalid input. Type your 9 digit number: ");

        pw = (char *)malloc(strlen(aux));
        strcpy(pw, aux);
	}
    return pw;
}


// -----------------------------------------------------------------
int send_msg(ftp_t* ftp, const char* msg) 
{
	if ((write(ftp->control_socket_fd, msg, strlen(msg))) <= 0) {
		printf("Could not write (send_msg)\n");
		return 1;
	}

	printf("> Sent: %s\n", msg);

	return 0;
}


// -----------------------------------------------------------------
int receive_msg(ftp_t* ftp, char* msg) 
{
	FILE* f = fdopen(ftp->control_socket_fd, "r");

	while(msg[0] < '1' || msg[0] > '5' || msg[3] != ' '){
		memset(msg, 0, MAX_SIZE);
		msg = fgets(msg, MAX_SIZE, f);
		printf("> Received %s", msg);
	} 

	return 0;
}


// -----------------------------------------------------------------
int connect_to_data(int port, const char* ip)
{
	int fd;
	struct sockaddr_in server_addr;

	bzero((char*) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);    //32 bit Internet address network byte ordered 
	server_addr.sin_port = htons(port);             //server TCP port must be network byte ordered 

	// open an TCP socket
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
		return 1;
	}

	    // connect to the server
	if (connect(fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
		perror("connect()");
		return 1;
	}

	return fd;
}


// -----------------------------------------------------------------
int ftp_connect(int port, const char* ip, ftp_t* ftp) 
{
	int fd;
	char read_buf[MAX_SIZE];

	if ((fd = connect_to_data(port, ip)) < 0) {
		printf("Could not connect socket\n");
		return 1;
	}

	ftp->control_socket_fd = fd;
	ftp->data_socket_fd = 0;

	if (receive_msg(ftp, read_buf)) {
		printf("Could not read info\n");
		return 1;
	}

	return 0;
}


// -----------------------------------------------------------------
int login(ftp_t* ftp, const char* user, const char* password) 
{
	char login[MAX_SIZE];
	sprintf(login, "USER %s\r\n", user);        // username into login string

	if(send_msg(ftp, login))    { printf("Failed to send message with username\n"); return 1; }
	if(receive_msg(ftp, login)) { printf("Failed to receive reply to username sent\n"); return 1; }

	memset(login, 0, sizeof(login));            // cleaning login string
	sprintf(login, "PASS %s\r\n", password);    // password into login string

	if(send_msg(ftp, login))    { printf("Failed to send message with password\n"); return 1; }
	if(receive_msg(ftp, login)) { printf("Failed to receive reply to password sent\n"); return 1; }

	return 0;
}


// -----------------------------------------------------------------
int ftp_cd(ftp_t* ftp, const char* path) 
{
	char cwd[MAX_SIZE];

	sprintf(cwd, "CWD %s\r\n", path);
	if (send_msg(ftp, cwd)) {
		printf("ERROR: Cannot send path to CWD.\n");
		return 1;
	}

	if (receive_msg(ftp, cwd)) {
		printf("ERROR: Cannot send path to change directory.\n");
		return 1;
	}

	return 0;
}


// -----------------------------------------------------------------
int ftp_pasv(ftp_t* ftp)
{
	char pasv[MAX_SIZE] = "PASV\r\n";
	if (send_msg(ftp, pasv))    { printf("Failed to send passive mode msg\n"); return 1; }
	if (receive_msg(ftp, pasv)) { printf("Could not enter passive mode\n"); return 1; }

    int IP[4], P1, P2; // IP Int array, and 2 auxiliar ports
	if ((sscanf(pasv, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &IP[0], &IP[1], &IP[2], &IP[3], &P1, &P2)) < 0){
		//change this
        printf("Could not read information to calculate port\n");
		return 1;
	}

	memset(pasv, 0, sizeof(pasv));  //cleaning pasv string


	if ((sprintf(pasv, "%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3])) < 0) {
		printf("Could\'nt form IP address\n");
		return 1;
	}

	// calculating final port
	int port = (P1 * 0x100) + P2;

	printf("IP:   %s\n", pasv);
	printf("PORT: %d\n", port);

	if ((ftp->data_socket_fd = connect_to_data(port, pasv)) < 0) return 1;

	return 0;
}


// -----------------------------------------------------------------
int ftp_retr(ftp_t* ftp, const char* filename) 
{
	char retr[MAX_SIZE];
	sprintf(retr, "RETR %s\r\n", filename);

	if(send_msg(ftp, retr))    { printf("Could not send RETR message\n"); return 1; }
	if(receive_msg(ftp, retr)) { printf("No information received\n"); return 1; }

	return 0;
}


// -----------------------------------------------------------------
int ftp_write_file(ftp_t* ftp, const char* filename) 
{
	FILE* F;
	int bytes;
	char buf[MAX_SIZE];

	if (!(F = fopen(filename, "w"))) {
		printf("Could not open file with filename: %s\n", filename);
		return 1;
	}

	while( (bytes = read(ftp->data_socket_fd, buf, sizeof(buf)) ) ) {
		if(bytes < 0) { printf("Nothing read from data socket fd\n"); return 1; }
		if((bytes=fwrite(buf, bytes, 1, F)) < 0) { printf("Failed to write data to file\n"); return 1; }
	}

	fclose(F);
	close(ftp->data_socket_fd);

	return 0;
}


// -----------------------------------------------------------------
int ftp_disconnect(ftp_t* ftp)
{
	char disconnect[MAX_SIZE];

	if (receive_msg(ftp, disconnect)) {
		printf("Could not disconnect\n");
		return 1;
	}

	sprintf(disconnect, "QUIT\r\n");

	if (send_msg(ftp, disconnect)) {
		printf("Could not send QUIT message\n");
		return 1;
	}

	if(ftp->control_socket_fd) close(ftp->control_socket_fd);

	return 0;
}

// -----------------------------------------------------------------


