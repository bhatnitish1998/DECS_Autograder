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
    int sockfd;
    uint32_t req_id;
    addrinfo *servinfo;
    std::string program_filename;
    std::string serverIp, serverPort;
    std::string remote_address;
    std::string response_string;

    void parseAddress(std::string remoteAddress);
    void setup_socket();
    void send_file();
    void send_req_id();
    void receive_response(int sockfd);

public:
    AsyncClient(const char *remote_address);
    void submit(const char *filename);
    void checkStatus(const uint32_t request_id);
};

#endif