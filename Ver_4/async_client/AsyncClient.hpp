#ifndef __AS_Client_hpp__
#define __AS_Client_hpp__

#include <iostream>
#include <string>
#include <fstream>
#include <netdb.h>
#include <cstring>
#include <arpa/inet.h>
#include <fstream>
#include <unistd.h>
#include <chrono>
#include <vector>
#include <filesystem>
#include <random>
#include <regex>
class AsyncClient
{
    int sockfd, iterations;
    uint32_t req_id;
    addrinfo *servinfo;
    std::string program_filename;
    std::string serverIp, port;
    size_t sleepTime, timeout, n_timeout, n_succ, n_req;
    std::vector<double> response_times;
    std::vector<std::string> test_files;

    std::string response_string;

    void parseAddress(std::string remoteAddress);
    void setup_socket();
    void send_file();
    void send_req_id();
    void receive_response();
    std::string choose_file();
    void display_statistics();

public:
    AsyncClient(const char *remote_address);
    void submit(const char *filename);
    void checkStatus(const uint32_t request_id);
};

#endif