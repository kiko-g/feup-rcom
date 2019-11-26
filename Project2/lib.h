#pragma once
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// ---------------------------------------------------
#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"
// ---------------------------------------------------
typedef char string[256];


// ---------------------------------------------------
// ---------------        URL        -----------------
// ---------------------------------------------------
typedef struct URL
{
    string ip;       // IP adress string
    string host;     // host string
    string path;     // path string
    string user;     // user string
    string password; // password string
    string filename; // filename string
    int port;
} url_t;


/** @brief initialize URL struct with default values
 *  @return the initialized URL struct object */
url_t init_URL();


/** 
 * @brief parses URL information into url param fields
 * @return 0 upon success, non-zero otherwise */
int parse_URL(url_t* url, const char* url_string);


/** 
 * @brief gets IP by hostname
 * @return 0 upon success, non-zero otherwise */
int getIP_by_hostname(url_t* url); // gets an IP by host name




// ---------------------------------------------------
// ---------------        FTP        -----------------
// ---------------------------------------------------
typedef struct FTP {
    int control_socket_fd; // control socket file descriptor
    int data_socket_fd;    // data socket file descriptor
} ftp_t;


int read_FTP(ftp_t* ftp, char* s);
int connect_FTP(ftp_t* ftp, int port, const char* ip);
static int connect_socket(int port, const char* ip);
