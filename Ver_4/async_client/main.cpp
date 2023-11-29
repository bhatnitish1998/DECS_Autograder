#include "AsyncClient.hpp"

int main(int argc, char const *argv[])
{
    try
    { // Process arguments
        if (argc != 7)
        {
            throw("Usage : ./submit  <new|status> <submission-serverIP:port> <response-serverIP:port> <loopNum> <timeout> <sourceCodeFileTobeGraded|requestID>");
            exit(0);
        }
        AsyncClient client(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));

        if (std::string(argv[1]) == "new")
        {
            // Call the submit function
            client.submit(argv[6]);
        }
        else if (std::string(argv[1]) == "status")
        {
            // Call the status function
            uint32_t req_id = std::stoul(argv[6]);
            client.checkStatus(req_id);
        }
        else
        {
            throw std::invalid_argument("Invalid second argument. Should be 'new' or 'status'.");
        }
    }
    catch (const char *msg)
    {
        std::cerr << msg << std::endl;
    }
    return 0;
}