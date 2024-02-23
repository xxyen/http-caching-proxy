#include "socket.hpp"
#include <pthread.h>
#include <vector>

class Proxy {
private:
    char* port;

public:
    Proxy(char* port);
    ~Proxy();
    void start();
    static void* handleRequest(void* clientFd);

};

