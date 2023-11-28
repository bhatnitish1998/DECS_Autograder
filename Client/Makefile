all : LoadTester client

LoadTester : main.o LoadTester.o Client.o
	g++ -o LoadTester main.o LoadTester.o Client.o
	

client : Client.o client_main.o
	g++ -o client Client.o client_main.o

LoadTester.o :  LoadTester.h LoadTester.cpp
	g++ -c LoadTester.cpp
	
Client.o : Client.hpp Client.cpp
	g++ -c Client.cpp

main.o : main.cpp
	g++ -c main.cpp

client_main.o : client_main.cpp
	g++ -c client_main.cpp
	
clean: 
	rm -f *.o LoadTester client
