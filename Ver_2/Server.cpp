#include "Server.hpp"

Server::Server(const char *port) : port(std::stoi(port))
{
    backlog = 5;
    service_time = 0;
    std::thread(&Server::control_thread_function, this).detach();
    setup_socket();
}

void Server::setup_socket()
{
    int status;
    addrinfo hints, *servinfo, *p;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    sin_size = sizeof client_addr;

    if ((status = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &servinfo)) != 0)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        exit(1);
    }
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
        throw("server: socket");
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1)
    {
        throw("setsockopt failed");
    }
    // Bind socket
    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        close(sockfd);
        throw("server: bind");
    }
    if (listen(sockfd, backlog) == -1)
        throw("listen");

    for (p = servinfo; p != nullptr; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        { // IPv4
            struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in *>(p->ai_addr);
            char ip4[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip4, INET_ADDRSTRLEN);
            std::cerr << "Server ready and listening at " << ip4 << ":" << port << std::endl;
            break;
        }
    }

    freeaddrinfo(servinfo);
}

void Server::accept_requests()
{
    sockaddr_in client_addr;
    socklen_t client_length = sizeof(client_addr);

    int newsockfd = accept(sockfd, (sockaddr *)&client_addr, &client_length);
    if (newsockfd < 0)
        throw("Error accepting connection");

    std::thread(&Server::thread_function, this, newsockfd).detach();
}

void Server::thread_function(int newsockfd)
{
    Worker worker(newsockfd);
    double temp_st = worker.process_request();
    {
        std::unique_lock<std::mutex> lock(service_mutex);
        service_time = temp_st;
    }
}

void Server::setup_control()
{

    sockaddr_in control_addr;
    control_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (control_sockfd < 0)
        throw("Error opening socket");

    memset(&control_addr, 0, sizeof(control_addr));
    control_addr.sin_family = AF_INET;
    control_addr.sin_port = htons(CONTROL_PORT);
    control_addr.sin_addr.s_addr = INADDR_ANY;

    int sc = bind(control_sockfd, (sockaddr *)&control_addr, sizeof(control_addr));
    if (sc < 0)
        throw("Error while binding");

    listen(control_sockfd, 1);
    std::cerr << "Control channel at " << control_addr.sin_addr.s_addr << ":" << CONTROL_PORT << std::endl;

    sockaddr_in loadtester_addr;
    socklen_t loadtester_length = sizeof(loadtester_addr);

    new_control_sockfd = accept(control_sockfd, (sockaddr *)&loadtester_addr, &loadtester_length);
    std::cerr << "Connected\n";
    if (new_control_sockfd < 0)
        throw("Error accepting connection");

    // set nonblocking
    int flags = fcntl(new_control_sockfd, F_GETFL, 0);
    if (flags < 0)
        throw("Error getting flags");

    if (fcntl(new_control_sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
        throw("Error setting flags");
}

uint32_t Server::receive_long()
{
    uint32_t value = 0;
    int n = read(new_control_sockfd, &value, sizeof(value));
    if (n > 0)
    {
        value = ntohl(value);
        return value;
    }
    return value;
}

void Server::control_thread_function()
{
    setup_control();
    std::ofstream fout("../Graphs_and_Logs/ver2_server.txt");
    fout << "cpu,threads,queue,service_time" << std::endl;
    uint32_t number_of_clients = 0;

    while (true)
    {
        number_of_clients = receive_long();

        // if received new value
        if (number_of_clients)
        {
            fout << number_of_clients << std::endl;
            if (number_of_clients > 5000)
            {
                fout.close();
                break;
            }
        }

        // if not received value then write same value.
        log_data(fout);
        sleep(0.5);
    }
}

double Server::get_cpu_utilization()
{
    FILE *top_output = popen("top -bn 1 | grep '%Cpu' | awk '{print $2}'", "r");
    if (!top_output)
    {
        return -1.0;
    }
    char buffer[128];
    double cpu_utilization = -1.0;

    if (fgets(buffer, sizeof(buffer), top_output) != nullptr)
    {
        cpu_utilization = std::stod(buffer);
    }
    pclose(top_output);
    return cpu_utilization;
}

int Server::get_threads()
{
    FILE *ps_output = popen("ps -T -p $(pgrep 'server') --no-headers | wc -l", "r");
    if (!ps_output)
    {
        return 0;
    }
    char buffer[128];
    int threads = 0;

    if (fgets(buffer, sizeof(buffer), ps_output) != nullptr)
    {
        threads = std::stoi(buffer);
    }
    pclose(ps_output);
    return threads;
}

void Server::log_data(std::ofstream &fout)
{
    // cpu utilization
    double cpu = get_cpu_utilization();
    fout << cpu << ",";

    // number of threads
    int threads = get_threads();
    fout << threads << ",";

    // current queue size
    int current_queue_size = 0;
    fout << current_queue_size << ",";

    double temp_st = 0;
    {
        std::unique_lock<std::mutex> lock(service_mutex);
        temp_st = service_time;
    }
    fout << temp_st << ",";

    fout << std::endl;
}
