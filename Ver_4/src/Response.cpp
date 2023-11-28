#include "ResponseServer.hpp"

int main(int argc, char const *argv[])
{
    // Process arguments
    int port;
    if (argc != 3)
    {
        perror("Usage : ./response_server <port> <thread_pool_size>");
        exit(1);
    }

    try
    {
        ResponseServer myserver(argv[1], argv[2]);
        while (1)
            myserver.accept_requests();
    }
    catch (const char *msg)
    {
        std::cerr << msg << std::endl;
        perror("Error string is:");
    }

    return 0;
}