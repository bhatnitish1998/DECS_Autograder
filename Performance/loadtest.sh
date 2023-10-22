SERVER="127.0.0.1:9001"
CLIENTS=$1
LOOP=$2
SLEEP=$3
TIMEOUT=$4
cd ../Client
for((i = 0 ; i < $CLIENTS ; i++))
do
./client $SERVER $LOOP $SLEEP $TIMEOUT > ../Performance/client$i.txt &
done

wait

