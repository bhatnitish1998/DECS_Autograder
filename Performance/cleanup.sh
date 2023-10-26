#!/bin/bash
cat client*.txt > all_clients.txt
grep : all_clients.txt > grep_out.txt
awk -f "final.awk" grep_out.txt > global_data.txt
rm -f client*.txt
rm -f grep_out.txt
rm -f all_clients.txt
