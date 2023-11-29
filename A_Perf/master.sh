#!/bin/bash

INTERVAL=10
ITERATION=20
INITIAL=5
SLEEP=1
TIMEOUT=0.1

if [ $# -ne 2 ]; then
    echo "Usage: $0 <server-address>"
fi

for ((i = $INITIAL; i < $ITERATION; i++)); do
    CLIENTS=$((INTERVAL * i))
    ./loadtest.sh $1 $CLIENTS
    python3 accumulate.py
    rm -rf client*.txt
    sleep 2
    echo "Done $CLIENTS clients"
done
