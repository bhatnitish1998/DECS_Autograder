#include "Worker.hpp"
// Unused Destructor
FastQueue grader_queue;
GradingWorker::~GradingWorker()
{
}

void SubmissionWorker::send_response(int sockfd, std::string response)
{
    uint32_t length_to_send = response.length();
    length_to_send = htonl(length_to_send);

    if (write(sockfd, &length_to_send, sizeof(length_to_send)) < 0)
        throw("error sending message size");

    if (write(sockfd, response.c_str(), response.length()) < 0)
        throw("error sending message");
}
void ResponseWorker::send_response(int sockfd, std::string response)
{
    uint32_t length_to_send = response.length();
    length_to_send = htonl(length_to_send);

    if (write(sockfd, &length_to_send, sizeof(length_to_send)) < 0)
        throw("error sending message size");

    if (write(sockfd, response.c_str(), response.length()) < 0)
        throw("error sending message");
}

void GradingWorker::cleanup()
{
    for (auto x : cleanuplist)
        remove(x.c_str());
}

SubmissionWorker::SubmissionWorker(int sockfd)
    : newsockfd(sockfd)
{
}
SubmissionWorker::~SubmissionWorker()
{
    close(newsockfd);
}
void SubmissionWorker::receive_file()
{
    program_file.clear();
    uint32_t file_size;
    if (read(newsockfd, &file_size, sizeof(file_size)) < 0)
        throw("file size read error");
    file_size = ntohl(file_size);

    char buffer[1024];
    uint32_t read_bytes = 0;
    while (read_bytes < file_size)
    {
        memset(buffer, 0, sizeof(buffer));
        uint32_t current_read = read(newsockfd, buffer, sizeof(buffer));
        if (current_read == 0)
            throw("socket read error");
        program_file += std::string(buffer, current_read);
        read_bytes += current_read;
    }
}
uint32_t SubmissionWorker::work()
{
    // receive the file as a string
    receive_file();
    // Check for system() in client code and reject if present
    std::regex pattern(" system\\(");
    if (!std::regex_search(program_file, pattern))
    {
        //  generate request id
        // gen_id();
        //  create entry in db with this id
        auto id = db.insertRequest(program_file);
        req_id = id;
        //  send request id and acknowledgement to client
        msg = "File submitted for grading. Your request id: <" + std::to_string(req_id) + ">";
    }
    else
    {
        //  send request id and acknowledgement to client
        msg = "Malicious activity detected in submitted program.\n";
    }
    send_response(newsockfd, msg);
    //  return req id for grading queue
    return req_id;
}
int writeStringToFile(std::string file_name, std::string content)
{
    std::ofstream outputFile(file_name);
    if (outputFile.is_open())
    {
        outputFile << content;
        outputFile.close();
        // std::cout << "Program string written to file: " << file_name << std::endl;
    }
    else
    {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
    }
    return 0;
}
/// @brief Creates a worker for grading job
/// @param request_id request id to grade
GradingWorker::GradingWorker(uint32_t request_id)
    : req_id(request_id), done(false), base_dir("temp/")
{
    // Request present in DB
    if (fetchDB() == 0)
    {
        db.updateReqStatus(req_id, "PROCESSING");
        std::string directoryPath = "temp";

        // Check if the temp directory exists
        if (!std::filesystem ::exists(directoryPath))
        {
            // Create the directory
            try
            {
                std::filesystem::create_directory(directoryPath);
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << "Error creating temp directory: " << e.what() << std::endl;
            }
        }
        file_identifier = base_dir + std::to_string(req_id);
        program_file = file_identifier + "prog_.cpp";
        executable_file = file_identifier + "exec";
        output_file = file_identifier + "output.txt";
        // Write fetched program string to local file
        writeStringToFile(program_file, req.program_file);
    }
}
double GradingWorker::work()
{
    auto start_time = std::chrono::high_resolution_clock::now();
    compile();
    if (!done)
        run_program();
    if (!done)
        compare_output();
    // Program is as expected
    if (!done)
    {
        req.output = "";
        req.request_status = "GRADED";
        req.grading_status = "PASS";
        db.updateRequest(req_id, req);
    }
    cleanup();
    auto end_time = std::chrono::high_resolution_clock::now();
    double service_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    return service_time;
}

std::string readFromFile(const std::string &fileName)
{
    std::ifstream file(fileName);
    if (file.is_open())
    {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return content;
    }
    else
    {
        throw std::runtime_error("Error: Unable to open file for reading.");
    }
}
int GradingWorker::fetchDB()
{
    req = db.queryFor(req_id);
    if (req.req_id == -1)
        return -1;
    else
        return 0;
}

void GradingWorker::compile()
{
    std::string compiler_filename = file_identifier + "compiler.txt";
    std::string cmd = "g++ -o " + executable_file + " " + program_file + " > /dev/null 2> " + compiler_filename;
    if (system(cmd.c_str()) != 0)
    {
        msg = "COMPILER ERROR\n";
        // write msg as grading status, update request status to GRADED, update output as contents of compiler_filename
        req.grading_status = msg;
        req.request_status = "GRADED";
        req.output = readFromFile(compiler_filename);
        db.updateRequest(req_id, req);
        done = true;
    }
    cleanuplist.push_back(compiler_filename);
    cleanuplist.push_back(program_file);
}

void GradingWorker::run_program()
{
    std::string runtime_filename = file_identifier + "runtime.txt";
    std::string cmd = "./" + executable_file + " > " + output_file + " 2> " + runtime_filename;
    if (system(cmd.c_str()) != 0)
    {
        msg = "RUNTIME ERROR\n";
        // write msg as grading status, update request status to GRADED, update output as contents of runtime_filename
        req.grading_status = msg;
        req.request_status = "GRADED";
        req.output = readFromFile(runtime_filename);
        db.updateRequest(req_id, req);
        done = true;
    }
    cleanuplist.push_back(runtime_filename);
    cleanuplist.push_back(executable_file);
    cleanuplist.push_back(output_file);
}

void GradingWorker::compare_output()
{
    std::string diff_filename = file_identifier + "diff.txt";
    std::string cmd = "diff -Z " + output_file + " ../../../Test_files/solution.txt > " + diff_filename;

    if (system(cmd.c_str()) != 0)
    {
        msg = "OUTPUT ERROR\n";
        // write msg as grading status, update request status to GRADED, update output as contents of diff_filename
        req.grading_status = msg;
        req.request_status = "GRADED";
        req.output = readFromFile(diff_filename);
        db.updateRequest(req_id, req);
        done = true;
    }
    cleanuplist.push_back(diff_filename);
}

ResponseWorker::ResponseWorker(int sockfd)
    : sock_fd(sockfd)
{
    recv_req_id();
}

ResponseWorker::~ResponseWorker()
{
    close(sock_fd);
}
void ResponseWorker::recv_req_id()

{
    uint32_t reqId;

    if (read(sock_fd, &reqId, sizeof(reqId)) < 0)
        throw("Request id read error");
    req_id = ntohl(reqId);
}

uint32_t ResponseWorker::findQueuePos()
{
    return req_id;
}

uint32_t ResponseWorker::getWaitTime()
{
    return 2;
}

void ResponseWorker::work()
{
    req = db.queryFor(req_id);
    if (req.req_id == -1)
    {
        msg = "Grading request ID: <" + std::to_string(req_id) + "> not found. Please check and resend your request ID or re-send your original grading request\n";
        send_response(sock_fd, msg);
    }
    else if (req.request_status == "QUEUED")
    {
        auto queue_pos = grader_queue.get_index(req.req_id);
        msg = "Your grading request ID: <" + std::to_string(req_id) + "> has been accepted.It is currently at position " + std::to_string(queue_pos) + "\nApprox wait time(seconds):" + std::to_string(queue_pos * getWaitTime());
        send_response(sock_fd, msg);
    }
    else if (req.request_status == "PROCESSING")
    {
        msg = "Your grading request ID: <" + std::to_string(req_id) + "> has been accepted and is currently being processed.";
        send_response(sock_fd, msg);
    }
    else if (req.request_status == "GRADED")
    {
        msg = "Your grading request ID: <" + std::to_string(req_id) + "> processing is done, here are the results:\n" + req.grading_status + "\n" + req.output;
        send_response(sock_fd, msg);
    }
}