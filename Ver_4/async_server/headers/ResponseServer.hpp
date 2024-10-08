#ifndef __Response_Server_hpp__
#define __Response_Server_hpp__

#include <iostream>
#include <netdb.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <thread>
#include "Worker.hpp"
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <arpa/inet.h>

class ResponseServer
{
    int port;
    int sockfd;
    sockaddr_in client_addr;
    socklen_t sin_size;
    int backlog;
    std::vector<std::thread> status_thread_pool;
    std::queue<int> status_queue;
    int pool_size;
    std::mutex status_queue_mutex;
    std::condition_variable status_queue_cond;

    void setup_socket();
    void statuspool_function();
    void setup_threadpool();
    void begin_log();

public:
    ResponseServer(const char *port, const char *pool_size);
    void accept_requests();
};

#endif