#include "client.h"

int main(int argc, char* argv[])
{
    int sock;
    char* ip;
    struct sockaddr_in* server_addr;

    if (argc < 2)
    {
        ip = "127.0.0.1";
    }
    else
    {
        ip = argv[1];
    }

    sock = makeSocket();
    server_addr = connectSocket(ip, PORT);

    // sendFile(sock, server_addr, file, sizeof(file));

    closeSocket(sock, server_addr);

    return 0;
}