Server : main.o Worker.o Server.o
	g++ -o server main.o Worker.o Server.o

Worker.o :  Worker.hpp Worker.cpp
	g++ -c Worker.cpp

	
Server.o : Server.hpp Server.cpp
	g++ -c Server.cpp

main.o : main.cpp
	g++ -c main.cpp

clean: 
	rm -f *.o server
