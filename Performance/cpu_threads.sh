#!/bin/bash
p=$(pidof ./server)

if [ -z "$p" ]; then
    echo "Server not running!"
    exit 1
fi
#taskset -c -p 0-1 $p

while ((i = 1)); do
    top -p $p -b -n 1 | grep Cpu >>cpu_threads.txt
    ps -o nlwp $p | awk '/[0-9]+/{print "Threads:" $0 " total"}' >>cpu_threads.txt
    sleep 0.5
done
