#include "headers/Submission.hpp"

SubmissionServer::SubmissionServer(const char *port, const char *pool_size)
    : port(std::stoi(port)),
      pool_size(std::stoi(pool_size)),
      backlog(5)
{
    setup_threadpools();
    setup_socket();
}

void SubmissionServer::setup_socket()
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

void SubmissionServer::setup_threadpools()
{
    init_submissionPool();
    init_gradingPool();
}

void SubmissionServer::init_submissionPool()
{
    for (int i = 0; i < pool_size; i++)
        submit_thread_pool.push_back(std::thread(&SubmissionServer::submissionPoolFunction, this));
}

void SubmissionServer::init_gradingPool()
{
    for (int i = 0; i < pool_size; i++)
        grader_thread_pool.push_back(std::thread(&SubmissionServer::gradingPoolFunction, this));
}

void SubmissionServer::submissionPoolFunction()
{
}

void SubmissionServer::submissionPoolFunction()
{
}