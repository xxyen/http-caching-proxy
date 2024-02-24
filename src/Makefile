all: main

main: main.cpp request.cpp response.cpp socket.cpp proxy.cpp cache.cpp
	g++ -o main main.cpp request.cpp response.cpp socket.cpp proxy.cpp cache.cpp -lpthread

clean:
	rm -rf *.o main