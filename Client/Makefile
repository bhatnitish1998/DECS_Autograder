all : LoadTester client AsyncClient normalclient

LoadTester : main.o LoadTester.o Client.o AsyncClient.o
	g++ -o LoadTester main.o LoadTester.o Client.o AsyncClient.o
	
normalclient :	normal_main.o normal_client.o
	g++ -o normalclient normal_main.o normal_client.o

client : Client.o client_main.o
	g++ -o client Client.o client_main.o

AsyncClient : AsyncClient.o async_main.o
	g++ -o AsyncClient AsyncClient.o async_main.o

LoadTester.o :  LoadTester.h LoadTester.cpp
	g++ -c LoadTester.cpp
	
normal_client.o : normal_main.cpp normal_client.hpp
	g++ -c normal_client.cpp

Client.o : Client.hpp Client.cpp
	g++ -c Client.cpp

AsyncClient.o : AsyncClient.hpp AsyncClient.cpp
	g++ -c AsyncClient.cpp

main.o : main.cpp
	g++ -c main.cpp

client_main.o : client_main.cpp
	g++ -c client_main.cpp
	
async_main.o :async_main.cpp
	g++ -c async_main.cpp
	
normal_main.o : normal_main.cpp
	g++ -c normal_main.cpp
	
clean: 
	rm -f *.o LoadTester client AsyncClient normalclient
