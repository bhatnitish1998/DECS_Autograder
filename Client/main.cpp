#include "LoadTester.h"
using namespace std;

int main(int argc, char const *argv[])
{
    try
    {   // Process arguments
        if (argc < 3)
        {
            throw("Usage : ./LoadTester <server ip:port> <version>");
            exit(0);
        }
        int version = atoi(argv[2]);
        if(version == 4 and argc !=4)
        {
            throw("Usage : ./LoadTester <submission server ip:port> 4 <response server ip:port>");
        }

        string op;
        // Set size of the global array to hold statistics
        if(version ==4 ) {
            global_data.resize(6, 0);
            op = argv[3];
        }
        else {
            global_data.resize(5, 0);
            op= "";
        }

        LoadTester lt = LoadTester(argv[1],version,op.c_str());
        lt.run_test();
    }
    catch (char const *msg)
    {
        std::cerr << msg << std::endl;
    }
    return 0;
}