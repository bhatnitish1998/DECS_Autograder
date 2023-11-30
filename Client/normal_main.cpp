#include "normal_client.hpp"

int main(int argc, char const *argv[])
{
    try
    { // Process arguments
        if (argc != 3)
        {
            throw std::runtime_error("Usage : ./client <serverip:port> <file_to_submit>");
            exit(0);
        }
        Client client(argv[1], argv[2]);
        client.submit();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}