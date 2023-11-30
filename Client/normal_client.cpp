#include "normal_client.hpp"

Client::Client(const char *remote_address, const char *program_filename) : program_filename(program_filename)
{

    int status;
    addrinfo hints, *p;
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    parseAddress(remote_address);
    if ((status = getaddrinfo(serverIp.c_str(), port.c_str(), &hints, &servinfo)) != 0)
    {
        throw std::runtime_error("getaddrinfo error");
    }
}

void Client::send_file()
{
    uint32_t file_size = std::filesystem::file_size(program_filename);
    std::cerr << file_size;
    uint32_t length_to_send = htonl(file_size);

    if (write(sockfd, &length_to_send, sizeof(length_to_send)) <= 0)
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
        int n = write(sockfd, buffer, k);
        if (n <= 0)
            throw std::runtime_error("error writing file");
    }
}

void Client::receive_response()
{
    std::string response = "";
    int status;
    uint32_t message_size;
    if ((status = recv(sockfd, &message_size, sizeof(message_size), 0)) < 0)
    {
        throw std::runtime_error("file size read error");
    }

    message_size = ntohl(message_size);

    char buffer[1024];
    uint32_t read_bytes = 0;
    uint32_t current_read = 0;

    int retry = 0;
    while (read_bytes < message_size)
    {
        current_read = 0;
        memset(buffer, 0, sizeof(buffer));
        status = recv(sockfd, buffer, sizeof(buffer), 0);

        if (status > 0)
        {
            current_read = status;
            read_bytes += current_read;
            response += std::string(buffer);
        }
    }
    std::cout << "Response received from server:\n" + response << std::endl;
}

void Client::parseAddress(std::string remoteAddress)
{
    size_t colonPos = remoteAddress.find(':');

    if (colonPos == std::string::npos)
    {
        throw std::runtime_error("Invalid server argument format. Use <server-ip:port>");
    }

    // Extract the IP address and port
    serverIp = remoteAddress.substr(0, colonPos);
    port = remoteAddress.substr(colonPos + 1);
}

void Client::setup_socket()
{
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
        throw std::runtime_error("Error opening socket");

    int sc = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (sc < 0)
        throw std::runtime_error("Cannot connect");
}

void Client::submit()
{
    try
    {
        send_file();
        receive_response();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    close(sockfd);
}