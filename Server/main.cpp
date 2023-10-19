#include "Server.hpp"

int main(int argc, char const *argv[])
{
    // Process arguments
    int port;
    if (argc != 2)
    {
        perror("Usage : ./server <port>");
        exit(1);
    }

    try
    {
        Server myserver(argv[1]);
        while (1)
            myserver.accept_requests();
    }
    catch (const char *msg)
    {
        std::cerr << msg << std::endl;
    }

    return 0;
}