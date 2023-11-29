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

extern uint32_t filename_ticket;
extern std::mutex ticket_lock;


class Worker
{
    int newsockfd;
    std::string file_identifier;
    std::string program_file;
    std::string executable_file;
    std::string output_file;
    std::string msg;
    std::string file_to_send;
    bool done;
    std::vector<std::string> cleanuplist;

    void cleanup();
    void receive_file();
    void send_response();
    void compile();
    void run_program();
    void compare_output();

public:
    Worker(int newsockfd);
    double process_request();
    ~Worker();
};

#endif
