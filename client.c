#include "client.h"

int main(int argc, char* argv[])
{
    int opt;
    char *ip, fileName[MAX_FILE_NAME_LENGTH], mode = 0;
    enum NetworkProtocol protocol;
    NetworkMetaInfo info;

    while((opt = getopt(argc, argv, "l:f:p:")) != -1)
    {
        switch(opt)
        {
            case 'l':
                ip = malloc(sizeof(optarg));
                strcpy(ip, optarg);
                mode |= MODE_IP;
                break; 
            case 'f':
                strcpy(fileName, optarg);
                mode |= MODE_FILE;
                break;
            case 'p':
                if (!strcmp(optarg, "TCP"))
                {
                    protocol = TCP;
                }
                else if (!strcmp(optarg, "UDP"))
                {
                    protocol = UDP;
                }
                else
                {
                    printClientHelp(argv[0]);
                }

                mode |= MODE_PROTOCOL;
            default:
                break;
        }
    } 

    if (!(mode & MODE_IP))
    {
        ip = "127.0.0.1";
    }

    if (!(mode & MODE_FILE))
    {
        printClientHelp(argv[0]);
    }

    if (!(mode & MODE_PROTOCOL))
    {
        protocol = AUTO;
    }

    memcpy(info.ip, ip, 16);
    info.port = PORT;
    info.protocol = protocol;

    sendFile(&info, "", fileName);

    return 0;
}