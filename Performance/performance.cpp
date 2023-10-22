#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>


// Currently lot of things are done by bash scripts
class PerfAnalyzer
{
    std::string num_of_clients;
    std::string iterations;
    std::string sleep_time;
    std::string timeout_time;
    double total_time;

public:
    void cleanup()
    {
        std::string cmd2 = "bash cleanup.sh";
        system(cmd2.c_str());
    }
    void run_load_test()
    {
        std::string cmd1 = "bash loadtest.sh " + num_of_clients + " " + iterations + " " + sleep_time + " " + timeout_time;
        auto start_time = std::chrono::high_resolution_clock::now();
        system(cmd1.c_str());
        auto end_time = std::chrono::high_resolution_clock::now();
        total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    }
    void display_metrics()
    {
        // Uses file as of now to read from generated bash script

        std::ifstream fin("global_data.txt");
        double success, average_response, requests, timeouts;

        fin >> average_response >> requests >> success >> timeouts;
        fin.close();

        std::cout << "Number of clients = " << num_of_clients << std::endl;
        std::cout << "Throughput = " << (success / total_time) * 1000 << std::endl;
        std::cout << "Average response time = " << average_response << std::endl;
        std::cout << "Request rate = " << ((requests / total_time)) * 1000 << std::endl;
        std::cout << "Success percent = " << (success / requests) * 100 << std::endl;
        std::cout << "Timeout percent = " << (timeouts/requests)*100 <<std::endl;
        std::cout << "Error percent = " << ((requests - success) / requests) * 100 << std::endl;

    }
    PerfAnalyzer(const char *num_of_clients, const char *iterations, const char *sleep_time, const char *timeout_time)
        : num_of_clients(num_of_clients), iterations(iterations), sleep_time(sleep_time), timeout_time(timeout_time), total_time(0)
    {
        run_load_test();
        cleanup();
        display_metrics();
    }
};

int main(int argc, char const *argv[])
{
    try
    {
        if (argc != 5)
        {
            throw("Usage : ./performance <clients> <iterations> <sleep> <timeout> ");
            exit(0);
        }
    }
    catch (const char *msg)
    {
        std::cerr << msg << std::endl;
    }

    PerfAnalyzer p(argv[1], argv[2], argv[3], argv[4]);

    return 0;
}
