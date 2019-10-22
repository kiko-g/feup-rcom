int main(int argc, char* argv[])
{
    //argv[0] deve conter ./sender ou ./receiver
    int portnumber;
    char* filename = "";
    if(!strcmp(argv[0], "./sender"))
    {
        if(argc != 3 || argv[1]!='0' || argv[1]!='1'){
            printf("Run the program with './sender <port number> <pathToFile>' ----- (pornumber must be 0 or 1)\n\n");
            return -1;
        }

        else{
            portnumber = atoi(argv[2]);
            strcpy(filename, argv[2]);
        }
    }

    else if(!strcmp(argv[0], "./receiver"))
    {
        if(argc != 2 || argv[1]!='0' || argv[1]!='1'){
            printf("Run the program with './sender <pathToFile>' or './receiver <pathToFile>' ----- (pornumber must be 0 or 1)\n\n");
            return -1;
        }
    }

    else
    {
        printf("Run the program with './sender <portnumber> <pathToFile>' or './receiver <port number>'\n\n");
        printf("Example: ./receiver 0\nExample: ./sender 0 yourfile.txt\n\n");
        return -1;
    }
    
    return 0;
}