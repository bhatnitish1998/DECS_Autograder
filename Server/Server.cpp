#include "Server.hpp"

Server::Server(const char *port,const char *pool_size) : port(std::stoi(port)),pool_size(std::stoi(pool_size))
{
    backlog = 5;
    setup_threadpool();
    std::thread(&Server::begin_log,this).detach();
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

void Server::setup_threadpool()
{
    for(int i =0;i<pool_size;i++)
        thread_pool.push_back(std::thread(&Server::threadpool_function,this));
}

void Server::accept_requests()
{
    sockaddr_in client_addr;
    socklen_t client_length = sizeof(client_addr);

    int newsockfd = accept(sockfd, (sockaddr *)&client_addr, &client_length);
    if (newsockfd < 0)
        throw("Error accepting connection");

    //block important for unlocking mutex when out of scope.
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        request_queue.push(newsockfd);
    }
    queue_cond.notify_one();
    
    // std::thread(&Server::thread_function, this, newsockfd).detach();
}

// void Server::thread_function(int newsockfd)
// {
//     Worker worker(newsockfd);
//     // cout << "TID: " << this_thread::get_id() << endl;
//     worker.process_request();
// }

void Server::threadpool_function()
{
    while (true)
    {
        int work_sockfd;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cond.wait(lock,[this](){return !request_queue.empty();});
            work_sockfd = request_queue.front();
            request_queue.pop();
        }

        Worker worker(work_sockfd);
        worker.process_request();

    }
}

// todo: make it noexcept and figure out some other way to handle errors.
void Server::begin_log()
{
    std::ofstream fout("log.txt");
    if(!fout) throw("cannot open log file");

    while(true)
    {   
        sleep(0.5);
        int current_queue_size =0;
        {
            std::unique_lock<std::mutex> (queue_mutex);
            current_queue_size = request_queue.size();
        }
        fout<<current_queue_size<<std::endl;
    }
    fout.close();
}


