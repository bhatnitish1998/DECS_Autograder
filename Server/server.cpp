#include "Worker.hpp"

class Server
{
    int port;
    int sockfd;
    sockaddr_in server_addr;
    int backlog;

    void setup_socket()
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            throw("Error opening socket");

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        int sc = bind(sockfd, (sockaddr *)&server_addr, sizeof(server_addr));
        if (sc < 0)
            throw("Error while binding");

        listen(sockfd, backlog);
        std::cout << "Server ready and listening at " << server_addr.sin_addr.s_addr << ":" << port << std::endl;
    }

public:
    Server(int port) : port(port)
    {
        backlog = 5;
        setup_socket();
    }

    void accept_requests()
    {
        sockaddr_in client_addr;
        socklen_t client_length = sizeof(client_addr);

        int newsockfd = accept(sockfd, (sockaddr *)&client_addr, &client_length);
        if (newsockfd < 0)
            throw("Error accepting connection");

        std::thread(&Server::thread_function, this, newsockfd).detach();
    }

    void thread_function(int newsockfd)
    {
        Worker worker(newsockfd);
        // cout << "TID: " << this_thread::get_id() << endl;
        worker.process_request();
    }
};

int main(int argc, char const *argv[])
{
    // Process arguments
    int port;
    if (argc != 2)
    {
        perror("Usage : ./server <port>");
        exit(1);
    }
    port = std::stoi(argv[1]);

    // int port = 9001;

    try
    {
        Server myserver(port);
        while (1)
            myserver.accept_requests();
    }
    catch (const char *msg)
    {
        std::cerr << msg << std::endl;
    }

    return 0;
}