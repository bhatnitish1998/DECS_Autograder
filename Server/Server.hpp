#ifndef __Server_hpp__
#define __Server_hpp__

#include <iostream>
#include <netdb.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <thread>
#include "Worker.hpp"

class Server
{
    int port;
    int sockfd;
    sockaddr_in client_addr;
    socklen_t sin_size;
    int backlog;
    void setup_socket();
    void thread_function(int newsockfd);

public:
    Server(const char *port);
    void accept_requests();
};

#endif