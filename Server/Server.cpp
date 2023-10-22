#include "Server.hpp"

Server::Server(const char *port) : port(std::stoi(port))
{
    backlog = 5;
    setup_socket();
}

void Server::setup_socket()
{
    int status;
    addrinfo hints, *servinfo;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    sin_size = sizeof client_addr;

    if ((status = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &servinfo)) != 0)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        exit(1);
    }
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
        throw("server: socket");

    // Bind socket
    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        close(sockfd);
        throw("server: bind");
    }
    if (listen(sockfd, backlog) == -1)
        throw("listen");

    freeaddrinfo(servinfo);
}

void Server::accept_requests()
{
    sockaddr_in client_addr;
    socklen_t client_length = sizeof(client_addr);

    int newsockfd = accept(sockfd, (sockaddr *)&client_addr, &client_length);
    if (newsockfd < 0)
        throw("Error accepting connection");

    std::thread(&Server::thread_function, this, newsockfd).detach();
}

void Server::thread_function(int newsockfd)
{
    Worker worker(newsockfd);
    // cout << "TID: " << this_thread::get_id() << endl;
    worker.process_request();
}
