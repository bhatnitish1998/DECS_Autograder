#include "Client.hpp"


int main(int argc, char const *argv[])
{
    try
    {   // Process arguments
        if (argc != 6)
        {
            throw("Usage : ./client <serverip:port> <filetosubmit> <loopNum> <sleepTime> <timeout>");
            exit(0);
        }
        Client client(argv[1], argv[2], argv[3], argv[4], argv[5]);
        client.submit();
    }
    catch (const char *msg)
    {
        std::cerr << msg << std::endl;
    }
    return 0;
}