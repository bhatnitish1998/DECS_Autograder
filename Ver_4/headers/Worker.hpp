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

extern uint32_t request_id;
extern std::mutex id_lock;

class Worker
{
protected:
    std::string file_identifier;
    std::string program_file;
    std::string msg;
    bool done;
    std::vector<std::string> cleanuplist;

    void cleanup();

    void send_response(int sockfd, std::string response);

public:
    Worker();
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
    uint32_t work() override;
};
class GradingWorker : public Worker
{
    uint32_t req_id;
    void compile();
    void run_program();
    void compare_output();
    void fetchDB();
    void updateDB();

public:
    GradingWorker(uint32_t request_id);
    uint32_t work() override;
};

class ResponseWorker : public Worker
{
    uint32_t req_id;
    void fetchDB();

public:
    ResponseWorker(int sockfd, uint32_t request_id);
    uint32_t work() override;
};

#endif
