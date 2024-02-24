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
    static void handleConnect(int user_fd, int server_fd, int thread_id);
    static std::string getCurrentTime();
};

