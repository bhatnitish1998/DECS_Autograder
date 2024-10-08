#ifndef __Submission_Server_hpp__
#define __Submission_Server_hpp__
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
#include "FastQueue.h"
#define CONTROL_PORT 5001
/// @brief Class representing the submission server
class SubmissionServer
{
    int port;
    int pool_size;
    int backlog;
    int sockfd;

    sockaddr_in client_addr;
    socklen_t sin_size;

    std::vector<std::thread> submit_thread_pool;
    std::vector<std::thread> grader_thread_pool;

    std::queue<int> submission_queue;
    std::queue<uint32_t> grader_queue;

    std::mutex submission_queue_mutex;
    std::condition_variable submission_queue_cond;
    std::mutex grader_queue_mutex;
    std::condition_variable grader_queue_cond;

    void setup_socket();
    // void thread_function(int newsockfd);
    void submissionPoolFunction();
    void gradingPoolFunction();
    void init_submissionPool();
    void init_gradingPool();
    void setup_threadpools();
    void recoverGradingQueue();

    // Performance collection stuff
    int control_sockfd;
    int new_control_sockfd;
    double service_time;
    std::mutex service_mutex;

    void setup_control();
    uint32_t receive_long();
    void control_thread_function();
    double get_cpu_utilization();
    int get_threads();
    void log_data(std::ofstream &);

public:
    SubmissionServer(const char *port, const char *pool_size);
    void accept_requests();
};

#endif