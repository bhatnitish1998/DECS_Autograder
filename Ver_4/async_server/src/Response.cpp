#include "ResponseServer.hpp"

int main(int argc, char const *argv[])
{
    // Process arguments
    int port;
    if (argc != 3)
    {
        std::cerr << "Usage : ./response_server <port> <thread_pool_size>\n";
        exit(1);
    }

    try
    {
        ResponseServer myserver(argv[1], argv[2]);
        while (1)
            myserver.accept_requests();
    }
    catch (const std::exception &e)
    {
        perror("Error string is:");
        std::cerr << e.what() << std::endl;
    }

    return 0;
}