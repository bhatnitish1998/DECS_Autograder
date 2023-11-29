#include "SubmissionServer.hpp"
SubmissionServer::SubmissionServer(const char *port, const char *pool_size)
    : port(std::stoi(port)),
      pool_size(std::stoi(pool_size)),
      backlog(5)
{
    setup_threadpools();
    setup_socket();
}

void SubmissionServer::accept_requests()
{
    sockaddr_in client_addr;
    socklen_t client_length = sizeof(client_addr);

    int newsockfd = accept(sockfd, (sockaddr *)&client_addr, &client_length);
    if (newsockfd < 0)
        throw("Error accepting connection");

    // block important for unlocking mutex when out of scope.
    {
        std::unique_lock<std::mutex> lock(submission_queue_mutex);
        submission_queue.push(newsockfd);
    }
    submission_queue_cond.notify_one();
}

void SubmissionServer::setup_socket()
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
    while (true)
    {
        int work_sockfd;
        Database db;
        {
            std::unique_lock<std::mutex> lock(submission_queue_mutex);
            submission_queue_cond.wait(lock, [this]()
                                       { return !submission_queue.empty(); });
            work_sockfd = submission_queue.front();
            submission_queue.pop();
        }

        SubmissionWorker worker(work_sockfd);
        auto req_id = worker.work();
        // Add as new grading request and notify grading threads
        {
            std::unique_lock<std::mutex> lock(grader_queue_mutex);
            grader_queue.push(req_id);
        }
        grader_queue_cond.notify_one();
    }
}

void SubmissionServer::gradingPoolFunction()
{
    while (true)
    {
        int work_req_id;
        {
            std::unique_lock<std::mutex> lock(grader_queue_mutex);
            grader_queue_cond.wait(lock, [this]()
                                   { return !grader_queue.empty(); });
            work_req_id = grader_queue.front();
            grader_queue.pop();
        }

        GradingWorker worker(work_req_id);
        worker.work();
    }
}