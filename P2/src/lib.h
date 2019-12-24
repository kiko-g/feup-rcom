#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// ------------- MACROS -------------
#define MAX_SIZE 1024
#define URL_STRLEN 256
// ----------- FTP MACROS -------------
#define PASV "PASV\n"
#define QUIT "QUIT\n"
// ------------- TYPES --------------
typedef struct URL
{
    char ip[URL_STRLEN];
    char host[URL_STRLEN];
    char username[URL_STRLEN];
    char password[URL_STRLEN];
    char filepath[URL_STRLEN];
    char filename[URL_STRLEN];
    int port;
} url_t;
// ----------------------------------
typedef struct FTP
{
    int server_socket_fd;
    int data_socket_fd;
} ftp_t;

// -----------------------------------
// ------- Function declarations -----
// -----------------------------------

/**
 * @brief tells the user how to run the executable
 * @param n number of arguments
 * @param s string provided in executable called
 * @return 0 upon success, 1 otherwise 
 */
void print_usage(char* s);

/** 
 * @brief check executable call usage
 * @param n number of arguments
 * @param array of strings in executable call
 * @return 0 upon success, 1 otherwise 
 */
int check_usage(int n, char** s);

/** 
 * @brief parse information given in executable call into url_t variable
 * @param link string array in executable call
 * @return 0 upon success, 1 otherwise 
 */
int url_parser(url_t* url, char* link);

/** 
 * @brief splits a whole path into path to file and filename
 * @param path initially path (ends up being just path to file)
 * @param file initially path (ends up being just the filename)
 * @return 0 upon success
 */
int split_path(char* path, char* file);

/** 
 * @brief check valid password (any is valid)
 * @param pass password (value depends on executable call)
 * @return final password (char* element)
 */
char* check_password(char* pass);

/** 
 * @brief send ftp message to server
 * @param ftp ftp struct element containing the File Descriptors
 * @param msg message received 
 * @return 0 upon success, 1 otherwise 
 */
int send_msg(ftp_t* ftp, const char* msg);

/** 
 * @brief receive ftp message from server
 * @param ftp ftp struct element containing the File Descriptors
 * @param msg message sent
 * @return 0 upon success
 */
int receive_msg(ftp_t* ftp, char* msg);

/** 
 * @brief connect socket to server
 * @param port server address port
 * @param ip IP address of the server
 * @return integer fd (file descrtptor)
 */
int connect_socket(int port, const char* ip);

/** 
 * @brief connects to server (220)
 * @param ftp ftp struct element containing the File Descriptors
 * @return 0 upon success
 */
int server_connect(int port, const char* ip, ftp_t* ftp);

/** 
 * @brief disconnects from server - asks for final message (226) and sends QUIT
 * @param ftp ftp struct containing the File Descriptors
 * @return 0 upon success
 */
int disconnect(ftp_t* ftp);

/** 
 * @brief login using USER and PASS requests with the information previously parsed
 * @param ftp ftp struct element containing the File Descriptors
 * @param user login username string
 * @param password login password
 * @return 0 upon success
 */
int login(ftp_t* ftp, const char* user, const char* password);

/** 
 * @brief sends a CWD request and tells the server the pathname prefix of the file we want to download
 * @param ftp struct containing the File Descriptors
 * @param path path to the directory of the file
 * @return 0 upon success
 */
int ftp_cwd(ftp_t* ftp, const char* path) ;

/** 
 * @brief enter passive (pasv) mode - sends PASV request and asks the server to accept a data connection
 * @param ftp struct containing the File Descriptors
 * @return 0 upon success
 */
int ftp_pasv(ftp_t* ftp);

/** 
 * @brief sends RETR request and asks the server for the contents of the file specified in filename
 * @param ftp struct element containing the File Descriptors
 * @param filename name of the file to be transfered
 * @return 0 upon success
 */
int ftp_retr(ftp_t* ftp, const char* filename);

/** 
 * @brief writes the intended file specified in filename
 * @param ftp ftp struct containing the File Descriptors
 * @param filename name of file to be downloaded
 * @return 0 upon success
 */
int write_file(ftp_t* ftp, const char* filename);