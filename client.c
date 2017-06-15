#include "client.h"

int main(int argc, char* argv[])
{
    int opt, c = 0;
    char *ip, fileName[MAX_FILE_NAME_LENGTH], mode = 0;
    enum NetworkProtocol protocol;
    enum CommandCode command;
    NetworkMetaInfo info;

    while((opt = getopt(argc, argv, "l:f:p:rdu")) != -1)
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
                break;

            case 'r':
                command = DELETE;
                if (c)
                {
                    printClientHelp(argv[0]);
                }
                c = 1;
                break;
            case 'd':
                command = LIST;
                if (c)
                {
                    printClientHelp(argv[0]);
                }
                c = 1;
                break;
            case 'u':
                command = UPLOAD;
                if (c)
                {
                    printClientHelp(argv[0]);
                }
                c = 1;
                break;
            default:
                break;
        }
    }

    if (!c)
    {
        command = UPLOAD;
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

    if (command == UPLOAD)
    {
        sendFile(&info, "", fileName, command);
    }
    else if (command == LIST)
    {
        //
    }
    else if (command == DELETE)
    {
        deleteFile(&info, "", fileName, command);
    }
    else
    {
        // error
    }


    return 0;
}