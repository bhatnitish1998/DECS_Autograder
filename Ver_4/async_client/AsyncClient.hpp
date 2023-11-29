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
    int submission_sockfd, response_sockfd, iterations;
    uint32_t req_id;
    addrinfo *submission_servinfo, *response_servinfo;
    std::string program_filename;
    std::string submission_serverIp, submission_port;
    std::string response_serverIp, response_port;
    size_t timeout, n_timeout, n_succ, n_req;
    std::vector<double> response_time_ack, response_time_done;
    std::vector<std::string> test_files;

    std::string response_string;

    void parseSubmissionAddress(std::string remoteAddress);
    void parseResponseAddress(std::string remoteAddress);
    void setup_submission_socket();
    void setup_response_socket();
    void send_file();
    void send_req_id();
    void receive_response(int sockfd);
    std::string choose_file();
    std::vector<double> get_statistics();
    uint32_t getIDFromMessage();

public:
    AsyncClient(const char *submission_remote_address, const char *response_remote_address, int loop_num, int time_out);
    void submit(const char *filename);
    void checkStatus(const uint32_t request_id);
};

#endif