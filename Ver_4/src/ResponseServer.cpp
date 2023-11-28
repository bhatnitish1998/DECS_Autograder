#include "ResponseServer.hpp"

ResponseServer::ResponseServer(const char *port, const char *pool_size)
    : port(std::stoi(port)),
      pool_size(std::stoi(pool_size)),
      backlog(5)
{
    setup_threadpool();
    setup_socket();
}
void ResponseServer::setup_threadpool()
{
    for (int i = 0; i < pool_size; i++)
        status_thread_pool.push_back(std::thread(&ResponseServer::statuspool_function, this));
}

void ResponseServer::accept_requests()
{
    sockaddr_in client_addr;
    socklen_t client_length = sizeof(client_addr);

    int newsockfd = accept(sockfd, (sockaddr *)&client_addr, &client_length);
    if (newsockfd < 0)
        throw("Error accepting connection");

    // block important for unlocking mutex when out of scope.
    {
        std::unique_lock<std::mutex> lock(status_queue_mutex);
        status_queue.push(newsockfd);
    }
    status_queue_cond.notify_one();
}

void ResponseServer::setup_socket()
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

void ResponseServer::statuspool_function()
{
    while (true)
    {
        int work_sockfd;
        {
            std::unique_lock<std::mutex> lock(status_queue_mutex);
            status_queue_cond.wait(lock, [this]()
                                   { return !status_queue.empty(); });
            work_sockfd = status_queue.front();
            status_queue.pop();
        }

        ResponseWorker worker(work_sockfd);
        worker.work();
    }
}