#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
typedef char string[256];

/** 
 * @brief initialize URL struct with default values
 * @return the initialized URL struct object
 */
URL initURL();

/** 
 * @brief parses URL information
 * @param url
 * @param url_string
 * @return 0 upon success, non-zero otherwise
*/
int parseURL(URL* url, const char* url_string);



typedef struct URL_t {
    string ip;          // IP adress string
    string host;        // host string
    string path;        // path string
    string user;        // user string
    string password;    // password string
    string filename;    // filename string
    int port;
} URL;