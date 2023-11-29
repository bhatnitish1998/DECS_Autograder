#!/bin/bash
# generate data which will be consumed by python scripts to plot graph
# need to make 4 txt files representing statistics(performance metrics chosen) for each server version
# as number of clients increases eg:
######   Clients   Req Rate   Timeout percent   Other Error percent   Avg Response Time   Throughput   ###############
#V1         5        100            30                  10                  650               10
#V2         5        90             30                  10                  750               25
#V3         5        70             30                  10                  250               30
#V4         5        100            30                  10                  150               40
## The above will be collected for different number of clients ##
# Steps:
# 1. A loadtestscript will run that will run a given NUMBER of CLIENTS with given loopnum, sleeptime & timeout values.                  DONE
# 2. We will run each server MANUALLY and then run the above script giving it the server address.
# 3. Each client will PRINT to STDOUT the cumulative stats of the number of times it sent requests.
# 4. Will redirect the above to different files. PARSE these files to create a cumulative across corresponding metric of each client.
# 5. This generates one single row of the above example table for a particular server version.
# 6. Will do the above in a loop, clearing previous files & increasing client count upto some range
# 7. Just repeat the above steps for each server
# 8. For distributed measurements, we can run a peer on client and on server side.
# 9. Each peer will have their relevant scripts to generate statistics.
# 10. Connect both peers, signal server side to start statistic gathering through its scripts.
# 11. When testing is done for a particular client num, signal the server side peer to stop and send the log file to client side.
# 12. Plot graphs on client side.
