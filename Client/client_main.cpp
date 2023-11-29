#include "Client.hpp"


int main(int argc, char const *argv[])
{
    try
    {   // Process arguments
        if (argc != 6)
        {
            throw("Usage : ./client <serverip:port> <loopNum> <sleepTime> <timeout> <file_to_submit>");
            exit(0);
        }
        Client client(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]),argv[5]);
        client.submit();
    }
    catch (const char *msg)
    {
        std::cerr << msg << std::endl;
    }
    return 0;
}