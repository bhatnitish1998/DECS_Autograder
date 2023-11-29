#include "Server.hpp"

int main(int argc, char const *argv[]) {
    // Process arguments
    int port;
    try {
        if (argc != 3) {
            throw std::runtime_error("Usage : ./server <port> <thread_pool_size>");
        }
        Server myserver(argv[1], argv[2]);
        while (1)
            myserver.accept_requests();
    }
    catch (const std::exception &e) {
        std::cerr << e.what()<<std::endl;
    }
    return 0;
}