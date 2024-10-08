#include "AsyncClient.hpp"

int main(int argc, char const *argv[])
{
    try
    { // Process arguments
        if (argc != 4)
        {
            throw std::runtime_error("Usage : ./submit  <new|status> <serverIP:port>  <sourceCodeFileTobeGraded|requestID>");
            exit(0);
        }
        AsyncClient client(argv[2]);

        if (std::string(argv[1]) == "new")
        {
            // Call the submit function
            client.submit(argv[3]);
        }
        else if (std::string(argv[1]) == "status")
        {
            // Call the status function
            uint32_t req_id = std::stoul(argv[3]);
            client.checkStatus(req_id);
        }
        else
        {
            throw std::invalid_argument("Invalid second argument. Should be 'new' or 'status'.");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}