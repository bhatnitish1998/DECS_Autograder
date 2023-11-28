#include "Database.hpp"

/// @brief Init connection to database in pqxx connection object
Database::Database(/* args */)
    : conn("postgresql://cs744:cs744@localhost:5432/grad?connect_timeout=10")
{
    if (conn.is_open())
    {
        std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
    }
    else
    {
        std::cout << "Can't open database" << std::endl;
    }
}
/// @brief Closes connection
Database::~Database()
{
}
/// @brief Delete all rows from table
void Database::deleteAll()
{
    // Delete all rows from the table
    pqxx::work txn(conn);
    txn.exec("DELETE FROM requests;");
    txn.commit();
}
/// @brief Print all rows from table
void Database::showAll()
{
    try
    {
        pqxx::work txn(conn);
        std::string sql = "SELECT * FROM REQUESTS;";
        pqxx::result result = txn.exec(sql);
        for (const auto &row : result)
        {
            auto size = row.size();
            for (int i = 0; i < size; i++)
            {
                std::cout << row[i].c_str() << " ";
            }
            std::cout << "\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
/// @brief Show details of a particular request id
/// @param req_id Request ID
void Database::show(uint32_t req_id)
{
    try
    {
        pqxx::work txn(conn);
        std::string sql = "SELECT * FROM REQUESTS WHERE req_id = " + txn.quote(req_id);

        pqxx::result R(txn.exec(sql));

        /* List down all the records */
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        {
            std::cout << "ID = " << c[0].as<uint32_t>() << std::endl;
            std::cout << "Program = " << c[1].as<std::string>() << std::endl;
            std::cout << "Request Status = " << c[2].as<std::string>() << std::endl;
            std::cout << "Grading status = " << c[3].as<std::string>() << std::endl;
            std::cout << "Output = " << c[4].as<std::string>() << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}
/// @brief Insert a new request to database
/// @param req Request struct variable
/// @return 0 on success
int Database::insertRequest(const Request &req)
{
    try
    {
        pqxx::work txn(conn);
        std::string sql = " INSERT INTO REQUESTS(REQ_ID, PROGRAM, REQUEST_STATUS, GRADING_STATUS, OUTPUT) \
        VALUES(" + std::to_string(req.req_id) +
                          "," + req.program_file + "," + req.request_status + "," + req.grading_status + "," + req.output + ");";
        txn.exec(sql);
        txn.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
/// @brief Insert a new request to database
/// @param req_id Request id as integer
/// @param program Program text
/// @param request_status Request status (default QUEUED)
/// @param grading_status Grading status (default empty)
/// @param output Grading output (default empty)
/// @return 0 on success
int Database::insertRequest(const uint32_t req_id, const std::string program, const std::string request_status, const std::string grading_status, const std::string output)
{
    try
    {
        std::string sql;
        pqxx::work txn(conn);
        if (request_status.empty())
        {
            sql = " INSERT INTO REQUESTS(REQ_ID, PROGRAM, REQUEST_STATUS, GRADING_STATUS, OUTPUT) \
        VALUES(" + std::to_string(req_id) +
                  "," + txn.quote(program) + "," + txn.quote("QUEUED") + "," + txn.quote(grading_status) + "," + txn.quote(output) + ");";
        }
        else
        {
            sql = " INSERT INTO REQUESTS(REQ_ID, PROGRAM, REQUEST_STATUS, GRADING_STATUS, OUTPUT) \
        VALUES(" + std::to_string(req_id) +
                  "," + txn.quote(program) + "," + txn.quote(request_status) + "," + txn.quote(grading_status) + "," + txn.quote(output) + ");";
        }

        txn.exec(sql);
        txn.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
/// @brief Update all fields of an existing request id
/// @param req_id
/// @param req
/// @return 0 on success
int Database::updateRequest(uint32_t req_id, const Request &req)
{
    try
    {
        pqxx::work txn(conn);
        std::string sql = "UPDATE REQUESTS SET "
                          "program_file = " +
                          txn.quote(req.program_file) + ", "
                                                        "request_status = " +
                          txn.quote(req.request_status) + ", "
                                                          "grading_status = " +
                          txn.quote(req.grading_status) + ", "
                                                          "output = " +
                          txn.quote(req.output) +
                          " WHERE req_id = " + txn.quote(req_id);
        txn.exec(sql);
        txn.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
/// @brief Update request status of a given id
/// @param req_id
/// @param new_status
/// @return 0 on success
int Database::updateReqStatus(uint32_t req_id, std::string new_status)
{
    try
    {
        pqxx::work txn(conn);
        std::string sql = "UPDATE REQUESTS SET "
                          "request_status = " +
                          txn.quote(new_status) +
                          " WHERE req_id = " + txn.quote(req_id);
        txn.exec(sql);
        txn.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
/// @brief  Update grading status of a request id
/// @param req_id
/// @param grad_status
/// @return 0 on successW
int Database::updateGradingStatus(uint32_t req_id, std::string grad_status)
{
    try
    {
        pqxx::work txn(conn);
        std::string sql = "UPDATE REQUESTS SET "
                          "grading_status = " +
                          txn.quote(grad_status) +
                          " WHERE req_id = " + txn.quote(req_id);
        txn.exec(sql);
        txn.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
/// @brief Update grading output of a request id
/// @param req_id
/// @param grad_output
/// @return 0 on success
int Database::updateGradingOutput(uint32_t req_id, std::string grad_output)
{
    try
    {
        pqxx::work txn(conn);
        std::string sql = "UPDATE REQUESTS SET "
                          "output = " +
                          txn.quote(grad_output) +
                          " WHERE req_id = " + txn.quote(req_id);
        txn.exec(sql);
        txn.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
/// @brief Queries and returns a row with given request id
/// @param req_id
/// @return row as Request struct
Request Database::queryFor(uint32_t req_id)
{
    try
    {
        pqxx::work txn(conn);
        std::string sql = "SELECT * FROM REQUESTS WHERE req_id = " + txn.quote(req_id);

        pqxx::result result(txn.exec(sql));
        if (!result.empty())
        {
            const auto &row = result[0];

            request.req_id = row["req_id"].as<uint32_t>();
            request.program_file = row["program_file"].c_str();
            request.request_status = row["request_status"].c_str();
            request.grading_status = row["grading_status"].c_str();
            request.output = row["output"].c_str();
        }
        else
        {
            request.req_id = -1;
            std::cout << "No data found for the given primary key." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return request;
}
