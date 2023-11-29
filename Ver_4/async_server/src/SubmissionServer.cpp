#include "SubmissionServer.hpp"
void SubmissionServer::setup_control()
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

uint32_t SubmissionServer::receive_long()
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
void SubmissionServer::control_thread_function()
{
    setup_control();
    std::ofstream fout("../Graphs_and_Logs/ver3_server.txt");
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
double SubmissionServer::get_cpu_utilization()
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
int SubmissionServer::get_threads()
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
void SubmissionServer::log_data(std::ofstream &fout)
{
    // cpu utilization
    double cpu = get_cpu_utilization();
    fout << cpu << ",";

    // number of threads
    int threads = get_threads();
    fout << threads << ",";

    // current queue size
    int current_queue_size = 0;
    {
        std::unique_lock<std::mutex> lock(grader_queue_mutex);
        current_queue_size = grader_queue.size();
    }
    fout << current_queue_size << ",";

    double temp_st = 0;
    {
        std::unique_lock<std::mutex> lock(service_mutex);
        temp_st = service_time;
    }
    fout << temp_st << ",";

    fout << std::endl;
}
SubmissionServer::SubmissionServer(const char *port, const char *pool_size)
    : port(std::stoi(port)),
      pool_size(std::stoi(pool_size)),
      backlog(5),
      service_time(0)
{
    // Recover queue in the event of server crash
    recoverGradingQueue();
    // Setup both threadpools
    setup_threadpools();
    // Setup Control thread
    std::thread(&SubmissionServer::control_thread_function, this).detach();
    // Setup connection socket
    setup_socket();
}

void SubmissionServer::accept_requests()
{
    sockaddr_in client_addr;
    socklen_t client_length = sizeof(client_addr);

    int newsockfd = accept(sockfd, (sockaddr *)&client_addr, &client_length);
    if (newsockfd < 0)
        throw("Error accepting connection");

    // block important for unlocking mutex when out of scope.
    {
        std::unique_lock<std::mutex> lock(submission_queue_mutex);
        submission_queue.push(newsockfd);
    }
    submission_queue_cond.notify_one();
}

void SubmissionServer::setup_socket()
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
            sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in *>(p->ai_addr);
            char ip4[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip4, INET_ADDRSTRLEN);
            std::cout << "Server ready and listening at " << ip4 << ":" << port << std::endl;
            break;
        }
    }

    freeaddrinfo(servinfo);
}

void SubmissionServer::setup_threadpools()
{
    init_submissionPool();
    init_gradingPool();
}

void SubmissionServer::recoverGradingQueue()
{
    Database db;
    std::vector<uint32_t> tasks = db.getPendingTasks();
    for (auto id : tasks)
    {
        grader_queue.push(id);
    }
}

void SubmissionServer::init_submissionPool()
{
    for (int i = 0; i < pool_size; i++)
        submit_thread_pool.push_back(std::thread(&SubmissionServer::submissionPoolFunction, this));
}

void SubmissionServer::init_gradingPool()
{
    for (int i = 0; i < pool_size; i++)
        grader_thread_pool.push_back(std::thread(&SubmissionServer::gradingPoolFunction, this));
}

void SubmissionServer::submissionPoolFunction()
{
    Database db;
    while (true)
    {
        int work_sockfd;

        {
            std::unique_lock<std::mutex> lock(submission_queue_mutex);
            submission_queue_cond.wait(lock, [this]()
                                       { return !submission_queue.empty(); });
            work_sockfd = submission_queue.front();
            submission_queue.pop();
        }

        SubmissionWorker worker(work_sockfd);
        auto req_id = worker.work();
        // Add as new grading request and notify grading threads
        {
            std::unique_lock<std::mutex> lock(grader_queue_mutex);
            grader_queue.push(req_id);
        }
        grader_queue_cond.notify_one();
    }
}

void SubmissionServer::gradingPoolFunction()
{
    while (true)
    {
        int work_req_id;
        {
            std::unique_lock<std::mutex> lock(grader_queue_mutex);
            grader_queue_cond.wait(lock, [this]()
                                   { return !grader_queue.empty(); });
            work_req_id = grader_queue.front();
            grader_queue.pop();
        }

        GradingWorker worker(work_req_id);
        worker.work();
    }
}