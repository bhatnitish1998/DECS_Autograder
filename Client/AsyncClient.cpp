#include "AsyncClient.hpp"

AsyncClient::AsyncClient(const char *submission_remote_address, const char *response_remote_address, int loop_num, int timeout)
    : n_req(0), n_succ(0), n_timeout(0), timeout(timeout), iterations(loop_num)
{
    int status;
    addrinfo hints, *p;
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    parseSubmissionAddress(submission_remote_address);
    parseResponseAddress(response_remote_address);
    if ((status = getaddrinfo(submission_serverIp.c_str(), submission_port.c_str(), &hints, &submission_servinfo)) != 0)
    {
        throw std::runtime_error("getaddrinfo error");
    }
    if ((status = getaddrinfo(response_serverIp.c_str(), response_port.c_str(), &hints, &response_servinfo)) != 0)
    {
        throw std::runtime_error("getaddrinfo error");
    }
}

void AsyncClient::send_file()
{
    std::cout << "Sending file " << program_filename << "\n";
    uint32_t file_size = std::filesystem::file_size(program_filename);
    uint32_t length_to_send = htonl(file_size);

    if (write(submission_sockfd, &length_to_send, sizeof(length_to_send)) < 0)
        throw std::runtime_error("error sending file size");

    std::ifstream fin(program_filename, std::ios::binary);
    if (!fin)
        throw std::runtime_error("error opening file");

    char buffer[1024];
    while (!fin.eof())
    {
        memset(buffer, 0, sizeof(buffer));
        fin.read(buffer, sizeof(buffer));
        int k = fin.gcount();
        int n = write(submission_sockfd, buffer, k);
        if (n < 0)
            throw std::runtime_error("error writing file");
    }
}

void AsyncClient::receive_response(int sockfd)
{
    std::string response = "";
    int status;
    uint32_t message_size;
    if ((status = recv(sockfd, &message_size, sizeof(message_size),0)) <0 )
    {
//        if (errno == EAGAIN || errno == EWOULDBLOCK) {
//            n_timeout++;
//            throw std::runtime_error("timeout 1");
//        }
        n_timeout++;
        throw std::runtime_error("timeout ");
    }

    message_size = ntohl(message_size);

    char buffer[1024];
    uint32_t read_bytes = 0;
    uint32_t current_read = 0;

    int retry =0;
    while (read_bytes < message_size)
    {
        current_read = 0;
        memset(buffer, 0, sizeof(buffer));
        status = recv(sockfd, buffer, sizeof(buffer),0);
        if (status <0 )
        {
//            if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                n_timeout++;
//                throw std::runtime_error("timeout 1");
//            }
            n_timeout++;
            throw std::runtime_error("timeout ");
        }
        if(status > 0) {
            current_read = status;
            read_bytes += current_read;
            response += std::string(buffer);
        }
    }
//    std::cout << "Response received from server:\n" + response << std::endl;
}

void AsyncClient::parseSubmissionAddress(std::string remoteAddress)
{
    size_t colonPos = remoteAddress.find(':');

    if (colonPos == std::string::npos)
    {
        throw std::runtime_error("Invalid server argument format. Use <server-ip:port>");
    }

    // Extract the IP address and port
    submission_serverIp = remoteAddress.substr(0, colonPos);
    submission_port = remoteAddress.substr(colonPos + 1);
}

void AsyncClient::parseResponseAddress(std::string remoteAddress)
{
    size_t colonPos = remoteAddress.find(':');

    if (colonPos == std::string::npos)
    {
        throw std::runtime_error("Invalid server argument format. Use <server-ip:port>");
    }

    // Extract the IP address and port
    response_serverIp = remoteAddress.substr(0, colonPos);
    response_port = remoteAddress.substr(colonPos + 1);
}

void AsyncClient::setup_submission_socket()
{
    if ((submission_sockfd = socket(submission_servinfo->ai_family, submission_servinfo->ai_socktype, submission_servinfo->ai_protocol)) == -1)
        throw std::runtime_error("Error opening socket");

//    timeval time_out;
//    time_out.tv_sec = timeout;
//    time_out.tv_usec = 0;
//    if (setsockopt(submission_sockfd, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof(time_out)) == -1)
//    {
//        close(submission_sockfd);
//        throw std::runtime_error("Set socket timeout failed");
//    }

    int sc = connect(submission_sockfd, submission_servinfo->ai_addr, submission_servinfo->ai_addrlen);
    if (sc < 0)
        throw std::runtime_error("Cannot connect");
}

void AsyncClient::setup_response_socket()
{
    if ((response_sockfd = socket(response_servinfo->ai_family, response_servinfo->ai_socktype, response_servinfo->ai_protocol)) == -1)
        throw std::runtime_error("Error opening socket");

    int sc = connect(response_sockfd, response_servinfo->ai_addr, response_servinfo->ai_addrlen);
    if (sc < 0)
        throw std::runtime_error("Cannot connect");
}

std::string AsyncClient::choose_file()
{
    int no_of_files = test_files.size();
    int r = rand() % test_files.size();
    return test_files.at(r);
}

std::vector<double> AsyncClient::get_statistics()
{
    std::vector<double> data;

    double total_rt_ack = 0;
    double total_rt_done = 0;
    for (auto x : response_time_ack)
        total_rt_ack += x;
    for (int i = 0; i < response_time_ack.size(); i++)
    {
        response_time_done[i] += response_time_ack[i]; // final response time is cumulative
    }
    for (auto x : response_time_done)
        total_rt_done += x;

    data.push_back(n_req);
    data.push_back(n_succ);
    data.push_back(n_timeout);
    data.push_back(n_req - (n_succ + n_timeout));
    data.push_back(total_rt_ack);
    data.push_back(total_rt_done);

    return data;
}

uint32_t AsyncClient::getIDFromMessage()
{
    size_t firstPos = response_string.find('<');
    size_t lastPos = response_string.find('>');
    auto id_string = response_string.substr(firstPos + 1, lastPos - firstPos - 1);
    uint32_t request_id = static_cast<uint32_t>(std::stol(id_string));
    return request_id;
}

void AsyncClient::submit(const char *filename)
{
    for (int i = 0; i < iterations; i++)
    {
        try
        {
            setup_submission_socket();
            program_filename = filename;
            send_file();
            n_req++;
            auto start_time = std::chrono::high_resolution_clock::now();
            receive_response(submission_sockfd);
            n_succ++;
            auto end_time = std::chrono::high_resolution_clock::now();
            double total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            response_time_ack.push_back(total_time);
            uint32_t req_id = getIDFromMessage();
            checkStatus(req_id);
        }
        catch (const char *msg)
        {
            std::cerr << msg << std::endl;
        }
        close(submission_sockfd);
    }
}

void AsyncClient::send_req_id()
{
    auto id = htonl(req_id);
    if (write(response_sockfd, &id, sizeof(id)) < 0)
        throw std::runtime_error("Request id send error");
}

void AsyncClient::checkStatus(const uint32_t request_id)
{
    // The regular expression pattern to search for
    std::regex pattern("done");
    req_id = request_id;
    // poll the server until grading is done
    while (1)
    {
        setup_response_socket();
        send_req_id();
        auto start_time = std::chrono::high_resolution_clock::now();
        receive_response(response_sockfd);
        // Check if the response contatins done somewhere
        if (std::regex_search(response_string, pattern))
        {
            auto end_time = std::chrono::high_resolution_clock::now();
            double total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            response_time_done.push_back(total_time);
            close(response_sockfd);
            break;
        }
        else
        {
            sleep(1);
        }
    }
}