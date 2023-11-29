#include "ResponseServer.hpp"
/// @brief Response server is created with a given thread pool size and listens on given port
/// @param port Port to bind to
/// @param pool_size Thread pool size
ResponseServer::ResponseServer(const char *port, const char *pool_size)
    : port(std::stoi(port)),
      pool_size(std::stoi(pool_size)),
      backlog(5)
{
    setup_threadpool();
    setup_socket();
}
/// @brief Creates the thread pool
void ResponseServer::setup_threadpool()
{
    for (int i = 0; i < pool_size; i++)
        status_thread_pool.push_back(std::thread(&ResponseServer::statuspool_function, this));
}
/// @brief Accepts a request and puts into queue
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
/// @brief Creates, binds and listens on a socket
void ResponseServer::setup_socket()
{
    int status;
    addrinfo hints, *servinfo, *p;
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
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1)
    {
        throw("setsockopt failed");
    }
    // Bind socket
    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        close(sockfd);
        throw("server: bind");
    }

    if (listen(sockfd, backlog) == -1)
        throw("listen");
    for (p = servinfo; p != nullptr; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        { // IPv4
            sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in *>(p->ai_addr);
            char ip4[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip4, INET_ADDRSTRLEN);
            std::cout << "Server ready and listening at " << ip4 << ":" << port << std::endl;
            break;
        }
    }
    freeaddrinfo(servinfo);
}
/// @brief Functions executed by the threads in the thread pool
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
        // Does the actual work of sending response
        ResponseWorker worker(work_sockfd);
        worker.work();
    }
}