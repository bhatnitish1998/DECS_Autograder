# LoadTester and Clients

#### Instructions to run LoadTester
1. make
2. Start the version of the server to test
3. ./LoadTester \<server ip:port\> \<version number\>
4. Log files will be generated in Graphs_and_Logs directory with all the required metrics


#### Note: 
1. LoadTester establishes another tcp connection on control port 5001 with the server
2. To change the think time, iterations etc. modify the macros defined in LoadTester.hpp


#### Instructions to run individual Load generating clients
1. make
2. ./client \<server ip:port\> \<loopNum\> \<sleep time\> \<timeout\>  \<file to send\>
3. ./AsyncClient \<new|status\> \<submission server ip:port\>\<response server ip:port\>  \<loopNum\> \<timeout\>  \<file_to_send|request_id\>