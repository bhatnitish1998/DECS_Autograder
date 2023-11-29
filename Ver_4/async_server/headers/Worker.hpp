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
extern uint32_t request_id;
extern std::mutex id_lock;

class Worker
{
protected:
    Database db;
    std::string file_identifier;
    std::string program_file;
    std::string msg;

    std::vector<std::string> cleanuplist;

    void cleanup();

    void send_response(int sockfd, std::string response);

public:
    Worker();
    ~Worker();
    virtual uint32_t work() = 0;
};

class SubmissionWorker : public Worker
{
    int newsockfd;
    uint32_t req_id;
    void receive_file();
    void gen_id();

public:
    SubmissionWorker(int sockfd);
    ~SubmissionWorker();
    uint32_t work() override;
};
class GradingWorker : public Worker
{
    uint32_t req_id;
    std::string executable_file;
    std::string output_file;
    void compile();
    void run_program();
    void compare_output();
    int fetchDB();
    Request req;
    bool done;
    std::string base_dir; // base directory for intermediate files

public:
    GradingWorker(uint32_t request_id);
    ~GradingWorker();
    uint32_t work() override;
};

class ResponseWorker : public Worker
{
    uint32_t req_id;
    int sock_fd;
    void fetchDB();
    uint32_t findQueuePos();
    uint32_t getWaitTime();
    void recv_req_id();
    Request req;

public:
    ResponseWorker(int sockfd);
    ~ResponseWorker();
    uint32_t work() override;
};

#endif
