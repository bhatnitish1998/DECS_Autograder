#ifndef __Client_hpp__
#define __Client_hpp__

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

#define RANDOMIZE 0

class Client
{
    int sockfd, iterations;
    addrinfo *servinfo;
    std::string program_filename;
    std::string serverIp, port;
    size_t sleepTime, timeout, n_timeout, n_succ, n_req;
    std::vector<double> response_times;
    std::vector<std::string> test_files;

    void parseAddress(std::string remoteAddress);
    void setup_socket();
    void send_file();
    void receive_response();
    std::string choose_file();
    void display_statistics();


public:
    Client(const char *remote_address, int loop_num, int sleep_time, int timeout_sec,const char * program_filename) ;
    void submit();
    std::vector<double>  get_statistics();

};

#endif