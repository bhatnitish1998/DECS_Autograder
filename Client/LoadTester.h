#ifndef DECS_AUTOGRADER_LOADTESTER_H
#define DECS_AUTOGRADER_LOADTESTER_H

#include "Client.hpp"
#include <mutex>
#include <thread>
#include <fstream>
#include <chrono>

// variables for number of clients interval generation for graphs
#define INITIAL 5
#define INTERVAL 5
#define TIMES 2

// Loadtesting client parameters
#define LOOPNUM 20
#define SLEEP 1
#define TIMEOUT 1

using namespace std;

// mutex and global vector for storing data returned by each client.
extern mutex global_mutex;
extern vector<double> global_data;

class LoadTester {
    string server_info;
    void run_client ();

public:
    LoadTester(const char * server_info);
    void run_test();
};


#endif //DECS_AUTOGRADER_LOADTESTER_H
