#!/bin/bash

INTERVAL=10
ITERATION=5
SLEEP=1
TIMEOUT=0.1

>graph_in.txt

g++ -o performance performance.cpp
./performance 1 $ITERATION $SLEEP $TIMEOUT >> graph_in.txt

for ((i =1 ; i< 20 ; i++ ))
do
CLIENTS=$((INTERVAL*i))
./performance $CLIENTS $ITERATION $SLEEP $TIMEOUT >> graph_in.txt
sleep 2
echo "Done $CLIENTS"
done

grep clients graph_in.txt | cut -d"=" -f2 > clients.txt
grep Throughput graph_in.txt | cut -d"=" -f2 > throughput.txt
grep response graph_in.txt | cut -d"=" -f2 > response.txt
grep Request graph_in.txt | cut -d"=" -f2 > request.txt
grep Success graph_in.txt | cut -d"=" -f2 > success.txt
grep Timeout graph_in.txt | cut -d"=" -f2 > timeout.txt
grep Error graph_in.txt | cut -d"=" -f2 > error.txt


mkdir -p graphs

paste clients.txt throughput.txt | graph -T png --bitmap-size "1400x1400"  -L "Throughput vs Number of Clients" -X "Number of Clients" -Y "Throughput" > ./graphs/throughput.png

paste clients.txt response.txt | graph -T png --bitmap-size "1400x1400"  -L "Response time vs Number of Clients" -X "Number of Clients" -Y "Response time in ms" > ./graphs/response.png

paste clients.txt request.txt | graph -T png --bitmap-size "1400x1400"  -L "Request rate vs Number of Clients" -X "Number of Clients" -Y "Request rate" > ./graphs/request.png

paste clients.txt success.txt | graph -T png --bitmap-size "1400x1400"  -L "Success  vs Number of Clients" -X "Number of Clients" -Y "Success Percent " > ./graphs/success.png

paste clients.txt timeout.txt | graph -T png --bitmap-size "1400x1400"  -L "Timeout vs Number of Clients" -X "Number of Clients" -Y "Timeout Percent" > ./graphs/timeout.png

paste clients.txt error.txt | graph -T png --bitmap-size "1400x1400"  -L "Error vs Number of Clients" -X "Number of Clients" -Y "Error Percent" > ./graphs/error.png

rm -f throughput.txt
rm -f response.txt
rm -f clients.txt
rm -f graph_in.txt
rm -f success.txt
rm -f request.txt
rm -f error.txt

