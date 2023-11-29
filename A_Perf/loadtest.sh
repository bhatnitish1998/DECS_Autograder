#!/bin/bash
if [ $# -ne 3 ]; then
    echo "Usage $0 <server-address> <num_clients>"
    exit 1
fi

LOOP=5
SLEEP=2
TIMEOUT=1

cd ../Client
for ((i = 0; i < $2; i++)); do
    ./client $1 $LOOP $SLEEP $TIMEOUT Test_files/tes4.cpp >../A_Perf/client$i.txt &
done

wait
