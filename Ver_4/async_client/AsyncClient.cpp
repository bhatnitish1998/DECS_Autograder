#include "AsyncClient.hpp"

AsyncClient::AsyncClient(const char *remote_address) : n_req(0), n_succ(0), n_timeout(0)
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

    std::string path = "input/";
    for (const auto &file : std::filesystem::directory_iterator(path))
        test_files.push_back(file.path());
}

void AsyncClient::send_file()
{
    std::cout << "Sending file " << program_filename << "\n";
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
        // todo: handle if kernel fails to send the data
        int n = write(sockfd, buffer, k);
        if (n < 0)
            throw("error writing file");
    }
}

void AsyncClient::receive_response()
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
    response_string = response;
    std::cout << response << "\n";
}

void AsyncClient::parseAddress(std::string remoteAddress)
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

void AsyncClient::setup_socket()
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

std::string AsyncClient::choose_file()
{
    int no_of_files = test_files.size();
    int r = rand() % test_files.size();
    return test_files.at(r);
}

void AsyncClient::display_statistics()
{
    double total_rt = 0;
    for (auto x : response_times)
        total_rt += x;
    std::cout << "Number of requests: " << n_req << std::endl;
    std::cout << "Successful responses: " << n_succ << std::endl;
    std::cout << "Timeouts: " << n_timeout << std::endl;
    std::cout << "Total response time: " << total_rt << std::endl;
}

void AsyncClient::submit(const char *filename)
{
    try
    {
        setup_socket();
        program_filename = filename;
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

    display_statistics();
}

void AsyncClient::send_req_id()
{
    auto id = htonl(req_id);
    if (write(sockfd, &id, sizeof(id)) < 0)
        throw("Request id send error");
}

void AsyncClient::checkStatus(const uint32_t request_id)
{
    // The regular expression pattern to search for
    std::regex pattern("done");
    req_id = request_id;
    // poll the server until grading is done
    while (1)
    {
        setup_socket();
        send_req_id();
        auto start_time = std::chrono::high_resolution_clock::now();
        receive_response();
        auto end_time = std::chrono::high_resolution_clock::now();
        double total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        response_times.push_back(total_time);
        // Check if the response contatins done somewhere
        if (std::regex_search(response_string, pattern))
        {
            break;
        }
        else
        {
            sleep(1);
        }
    }
}