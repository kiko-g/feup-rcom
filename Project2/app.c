#include "lib.h"


int check_usage(int n, char* s);
int main(int argc, char** argv)
{
    if(check_usage(argc, argv[0])) return -1;
    url_t url = init_URL();

    // parse URL components with argv[1] info
    if (parse_URL(&url, argv[1])) return -1;

    // get URL IP by hostname
    if(getIP_by_hostname(&url)) {
        printf("\nCannot find IP matching hostname: %s.\n", url.host);
        return -1;
    }
}





int check_usage(int n, char* s)
{
    if (n != 2)
    {
        printf("\nWrong executable call (number of arguments must be 2)\n");
        printf("| USAGE |\nNORMAL\t %s ftp://[<user>:<password>@]<host>/<url_path>\n", s);
        printf("ANONYMOUS\t %s ftp://<host>/<url_path>\n\n\n", s);
        return -1;
    }
    
    return 0;
}