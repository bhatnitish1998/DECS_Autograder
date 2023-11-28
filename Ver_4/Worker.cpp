#include "headers/Worker.hpp"
uint32_t request_id = 0;
std::mutex id_lock;
void Worker ::send_response(int sockfd, std::string response)
{
    uint32_t length_to_send = response.length();
    length_to_send = htonl(length_to_send);

    if (write(sockfd, &length_to_send, sizeof(length_to_send)) < 0)
        throw("error sending message size");

    if (write(sockfd, response.c_str(), response.length()) < 0)
        throw("error sending message");
}

void Worker::cleanup()
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
void SubmissionWorker::gen_id()
{
    id_lock.lock();
    req_id = ++request_id;
    id_lock.unlock();
}
uint32_t SubmissionWorker::work()
{
    // receive the file as a string
    receive_file();
    //  generate request id
    gen_id();
    //  create entry in db with this id
    //  send request id and acknowledgement to client
    msg = "File submitted for grading. Your request id: " + std::to_string(req_id);
    send_response(newsockfd, msg);
    //  return req id for grading queue
    return req_id;
}
