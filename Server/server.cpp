#include <iostream>
#include <string>
#include <mutex>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <stdlib.h>
#include <filesystem>
#include <vector>
#include <thread>

using namespace std;

uint32_t filename_ticket = 0;
mutex ticket_lock;

class Worker
{
    int newsockfd;
    string file_identifier;
    string program_file;
    string executable_file;
    string output_file;
    string msg;
    string file_to_send;
    bool done;
    vector<string> cleanuplist;

    void cleanup()
    {
        for (auto x : cleanuplist)
            remove(x.c_str());

        close(newsockfd);
    }

    void receive_file()
    {
        uint32_t file_size;
        if (read(newsockfd, &file_size, sizeof(file_size)) < 0)
            throw("file size read error");
        file_size = ntohl(file_size);

        ofstream fout(program_file.c_str(), ios::binary);
        if (!fout)
            throw("error opening program file");

        char buffer[1024];
        uint32_t read_bytes = 0;
        while (read_bytes < file_size)
        {
            memset(buffer, 0, sizeof(buffer));
            uint32_t current_read = read(newsockfd, buffer, sizeof(buffer));
            if (current_read == 0)
                throw("socket read error");
            fout.write(buffer, current_read);
            if (fout.bad())
                throw("file write error");
            read_bytes += current_read;
        }
        fout.close();
    }

    void send_response()
    {
        if (file_to_send == "")
        {
            uint32_t length_to_send = msg.length();
            length_to_send = htonl(length_to_send);

            if (write(newsockfd, &length_to_send, sizeof(length_to_send)) < 0)
                throw("error sending message size");
            
            if (write(newsockfd, msg.c_str(), msg.length()) < 0)
                throw("error sending message");

            return;
        }
        uint32_t response_size = filesystem::file_size(file_to_send);
        response_size += msg.length();
        uint32_t length_to_send = htonl(response_size);

        if (write(newsockfd, &length_to_send, sizeof(length_to_send)) < 0)
            throw("error sending filesize");

        if (write(newsockfd, msg.c_str(), msg.length()) < 0)
            throw("error sending message");
        
        ifstream fin(file_to_send, ios::binary);
        if (!fin)
            throw("error opening file");
        
        char buffer[1024];
        while (!fin.eof())
        {
            memset(buffer, 0, sizeof(buffer));
            fin.read(buffer, sizeof(buffer));
            int k = fin.gcount();
            // todo: handle if kernel fails to send the data
            int n = write(newsockfd, buffer, k);
            if (n < 0)
                throw("error writing file");
        }
    }

    void compile()
    {
        string compiler_filename = file_identifier + "compiler.txt";
        string cmd = "g++ -o " + executable_file + " " + program_file + " > /dev/null 2> " + compiler_filename;
        if (system(cmd.c_str()) != 0)
        {
            msg = "COMPILER ERROR\n";
            file_to_send = compiler_filename.c_str();
            done = true;
        }
        cleanuplist.push_back(compiler_filename);
        cleanuplist.push_back(program_file);
    }

    void run_program()
    {
        string runtime_filename = file_identifier + "runtime.txt";
        string cmd = "./" + executable_file + " > " + output_file + " 2> " + runtime_filename;
        if (system(cmd.c_str()) != 0)
        {
            msg = "RUNTIME ERROR\n";
            file_to_send = runtime_filename.c_str();
            done = true;
        }
        cleanuplist.push_back(runtime_filename);
        cleanuplist.push_back(executable_file);
        cleanuplist.push_back(output_file);
    }

    void compare_output()
    {
        string diff_filename = file_identifier + "diff.txt";
        string cmd = "diff -Z " + output_file + " solution.txt > " + diff_filename;

        if (system(cmd.c_str()) != 0)
        {
            msg = "OUTPUT ERROR\n";
            file_to_send = diff_filename.c_str();
            done = true;
        }
        cleanuplist.push_back(diff_filename);
    }

public:
    Worker(int newsockfd) : newsockfd(newsockfd)
    {
        ticket_lock.lock();
        file_identifier = to_string(++filename_ticket);
        ticket_lock.unlock();
        program_file = file_identifier + "prog.cpp";
        msg = "PASS\n";
        file_to_send = "";
        executable_file = file_identifier + "exec";
        output_file = file_identifier + "output.txt";
        done = false;
    }

    void process_request()
    {
        receive_file();
        compile();
        if (!done)
            run_program();
        if (!done)
            compare_output();
        send_response();
        cleanup();
    }
};

class Server
{
    int port;
    int sockfd;
    sockaddr_in server_addr;
    int backlog;

    void setup_socket()
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            throw("Error opening socket");

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        int sc = bind(sockfd, (sockaddr *)&server_addr, sizeof(server_addr));
        if (sc < 0)
            throw("Error while binding");

        listen(sockfd, backlog);
        cout << "Server ready and listening at " << server_addr.sin_addr.s_addr << ":" << port << endl;
    }

public:
    Server(int port) : port(port)
    {
        backlog = 5;
        setup_socket();
    }

    void accept_requests()
    {
        sockaddr_in client_addr;
        socklen_t client_length = sizeof(client_addr);

        int newsockfd = accept(sockfd, (sockaddr *)&client_addr, &client_length);
        if (newsockfd < 0)
            throw("Error accepting connection");

        thread (&Server::thread_function, this, newsockfd).detach();
    }

    void thread_function(int newsockfd)
    {
        Worker worker(newsockfd);
        // cout << "TID: " << this_thread::get_id() << endl;
        worker.process_request();
    }

};

int main(int argc, char const *argv[])
{
    // Process arguments
    int port;
    if (argc != 2)
    {
        perror("Usage : ./server <port>");
        exit(1);
    }
    port = stoi(argv[1]);

    // int port = 9001;

    try
    {
        Server myserver(port);
        while (1)
            myserver.accept_requests();
    }
    catch (const char *msg)
    {
        cerr<<msg<<endl;
    }

    return 0;
}