#ifndef DECS_AUTOGRADER_LOADTESTER_H
#define DECS_AUTOGRADER_LOADTESTER_H

#include "Client.hpp"
#include "AsyncClient.hpp"
#include <mutex>
#include <thread>
#include <fstream>
#include <chrono>

// variables for number of clients interval generation for graphs
#define INITIAL 5
#define INTERVAL 5
#define TIMES 20

// Loadtesting client parameters
#define LOOPNUM 5
#define SLEEP 1
#define TIMEOUT 1

// control port
#define CONTROL_PORT 5001

// program file
#define PROGRAMFILE "../Test_files/test4.cpp"

using namespace std;

// mutex and global vector for storing data returned by each client.
extern mutex global_mutex;
extern vector<double> global_data;

class LoadTester {
    string server_info;
    string response_server;
    int version;
    int control_sockfd;

    void run_client ();
    void establish_control_connection();
    void send_long (uint32_t value);

public:
    LoadTester(const char * server_info, int version, const char * response_server);
    void run_test();
};


#endif //DECS_AUTOGRADER_LOADTESTER_H
