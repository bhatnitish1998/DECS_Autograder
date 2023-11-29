#include "Client.hpp"

Client::Client(const char *remote_address, int loop_num, int sleep_time, int timeout_sec,const char * program_filename) : n_req(0), n_succ(0), n_timeout(0), iterations(loop_num), sleepTime(sleep_time), timeout(timeout_sec),program_filename(program_filename)
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

    if(RANDOMIZE) {
        std::string path = "../Test_files/";
        for (const auto &file: std::filesystem::directory_iterator(path))
            test_files.push_back(file.path());
    }
}

void Client::send_file()
{
    uint32_t file_size = std::filesystem::file_size(program_filename);
    uint32_t length_to_send = htonl(file_size);

    if (write(sockfd, &length_to_send, sizeof(length_to_send)) < 0)
        throw("error sending file size");

    std::ifstream fin(program_filename, std::ios::binary);
    if (!fin)
        throw("error opening file");

    char buffer[1024];
    while (!fin.eof())
    {
        memset(buffer, 0, sizeof(buffer));
        fin.read(buffer, sizeof(buffer));
        int k = fin.gcount();
        int n = write(sockfd, buffer, k);
        if (n < 0)
            throw("error writing file");
    }
}

void Client::receive_response()
{
    std::string response = "";
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
        response += std::string(buffer);
    }
    std::cout<<"Response received from server:\n"+response<<std::endl;
}

void Client::parseAddress(std::string remoteAddress)
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

void Client::setup_socket()
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

std::string Client::choose_file()
{
    int no_of_files = test_files.size();
    int r = rand() % test_files.size();
    return test_files.at(r);
}

void Client::display_statistics()
{
    double total_rt = 0;
    for (auto x : response_times)
        total_rt += x;
    std::cout << "Number of requests: " << n_req << std::endl;
    std::cout << "Successful responses: " << n_succ << std::endl;
    std::cout << "Timeouts: " << n_timeout << std::endl;
    std::cout << "Total response time: " << total_rt << std::endl;
}

// return vector [requests, successes, timeouts, error, total response time,total response time 2]
std::vector<double> Client::get_statistics() {
    std::vector<double> data;

    double total_rt = 0;
    for (auto x : response_times)
        total_rt += x;

    data.push_back(n_req);
    data.push_back(n_succ);
    data.push_back(n_timeout);
    data.push_back(n_req-(n_succ+n_timeout));
    data.push_back(total_rt);

    return data;
}

void Client::submit()
{
    for (int i = 0; i < iterations; i++)
    {
        try
        {
            setup_socket();
            if (RANDOMIZE)
            program_filename = choose_file();
            else
                program_filename = "../Test_files/test4.cpp";
            send_file();
            n_req++;
            auto start_time = std::chrono::high_resolution_clock::now();
            receive_response();
            n_succ++;
            auto end_time = std::chrono::high_resolution_clock::now();
            double total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            response_times.push_back(total_time);
        }
        catch (const char *msg)
        {
            std::cerr << msg << std::endl;
        }
        close(sockfd);

        sleep(sleepTime);
    }
}