#include "Server.hpp"

int main(int argc, char const *argv[]) {
    // Process arguments
    int port;
    try {
        if (argc != 2) {
            throw ("Usage : ./server <port>");
        }

        Server myserver(argv[1]);
        while (1)
            myserver.accept_requests();
    }
    catch (char const *msg) {
        std::cerr << msg << std::endl;
        perror(msg);
    }

    return 0;
}