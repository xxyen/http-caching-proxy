all: testparse

testparse: testparse.cpp request.cpp response.cpp
	g++ -o testparse testparse.cpp request.cpp response.cpp

clean:
	rm -rf *.o testparse