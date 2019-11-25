#include "lib.h"

URL initURL() {
    URL url;
    strcpy(url.ip, "");
    strcpy(url.host, "");
    strcpy(url.path, "");
    strcpy(url.user, "");
    strcpy(url.password, "");
    strcpy(url.filename, "");
    url.port = 21;
    return url;
}


int parseURL(URL* url, const char* url_string)
{
    
}