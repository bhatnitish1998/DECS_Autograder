#include "LoadTester.h"

mutex global_mutex;
vector<double> global_data;

LoadTester::LoadTester(const char * server_info) :server_info(server_info){}

void LoadTester::run_client() {
    Client client = Client(server_info.c_str(), LOOPNUM, SLEEP, TIMEOUT);
    client.submit();
    vector<double> data = client.get_statistics();
    {
        unique_lock<mutex> lock;
        for (int i = 0; i < 5; i++) {
            global_data[i] += data[i];
        }
    }
}

void LoadTester::run_test() {

    std::vector<std::thread> threads;

    ofstream fout("../Graphs_and_Logs/ver3.txt");
    fout<<"clients,requests,success,timeout,error,avg_rt,throughput\n";

    for (int i = INITIAL; i<=(TIMES*INTERVAL);i= i + INTERVAL)
    {
        auto start_time = chrono::high_resolution_clock::now();
        for(int j = 0; j<i;j++)
        threads.push_back(thread(&LoadTester::run_client,this));

        for (auto &x : threads)
            x.join();
        auto end_time = chrono::high_resolution_clock::now();
        double time_elapsed = chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count();
        threads.clear();

        fout<<i<<","; //clients
        fout << global_data[0] << ","; // Requests
        fout << global_data[1] << ","; // Successes
        fout << global_data[3] << ","; // Timeouts
        fout << global_data[4]/global_data[1] << ","; // average response time
        fout << (global_data[1]/time_elapsed)*1000; // throughput
        fout<<endl;

        // reset data for next iteration
        for(int k =0;k<5;k++) {
            global_data[k]=0;
        }
    }
    fout.close();
}
