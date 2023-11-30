#include "AsyncClient.hpp"
/// @brief Async client constructor
/// @param submission_remote_address Address of submission server
/// @param response_remote_address  Address of response server
/// @param loop_num Number of requests to send
/// @param timeout Timeout in seconds
AsyncClient::AsyncClient(const char *_remote_address)
    : remote_address(_remote_address)
{
    int status;
    addrinfo hints, *p;
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    parseAddress(remote_address);
    if ((status = getaddrinfo(serverIp.c_str(), serverPort.c_str(), &hints, &servinfo)) != 0)
    {
        throw std::runtime_error("getaddrinfo error");
    }
}
/// @brief Sends program file to submission server
void AsyncClient::send_file()
{
    //    std::cout << "Sending file " << program_filename << "\n";
    uint32_t file_size = std::filesystem::file_size(program_filename);
    uint32_t length_to_send = htonl(file_size);

    if (write(sockfd, &length_to_send, sizeof(length_to_send)) < 0)
        throw std::runtime_error("error sending file size");

    std::ifstream fin(program_filename, std::ios::binary);
    if (!fin)
    {
        fin.close();
        throw std::runtime_error("error opening file");
    }

    char buffer[1024];
    while (!fin.eof())
    {
        memset(buffer, 0, sizeof(buffer));
        fin.read(buffer, sizeof(buffer));
        int k = fin.gcount();
        int n = write(sockfd, buffer, k);
        if (n < 0)
        {
            fin.close();
            throw std::runtime_error("error writing file");
        }
    }
    fin.close();
}
/// @brief receives response from given socket descriptor
/// @param sockfd Socket descriptor
void AsyncClient::receive_response(int sockfd)
{
    std::string response = "";
    int status;
    uint32_t message_size;
    if ((status = read(sockfd, &message_size, sizeof(message_size))) < 0)
    {
        throw std::runtime_error("Couldn't read file size");
    }

    message_size = ntohl(message_size);

    char buffer[1024];
    uint32_t read_bytes = 0;
    int current_read = 0;
    while (read_bytes < message_size)
    {
        current_read = 0;
        memset(buffer, 0, sizeof(buffer));
        current_read = read(sockfd, buffer, sizeof(buffer));
        if (current_read < 0)
        {
            throw std::runtime_error("Couldn't read response");
        }
        read_bytes += current_read;

        response += std::string(buffer);
    }
    response_string = response;
    std::cout << response << "\n";
}
/// @brief Parse submission server address
/// @param remoteAddress
void AsyncClient::parseAddress(std::string remoteAddress)
{
    size_t colonPos = remoteAddress.find(':');

    if (colonPos == std::string::npos)
    {
        throw std::runtime_error("Invalid server argument format. Use <server-ip:port>");
    }

    // Extract the IP address and port
    serverIp = remoteAddress.substr(0, colonPos);
    serverPort = remoteAddress.substr(colonPos + 1);
}
/// @brief Creates socket and connects to submission server
void AsyncClient::setup_socket()
{
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
        throw std::runtime_error("Error opening socket");

    int sc = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (sc < 0)
        throw std::runtime_error("Cannot connect");
}
/// @brief Simulates multiple requests to submission server and polls response server in 1 second interval
/// @param filename
void AsyncClient::submit(const char *filename)
{
    try
    {
        setup_socket();
        program_filename = filename;
        send_file();
        receive_response(sockfd);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    close(sockfd);
}
/// @brief Sends request id to response server
void AsyncClient::send_req_id()
{
    auto id = htonl(req_id);
    if (write(sockfd, &id, sizeof(id)) < 0)
        throw std::runtime_error("Request id send error");
}
/// @brief Function to query status of a request id
/// @param request_id Id to query for
void AsyncClient::checkStatus(const uint32_t request_id)
{
    // The regular expression pattern to search for
    std::regex pattern("done");
    req_id = request_id;
    setup_socket();
    send_req_id();
    receive_response(sockfd);
}