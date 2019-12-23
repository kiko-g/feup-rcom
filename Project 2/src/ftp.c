#include "lib.h"
int file_size = 0;
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
        printf("\n============ Parsed url and authentication ============\n\n");
        printf("User:      %s\n", url->username);
        printf("Pass:      %s\n", url->password);
        printf("Host:      %s\n", url->host);
        printf("Path:      %s\n", url->filepath);
        printf("Filename:  %s\n", url->filename);
        memset(url->username, 0, sizeof(char) * URL_STRLEN);
        strcpy(url->username, "anonymous");
    }

    else if (sscanf(link, "ftp://%[^/]%*[/]%s", url->host, url->filepath) == 2)
    {
        split_path(url->filepath, url->filename);
        strcpy(url->username, "anonymous");
        strcpy(url->password, ""); //will be altered later
        printf("\n=== Parsed url with no authentication (anonymous) ===\n\n");
        printf("Host:      %s\n", url->host);
        printf("Path:      %s\n", url->filepath);
        printf("Filename:  %s\n", url->filename);
    }

    else {
        printf("Error while trying to parse url\n");
        print_usage(link);
        return 1;
    }

    return 0;    
}

// -----------------------------------------------------------------
int split_path(char* path, char* file)
{
   strcpy(file, path);       
   size_t len = strlen(path);
   int pos = len-1;
   char final[len];
   const char* aux = path;   

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
    //if it was specified in executable call no changes
	if(strlen(pass)) pw = pass; 
    //else type it now
    else {
		char aux[20];
		printf("Using server in ANONYMOUS mode\nType in any password: ");
		while (strlen(fgets(aux, 20, stdin)) < 1)
			printf("\nInvalid input. Try again: ");

        pw = (char *)malloc(strlen(aux));
        strcpy(pw, aux);
	}
    return pw; //final password
}


// -----------------------------------------------------------------
int send_msg(ftp_t* ftp, const char* msg) 
{
	if ((write(ftp->server_socket_fd, msg, strlen(msg))) <= 0) {
		printf("Couldn\'t write (send_msg)\n");
		return 1;
	}

	printf("\n> Sent: %s", msg);

	return 0;
}


// -----------------------------------------------------------------
int receive_msg(ftp_t* ftp, char* msg) 
{
	FILE* f = fdopen(ftp->server_socket_fd, "r");
    memset(msg, 0, MAX_SIZE);

	while(msg[3] != ' ') {
		memset(msg, 0, MAX_SIZE);
		msg = fgets(msg, MAX_SIZE, f);
		printf("> Received: %s", msg);
	}
	return 0;
}


// -----------------------------------------------------------------
int connect_socket(int port, const char* ip)
{
	int fd;
	struct sockaddr_in server_addr;

	bzero((char*) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);    //32 bit Internet address network byte ordered 
	server_addr.sin_port = htons(port);             //server TCP port must be network byte ordered 

	// open a TCP socket
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
	char read_buf[MAX_SIZE];

	if ((ftp->server_socket_fd = connect_socket(port, ip)) < 0) {
		printf("Couldn\'t connect socket\n");
		return 1;
	}

	if (receive_msg(ftp, read_buf)) {
		printf("Couldn\'t read info\n");
		return 1;
	}

	return 0;
}


// -----------------------------------------------------------------
int login(ftp_t* ftp, const char* user, const char* password) 
{
	char login[MAX_SIZE];
	sprintf(login, "USER %s\n", user);        // username into login string

	if(send_msg(ftp, login))    { printf("Failed to send message with username (USER)\n"); return 1; }
	if(receive_msg(ftp, login)) { printf("Failed to receive reply to username sent (USER)\n"); return 1; }

	memset(login, 0, sizeof(login));            // cleaning login string
	sprintf(login, "PASS %s\n", password);    // password into login string

	if(send_msg(ftp, login))    { printf("Failed to send message with password (PASS)\n"); return 1; }
	if(receive_msg(ftp, login)) { printf("Failed to receive reply to password sent (PASS)\n"); return 1; }

	return 0;
}


// -----------------------------------------------------------------
int ftp_cwd(ftp_t* ftp, const char* path) 
{
	char cwd[MAX_SIZE];

	sprintf(cwd, "CWD %s\n", path);
	if (send_msg(ftp, cwd)) {
		printf("Failed to send change directory message (CWD)\n");
		return 1;
	}

	if (receive_msg(ftp, cwd)) {
		printf("Couldn\'t receive acceptance directory messages (CWD)\n");
		return 1;
	}

	return 0;
}


// -----------------------------------------------------------------
int ftp_pasv(ftp_t* ftp)
{
	char pasv[MAX_SIZE] = PASV;
	if (send_msg(ftp, pasv))    { printf("Failed to send passive mode msg\n"); return 1; }
	if (receive_msg(ftp, pasv)) { printf("Couldn\'t enter passive mode\n"); return 1; }

    int IP[4], P1, P2; // IP array, and 2 auxiliar ports
    //P1 and P2 are the port the server is telling the client to use during the data transfer
	if ((sscanf(pasv, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &IP[0], &IP[1], &IP[2], &IP[3], &P1, &P2)) < 0){
        printf("Couldn\'t parse information to calculate port correctly\n");
		return 1;
	}

	memset(pasv, 0, sizeof(pasv));  //cleaning pasv string


	if ((sprintf(pasv, "%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3])) < 0) {
		printf("Couldn\'t generate IP address\n");
		return 1;
	}

	// calculating final ftp pasv data port
	int port = (P1 * 256) + P2;

	printf("IP:   %s\n", pasv);
	printf("PORT: %d\n", port);

    //if fd has invalid value abort ()
	if ((ftp->data_socket_fd = connect_socket(port, pasv)) < 0) return 1;

	return 0;
}


// -----------------------------------------------------------------
void progress(char* s)
{
    int init_pos, final_pos;
    for(int i=0; i<MAX_SIZE; ++i)
        if(s[i]==' ' && s[i+1]=='b' && s[i+2]=='y' && s[i+3]=='t' && s[i+4]=='e' && s[i+5]=='s') {
            final_pos = i-1;
            break;
        }

    for(int i=final_pos; i > 0; --i)
        if(s[i] == '(') {
            init_pos = i+1;
            break;
        }
    
    char number[20];
    for(int i=0; i<20; ++i) {
        number[i] = s[init_pos+i];
        if(init_pos+i == final_pos) break;
    }
    number[final_pos - init_pos + 1] = 0;

    file_size = atoi(number);
}

int ftp_retr(ftp_t* ftp, const char* filename) 
{
	char retr[MAX_SIZE];
	sprintf(retr, "RETR %s\n", filename);

	if(send_msg(ftp, retr))    { printf("Couldn\'t send RETR message\n"); return 1; }
	if(receive_msg(ftp, retr)) { printf("No information received (RETR)\n"); return 1; }
    progress(retr);

	return 0;
}
// -----------------------------------------------------------------
int ftp_write_file(ftp_t* ftp, const char* filename) 
{
	FILE* F;
	int bytes;
	char buf[MAX_SIZE];

	if (!(F = fopen(filename, "w"))) {
		printf("Couldn\'t open file with filename: %s\n", filename);
		return 1;
	}

	while( (bytes = read(ftp->data_socket_fd, buf, sizeof(buf)) ) ) {
		if(bytes < 0) { printf("Nothing read from data socket fd\n"); return 1; }
		if((bytes = fwrite(buf, bytes, 1, F)) < 0) { printf("Failed to write data to file\n"); return 1; }
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
		printf("Couldn\'t receive disconnect message\n");
		return 1;
	}

	sprintf(disconnect, QUIT);

	if (send_msg(ftp, disconnect)) {
		printf("Couldn\'t send QUIT message\n");
		return 1;
	}

	if(ftp->server_socket_fd) close(ftp->server_socket_fd);

	return 0;
}


