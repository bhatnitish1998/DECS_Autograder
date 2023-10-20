#!/bin/bash
if [ $# -lt 4 ]; then
    echo "usage: $0 <numClients>  <loopNum>  <sleepTimeSeconds> <timeout>"
    exit 1
fi
declare -a bg_pids
port=9750
g++ -o client loadGenerating-client_m.cpp
file="test.c"
ip="localhost"

./cpu_util.sh &
pid=$!
sleep 5
echo "start" >>cpu_threads.txt
start_time=$(date +%s.%3N)
for ((i = 1; i <= $1; i++)); do
    ./client $ip:$port $file $2 $3 $4 >>result_$i.txt &
    bg_pids+=($!)
done
for val in "${bg_pids[@]}"; do
    wait $val
done
end_time=$(date +%s.%3N)
elapsed_time=$(echo "$end_time $start_time" | awk '{print $1 - $2}')

# Print the result
# echo "Time elapsed: $elapsed_time seconds"
echo "End" >>cpu_threads.txt
# echo $0 $1 $2 $3
response_time_sum=0
total_success=0
total_requests=0
n_timeouts=0
n_errors=0
total_loop_time=0
for file in result_*.txt; do
    response_time_sum=$(awk -v rtsum="$response_time_sum" 'BEGIN { rt=0;n_resp=0; } /Successful responses:/{n_resp = $NF;} /Average response time:/{ rt = n_resp * $(NF - 1) } END { rtsum+=rt; print rtsum }' $file)
    total_success=$(awk -v succ="$total_success" '/Successful responses:/{ succ += $NF} END { print succ }' $file)
    total_requests=$(awk -v nreq="$total_requests" '/Number of requests:/{ nreq += $NF} END { print nreq }' $file)
    n_timeouts=$(awk -v timo="$n_timeouts" '/Timeouts:/{ timo += $NF} END { print timo }' $file)
    n_errors=$(awk -v err="$n_errors" '/Errors:/{ err += $NF} END { print err }' $file)
    total_loop_time=$(awk -v loop_t="$total_loop_time" '/Time to loop completion:/{loop_t += $(NF-1)} END { print loop_t }' $file)
    rm $file
done
avg_rt=$(awk -v rtsum="$response_time_sum" -v succ="$total_success" 'BEGIN{ print rtsum/succ }')
throughput=$(awk -v tot_t="$total_loop_time" -v succ="$total_success" 'BEGIN { print 1000*succ/tot_t}')

avg_threads=$(
    awk '/start/ { p = 1; next } /End/ { p = 0 } p { print }' cpu_threads.txt |
        awk '/^Threads:/{ match($0, /[0-9]+ total/); 
    if (RSTART) { 
    thread_value = substr($0, RSTART, RLENGTH - 6); 
    total += thread_value; 
    count++;
    }} 
    END { 
        if (count > 0) 
            print total/count 
        }'
)

avg_cpu_util=$(awk '/start/ { p = 1; next } /End/ { p = 0 } p { print }' cpu_threads.txt |
    awk '/^%Cpu\(s\):/ {
        split($2, us, "us,");
        split($4, sy, "sy,");
        tot = us[1] + sy[1]
        total_us_sy += tot;
        count++;
    }
    END {
        if (count > 0) {
            avg_us_sy = total_us_sy / count;
            print avg_us_sy;
        }
    }')

throughput=$(echo "$elapsed_time $total_success" | awk '{print $2/$1}')
echo "$1 $avg_rt $throughput $total_requests $total_success $n_timeouts $n_errors $avg_cpu_util $avg_threads" >>data.txt
sleep 5
rm cpu_threads.txt
kill -9 $pid
killall python
# killall vmstat
