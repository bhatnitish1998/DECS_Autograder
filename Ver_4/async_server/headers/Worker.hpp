#ifndef __Worker_hpp__
#define __Worker_hpp__

#include <iostream>
#include <string>
#include <mutex>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <stdlib.h>
#include <filesystem>
#include <vector>
#include <thread>
#include "Database.hpp"
#include <regex>
#include <chrono>
#include "FastQueue.h"
extern FastQueue grader_queue;
class SubmissionWorker
{
    Database db;
    int newsockfd;
    uint32_t req_id;
    std::string file_identifier;
    std::string program_file;
    std::string msg;
    void receive_file();
    std::vector<std::string> cleanuplist;

    void cleanup();
    void send_response(int sockfd, std::string response);

public:
    SubmissionWorker(int sockfd);
    ~SubmissionWorker();
    uint32_t work();
};
class GradingWorker
{
    Database db;
    uint32_t req_id;
    std::string executable_file;
    std::string output_file;
    std::string file_identifier;
    std::string program_file;
    std::string msg;
    void compile();
    void run_program();
    void compare_output();
    int fetchDB();
    Request req;
    bool done;
    std::string base_dir; // base directory for intermediate files
    std::vector<std::string> cleanuplist;

    void cleanup();

public:
    GradingWorker(uint32_t request_id);
    ~GradingWorker();
    double work();
};

class ResponseWorker
{
    Database db;
    uint32_t req_id;
    int sock_fd;
    std::string msg;
    Request req;
    uint32_t findQueuePos();
    uint32_t getWaitTime();
    void recv_req_id();

    void send_response(int sockfd, std::string response);

public:
    ResponseWorker(int sockfd);
    ~ResponseWorker();
    void work();
};

#endif
