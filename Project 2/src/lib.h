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
// ----------------------------------
#define MAX_SIZE 2048
#define READY 220
#define LOGGED_IN 230
#define FILE_STATUS_OK 150
#define PASSWORD_REQUIRED 331
#define ENTERED_PASV_MODE 227
#define CLOSE_DATA_CONNECTION 226
#define CLOSE_CONTROL_CONNECTION 221
// ----------------------------------
typedef struct FTP
{
    int max_size;
    int sockfd;
    int datafd;
    int server_port;
    int data_server_port;
    int authenticate;
    char* server_addr;
    char* data_server_addr;

} ftp_t;
// ----------------------------------
typedef struct URL
{
    char host[256];
    char username[256];
    char password[256];
    char filepath[256];

} url_t;

/*-----------------------------------
------- Function declarations -------
-----------------------------------*/

/** 
 * @brief parse initial information
 * @param argc number of arguments
 * @param argv string array in executable call
 */
void parse(int argc, char *argv[]);


/** 
 * @brief check executable call usage (must have 2 args (n should be 2))
 * @param n number of arguments
 * @param s string provided in executable called
 * @return 0 upon success, 1 otherwise 
 */
int check_usage(int n, char *s);


/** 
 * @brief send ftp message
 * @param server_addr
 * @param data
 * @return 0 upon success, 1 otherwise 
 */
int send_msg(int fd, char* msg);


/** 
 * @brief receive ftp message
 * @param server_addr
 * @param data
 * @return 0 upon success, 1 otherwise 
 */
int receive_msg(int fd, char* msg);


/** 
 * @brief 
 * @param server_addr
 * @param data
 * @return 0 upon success, 1 otherwise 
 */
int download_file(int fd, char* filename);


/** 
 * @brief connect data to the server address
 * @param server_addr
 * @param data
 * @return 0 upon success, 1 otherwise
 */
int connect_data(struct sockaddr_in* server_addr, char data[][MAX_SIZE]);


/** 
 * @brief dismantle __ strings with tokens
 * @param buf 
 * @param data 
 * @param tokens contains 3 token charecters to dismantle strings
 * @return 0 upon success, 1 otherwise 
 */
int dismantle(char *buf, char data[][MAX_SIZE], char tokens[3]);