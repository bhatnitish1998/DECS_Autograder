#include "LoadTester.h"
using namespace std;

int main(int argc, char const *argv[])
{
    try
    {   // Process arguments
        if (argc != 3)
        {
            throw("Usage : ./LoadTester <server ip:port> <version>");
            exit(0);
        }
        // Set size of the global array to hold statistics
        global_data.resize(5,0);

        LoadTester lt = LoadTester(argv[1],atoi(argv[2]));
        lt.run_test();
    }
    catch (const char *msg)
    {
        std::cerr << msg << std::endl;
    }
    return 0;
}