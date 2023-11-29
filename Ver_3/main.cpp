#include "Server.hpp"

int main(int argc, char const *argv[]) {
    // Process arguments
    int port;
    try {
        if (argc != 3) {
            throw ("Usage : ./server <port> <thread_pool_size>");
        }

        Server myserver(argv[1], argv[2]);
        while (1)
            myserver.accept_requests();
    }
    catch (char const *msg) {
        std::cerr << msg << std::endl;
        perror(msg);
    }

    return 0;
}