#include "lib.h"

url_t init_URL() 
{
    url_t url;
    size_t sz = sizeof(string);

    url.port = 21;
    memset(url.user, 0, sz);
    memset(url.password, 0, sz);
    memset(url.host, 0, sz);
    memset(url.path, 0, sz);
    memset(url.filename, 0, sz);

    return url;
}



int parse_URL(url_t* url, const char* url_string)
{
    return 0;
}

int getIpByHost(url_t* url)
{
	struct hostent* h;
	if ((h = gethostbyname(url->host)) == NULL) {
		printf("Error while trying to get hostname\n");
		return -1;
	}

	char* temp = inet_ntoa(*((struct in_addr *) h->h_addr_list[0]));
    strcpy(url->ip, temp);

    return 0;
}