#include <unordered_map>
#include "response.hpp"
#include "request.hpp"
class Cache {
private:
    std::unordered_map<std::string, Response> cache;
    size_t capacity;
public:
    // std::unordered_map<std::string, Response> cache;
    Cache() {}
    Cache(size_t cap) : capacity(cap) {}
    Response get(std::string key);
    void put(std::string key, Response res);
    void revalidate(Response & res, Request & req, int server_fd);
};

