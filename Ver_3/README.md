# DECS_Autograder Ver 3

Autograder with threadpool.

A control thread periodically checks for cpu utilization, number of threads, queue size  and service time and logs it.

#### Instructions to run
1. make
2. ./server \<server ip:port\> \<threadpool_size\>
3. Do not change the name of executable as cpu utilization and number of active threads is based on process name.

