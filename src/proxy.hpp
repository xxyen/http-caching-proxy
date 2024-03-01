#include "socket.hpp"
#include"cache.hpp"
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
    static void handlePost(int user_fd, int server_fd, int thread_id, vector<char> message, const char* hostname);
    static void handleGet(int user_fd, int server_fd, int thread_id, vector<char> message, const char* hostname, Cache* cache);
    static std::string getCurrentTime();
    static int recv_all(int _fd, vector<char>& msg);
    static void send502Response(int user_fd, int thread_id);
};

