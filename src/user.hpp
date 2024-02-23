#include <string>

class User {
private:
    int client_fd;
    int thread_id;
    std::string ip;
public:
    User(int client_fd, int thread_id, std::string ip) : client_fd(client_fd), thread_id(thread_id), ip(ip) {}
    int getClientFd() { return client_fd; }
    int getThreadId() { return thread_id; }
    std::string getIp() { return ip; }
};