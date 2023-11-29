#include "Server.hpp"

int main(int argc, char const *argv[]) {
    // Process arguments
    int port;
    try {
        if (argc != 2) {
            throw std::runtime_error("Usage : ./server <port>");
        }

        Server myserver(argv[1]);
        while (1)
            myserver.accept_requests();
    }
    catch (const std::exception &e) {
        std::cerr << e.what()<<std::endl;
    }
    return 0;
}