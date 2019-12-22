#include "lib.h"


int main(int argc, char** argv)
{
    printf("\n");
    printf("-------------------------------------------------------\n");
    printf("------------ This server is ANONYMOUS only ------------\n");
    printf("--- Any USER introduced will be read as 'anonymous' ---\n");
    printf("-------------------------------------------------------\n");
    if(check_usage(argc, argv)) return 1; //check executable call

    //variables
    url_t url;
	ftp_t ftp;
	struct hostent* h;
    
    if(url_parser(&url, argv[1])) return 1; //parse info given in argv[1] altering url variable
    
    //------------------- get IP with host --------------------
    printf("\n=== Getting IP with host ===\n");
	if ((h = gethostbyname(url.host)) == NULL) {
		herror("gethostbyname");
		return 1;
	}
	char* ip = inet_ntoa(*((struct in_addr *) h->h_addr_list[0]));
	strcpy(url.ip, ip);
    printf("IP: %s\n\n", url.ip);
    //---------------------------------------------------------

	if(ftp_connect(url.port, url.ip, &ftp)) return 1;   //connect to the server
	strcpy(url.password, check_password(url.password)); //check password, alter it if need be
    if(login(&ftp, url.username, url.password)) {
        printf("Couldn\'t login as user %s\n", url.username);
        return 1;
    }
    //---------------------------------------------------------
	if (ftp_cwd(&ftp, url.filepath)) {
		printf("Couldn\'t change directory of the file (%s)\n", url.filename);
		return 1;
	}
    //---------------------------------------------------------
	if (ftp_pasv(&ftp)) {
		printf("Couldn\'t enter in passive mode\n");
		return 1;
	}
    //---------------------------------------------------------
	ftp_retr(&ftp, url.filename);           //get file
	ftp_write_file(&ftp, url.filename);     //download file
	ftp_disconnect(&ftp);                   //exit

    return 0;
}