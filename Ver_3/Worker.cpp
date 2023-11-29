#include "Worker.hpp"


uint32_t filename_ticket = 0;
std::mutex ticket_lock;


Worker ::Worker(int newsockfd) : newsockfd(newsockfd)
{
    ticket_lock.lock();
    file_identifier = std::to_string(++filename_ticket);
    ticket_lock.unlock();
    program_file = file_identifier + "prog.cpp";
    msg = "PASS\n";
    file_to_send = "";
    executable_file = file_identifier + "exec";
    output_file = file_identifier + "output.txt";
    done = false;
}

void Worker ::send_response()
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
    uint32_t response_size = std::filesystem::file_size(file_to_send);
    response_size += msg.length();
    uint32_t length_to_send = htonl(response_size);

    if (write(newsockfd, &length_to_send, sizeof(length_to_send)) < 0)
        throw("error sending filesize");

    if (write(newsockfd, msg.c_str(), msg.length()) < 0)
        throw("error sending message");

    std::ifstream fin(file_to_send, std::ios::binary);
    if (!fin)
        throw("error opening file");

    char buffer[1024];
    while (!fin.eof())
    {
        memset(buffer, 0, sizeof(buffer));
        fin.read(buffer, sizeof(buffer));
        int k = fin.gcount();
        int n = write(newsockfd, buffer, k);
        if (n < 0)
            throw("error writing file");
    }
}

void Worker::receive_file()
{
    uint32_t file_size;
    if (read(newsockfd, &file_size, sizeof(file_size)) < 0)
        throw("file size read error");
    file_size = ntohl(file_size);

    std::ofstream fout(program_file.c_str(), std::ios::binary);
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

void Worker::compile()
{
    std::string compiler_filename = file_identifier + "compiler.txt";
    std::string cmd = "g++ -o " + executable_file + " " + program_file + " > /dev/null 2> " + compiler_filename;
    if (system(cmd.c_str()) != 0)
    {
        msg = "COMPILER ERROR\n";
        file_to_send = compiler_filename.c_str();
        done = true;
    }
    cleanuplist.push_back(compiler_filename);
    cleanuplist.push_back(program_file);
}

void Worker::run_program()
{
    std::string runtime_filename = file_identifier + "runtime.txt";
    std::string cmd = "./" + executable_file + " > " + output_file + " 2> " + runtime_filename;
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

void Worker::compare_output()
{
    std::string diff_filename = file_identifier + "diff.txt";
    std::string cmd = "diff -Z " + output_file + " ../Test_files/solution.txt > " + diff_filename;

    if (system(cmd.c_str()) != 0)
    {
        msg = "OUTPUT ERROR\n";
        file_to_send = diff_filename.c_str();
        done = true;
    }
    cleanuplist.push_back(diff_filename);
}

double Worker::process_request()
{
    auto start_time = std::chrono::high_resolution_clock::now();
    receive_file();
    compile();
    if (!done)
        run_program();
    if (!done)
        compare_output();
    send_response();
    cleanup();
    auto end_time = std::chrono::high_resolution_clock::now();
    double service_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count();
    return service_time;
}

void Worker:: cleanup()
    {
        for (auto x : cleanuplist)
            remove(x.c_str());

        close(newsockfd);
    }