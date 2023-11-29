# DECS_Autograder Ver 4

This is the implementation of ver 4 Asynchronous server and client. We have made separate server for accepting requests and sending status response

`async-server` houses code for both these servers.
`async-client` houses code for the asynchronous client.

### Build
- run `createDB.sh -i` to install libpqxx and install postgresql(requires super user permissions)
- To build run `make` from root directory

### Making a new request
#### Run submission server
`./bin/submission_server <port> <thread-pool size>`
#### Run client
`./bin/submit new <submission-server-ip:port> <test_file_path>`

All test files are under Test_files

### Making a status check
#### Run submission server
`./bin/response_server <port> <thread-pool size>`
#### Run client
`./bin/submit status  <response server-ip:port> <req-id>`
