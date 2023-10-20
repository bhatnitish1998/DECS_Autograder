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

using namespace std;
// TODO: ADD Namespace std
class Client
{
    int sockfd, iterations;
    addrinfo *servinfo;
    string program_filename;
    string serverIp, port;
    size_t n_responses, sleepTime, timeout, n_timeout, n_succ, n_req;
    vector<double> response_times;

    void parseAddress(std::string remoteAddress)
    {
        size_t colonPos = remoteAddress.find(':');

        if (colonPos == std::string::npos)
        {
            throw("Invalid server argument format. Use <server-ip:port>");
        }

        // Extract the IP address and port
        serverIp = remoteAddress.substr(0, colonPos);
        port = remoteAddress.substr(colonPos + 1);
    }

    void setup_socket()
    {
        if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
            throw("Error opening socket");

        timeval time_out;
        time_out.tv_sec = timeout;
        time_out.tv_usec = 0;
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof(time_out)) == -1)
        {
            close(sockfd);
            throw("Set socket timeout failed");
        }

        int sc = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
        if (sc < 0)
            throw("Cannot connect");
    }

    void send_file()
    {
        uint32_t file_size = filesystem::file_size(program_filename);
        uint32_t length_to_send = htonl(file_size);

        if (write(sockfd, &length_to_send, sizeof(length_to_send)) < 0)
            throw("error sending file size");

        ifstream fin(program_filename, ios::binary);
        if (!fin)
            throw("error opening file");

        char buffer[1024];
        while (!fin.eof())
        {
            memset(buffer, 0, sizeof(buffer));
            fin.read(buffer, sizeof(buffer));
            int k = fin.gcount();
            // todo: handle if kernel fails to send the data
            int n = write(sockfd, buffer, k);
            if (n < 0)
                throw("error writing file");
        }
    }

    void receive_response()
    {
        string response = "";
        int status;
        uint32_t message_size;
        if ((status = read(sockfd, &message_size, sizeof(message_size))) < 0)
        {
            if (errno == EWOULDBLOCK)
                n_timeout++;
            throw("file size read error");
        }

        message_size = ntohl(message_size);

        char buffer[1024];
        uint32_t read_bytes = 0;
        uint32_t current_read = 0;
        while (read_bytes < message_size)
        {
            current_read = 0;
            memset(buffer, 0, sizeof(buffer));
            current_read = read(sockfd, buffer, sizeof(buffer));
            if (current_read < 0)
            {
                if (errno == EWOULDBLOCK)
                    n_timeout++;
                throw("socket read error");
            }
            read_bytes += current_read;

            response += string(buffer);
        }

        // cout << "Received response: ";
        // cout << response << endl;
    }

public:
    Client(const char *remote_address, const char *file_name, const char *loop_num, const char *sleep_time, const char *timeout_sec) : program_filename(program_filename),
                                                                                                                                       n_req(0), n_responses(0), n_succ(0), n_timeout(0), iterations(std::atoi(loop_num)), sleepTime(std::atoi(sleep_time)), timeout(std::atoi(timeout_sec))
    {

        int status;
        addrinfo hints, *p;
        std::memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        parseAddress(remote_address);
        if ((status = getaddrinfo(serverIp.c_str(), port.c_str(), &hints, &servinfo)) != 0)
        {
            throw("getaddrinfo error");
        }
    };

    void display_statistics()
    {
        double total_rt = 0;
        for (auto x : response_times)
            total_rt += x;
        std::cout << "Number of requests: " << n_responses << std::endl;
        std::cout << "Successful responses: " << n_succ << std::endl;
        std::cout << "Timeouts: " << n_timeout << std::endl;
        std::cout << "Total response time: " << total_rt << std::endl;
    }

    void submit()
    {
        for (int i = 0; i < iterations; i++)
        {
            try
            {
                setup_socket();
                send_file();
                n_req++;
                auto start_time = chrono::high_resolution_clock::now();
                receive_response();
                n_succ++;
                auto end_time = chrono::high_resolution_clock::now();
                double total_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
                response_times.push_back(total_time);
            }
            catch (const char *msg)
            {
                cerr << msg << endl;
            }
            close(sockfd);

            sleep(sleepTime);
        }

        display_statistics();
    }
};

int main(int argc, char const *argv[])
{
    // Process arguments
    if (argc != 6)
    {
        throw("Usage : ./client <serverip:port> <filetosubmit> <loopNum> <sleepTime> <timeout>");
        exit(0);
    }

    try
    {
        Client client(argv[1], argv[2], argv[3], argv[4], argv[5]);
        client.submit();
    }
    catch (const char *msg)
    {
        cerr << msg << endl;
    }
    return 0;
}