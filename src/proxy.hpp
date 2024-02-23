#include "socket.hpp"
#include <pthread.h>
#include <vector>

class Proxy {
private:
    const char* port;

public:
    Proxy(const char* port);
    ~Proxy();
    void start();
    static void* handleRequest(void* userInfo);
    static std::string getCurrentTime();
};

