#include "LoadTester.h"

mutex global_mutex;
vector<double> global_data;

LoadTester::LoadTester(const char * server_info, int version, const char * response_server) :server_info(server_info),version(version),response_server(response_server){
    control_sockfd =0;
    establish_control_connection();
}

void LoadTester::run_client() {
    vector<double> data;
    if(version == 4)
    {
        AsyncClient client = AsyncClient(server_info.c_str(),response_server.c_str(),LOOPNUM,TIMEOUT);
        client.submit(PROGRAMFILE);
        data = client.get_statistics();
    }
    else
    {
        Client client = Client(server_info.c_str(), LOOPNUM, SLEEP, TIMEOUT,PROGRAMFILE);
        client.submit();
        data = client.get_statistics();
    }
    {
        unique_lock<mutex> lock(global_mutex);
        for (int i = 0; i < 5; i++) {
            global_data[i] += data[i];

        }
        if(version ==4) global_data[5]+=data[5];
    }
}

void LoadTester::establish_control_connection() {

    int t1 = server_info.find(":");
    string server_ip = server_info.substr(0, t1);

    control_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (control_sockfd < 0)
        throw std::runtime_error("Error opening control socket");

    sockaddr_in control_addr;
    memset(&control_addr, 0, sizeof(control_addr));
    control_addr.sin_family = AF_INET;
    control_addr.sin_port = htons(CONTROL_PORT);
    control_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());

    int sc = connect(control_sockfd, (struct sockaddr *)&control_addr, sizeof(control_addr));
    if (sc < 0)
        throw std::runtime_error("Cannot connect");
}

void LoadTester::send_long(uint32_t value)
{
    value = htonl(value);
    if (write(control_sockfd, &value, sizeof(value)) <= 0)
        throw std::runtime_error("error sending control message");
}


void LoadTester::run_test() {

    std::vector<std::thread> threads;

    ofstream fout("../Graphs_and_Logs/ver_"+ to_string(version)+".txt");
    fout<<"clients,requests,success_percent,timeout_percent,error_percent,avg_rt,throughput";
    if(version ==4)
        fout<<",avg_rt_2";
    fout<<endl;

    for (uint32_t i = INITIAL; i<=(TIMES*INTERVAL);i= i + INTERVAL)
    {

        send_long(i);
        cerr<<"Sent "<<i<<endl;

        auto start_time = chrono::high_resolution_clock::now();
        for(int j = 0; j<i;j++)
        threads.push_back(thread(&LoadTester::run_client,this));

        for (auto &x : threads)
            x.join();
        auto end_time = chrono::high_resolution_clock::now();
        double time_elapsed = chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count();
        threads.clear();

        fout<<i<<","; //clients
        fout << (global_data[0]/time_elapsed)*1000 << ","; // Request rate
        fout << (global_data[1]/global_data[0])*100 << ","; // Success percent
        fout << (global_data[2]/global_data[0])*100 << ","; // Timeout percent
        fout << (global_data[3]/global_data[0])*100 << ","; // other error percent
        fout << global_data[4]/global_data[1] << ","; // average response time
        fout << (global_data[1]/time_elapsed)*1000; // throughput
        if(version ==4) fout<<","<<global_data[5]/global_data[1];
        fout<<endl;

        // reset data for next iteration
        for(int k =0;k<5;k++) {
            global_data[k]=0;
        }
        if(version ==4) global_data[5]=0;
    }
    fout.close();
    uint32_t i = 100000;
    send_long(i);
}
