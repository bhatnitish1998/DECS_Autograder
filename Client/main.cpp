#include "Client.hpp"


int main(int argc, char const *argv[])
{
    try
    {   // Process arguments
        if (argc != 5)
        {
            throw("Usage : ./client <serverip:port> <loopNum> <sleepTime> <timeout>");
            exit(0);
        }
        Client client(argv[1], argv[2], argv[3], argv[4]);
        client.submit();
    }
    catch (const char *msg)
    {
        std::cerr << msg << std::endl;
    }
    return 0;
}