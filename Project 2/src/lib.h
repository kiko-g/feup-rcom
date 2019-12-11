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
    int control_socket_fd;
    int data_socket_fd;
} ftp_t;

// -----------------------------------
// ------- Function declarations -----
// -----------------------------------

/**
 * @brief check executable call usage (must have 2 args (n should be 2))
 * @param n number of arguments
 * @param s string provided in executable called
 * @return 0 upon success, 1 otherwise 
 */
void print_usage(char* s);

/** 
 * @brief check executable call usage (must have 2 args (n should be 2))
 * @param n number of arguments
 * @param array of strings in executable call
 * @return 0 upon success, 1 otherwise 
 */
int check_usage(int n, char** s);

/** 
 * @brief parse initial information
 * @param argv string array in executable call
 * @return 0 upon success, 1 otherwise 
 */
int url_parser(url_t* url, char* link);

/** 
 * @brief check valid password
 * @param pass url password
 * @return 0 upon success
 */
int split_path(char* path, char* file);

/** 
 * @brief check valid password
 * @param pass url password
 * @return 0 upon success
 */
char* check_password(char* pass);

/** 
 * @brief send ftp message
 * @param server_addr
 * @param data
 * @return 0 upon success, 1 otherwise 
 */
int send_msg(ftp_t* ftp, const char* msg);

/** 
 * @brief receive ftp message
 * @param server_addr
 * @param data
 * @return 0 upon success
 */
int receive_msg(ftp_t* ftp, char* msg);

/** 
 * @brief connect socket
 * @param ftp ftp struct containing the File Descritors
 * @return 0 upon success
 */
int connect_to_data(int port, const char* ip);

/** 
 * @brief connect
 * @param ftp ftp struct containing the File Descritors
 * @return 0 upon success
 */
int ftp_connect(int port, const char* ip, ftp_t* ftp);

/** 
 * @brief disconnect from server
 * @param ftp ftp struct containing the File Descriptors
 * @return 0 upon success
 */
int ftp_disconnect(ftp_t* ftp);

/** 
 * @brief 
 * @param ftp ftp struct containing the File Descritors
 * @param user login username string
 * @param password login password
 * @return 0 upon success
 */
int login(ftp_t* ftp, const char* user, const char* password);

/** 
 * @brief change to directory of the file
 * @param ftp struct containing the File Descriptors
 * @param path path to the directory of the file
 * @return 0 upon success
 */
int ftp_cd(ftp_t* ftp, const char* path) ;

/** 
 * @brief enter passive (pasv) mode
 * @param ftp struct containing the File Descriptors
 * @return 0 upon success
 */
int ftp_pasv(ftp_t* ftp);

/** 
 * @brief 
 * @param ftp struct containing the File Descritors
 * @param filename name of the file to be transfered
 * @return 0 upon success
 */
int ftp_retr(ftp_t* ftp, const char* filename);

/** 
 * @brief 
 * @param ftp ftp struct containing the File Descritors
 * @param filename name of file to be downloaded
 * @return 0 upon success
 */
int ftp_write_file(ftp_t* ftp, const char* filename);