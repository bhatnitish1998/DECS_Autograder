#ifndef __Server_hpp__
#define __Server_hpp__

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
#include <fcntl.h>
#include <arpa/inet.h>
#include<exception>

#define CONTROL_PORT 5001

class Server
{
    int port;
    int sockfd;
    sockaddr_in client_addr;
    socklen_t sin_size;
    int backlog;

    int control_sockfd;
    int new_control_sockfd;
    double service_time;
    std::mutex service_mutex;
    

    void setup_socket();
    void thread_function(int newsockfd);
    void setup_control();
    uint32_t receive_long();
    void control_thread_function();
    double get_cpu_utilization();
    int get_threads();
    void log_data(std::ofstream&);



public:
    Server(const char *port);
    void accept_requests();
};

#endif