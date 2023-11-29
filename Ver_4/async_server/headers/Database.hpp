#ifndef __DB_hpp__
#define __DB_hpp__ 1
#include <pqxx/pqxx>
#include <exception>
#include <iostream>
#include <random>
#include <vector>

struct Request
{
    uint32_t req_id;
    std::string program_file;
    std::string request_status;
    std::string grading_status;
    std::string output;
};
class Database
{
private:
    pqxx::connection conn;
    Request request;
    std::vector<uint32_t> pendingTasks;

public:
    void deleteAll();
    std::vector<uint32_t> getPendingTasks();
    void showAll();
    void show(uint32_t req_id);
    uint32_t insertRequest(const Request &req);
    uint32_t insertRequest(const std::string program, const std::string request_status = "", const std::string grading_status = "", const std::string output = "");
    int updateRequest(uint32_t req_id, const Request &req);
    int updateReqStatus(uint32_t req_id, std::string new_status);
    int updateGradingStatus(uint32_t req_id, std::string grad_status);
    int updateGradingOutput(uint32_t req_id, std::string grad_output);
    Request queryFor(uint32_t req_id);
    Database(/* args */);
    ~Database();
};

#endif