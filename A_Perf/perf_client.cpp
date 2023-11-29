#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <thread>

class PerfClient
{
private:
    int sockfd;
    addrinfo *servinfo;
    std::string serverIp, port;
    void setup_socket();
    void parseAddress(std::string remoteAddress);
    void send();
    void receive();

public:
    void connect();
    void signalStart();
    void signalEnd();
    PerfClient(const char *remote_address);
    ~PerfClient();
};

PerfClient::PerfClient(const char *remote_address)
{
}

PerfClient::~PerfClient()
{
}
