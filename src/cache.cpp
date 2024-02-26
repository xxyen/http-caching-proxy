#include "cache.hpp"
#include <sstream>
#include <iostream>
#include <vector>
#include <sys/socket.h>

Response Cache::get(std::string key) {
    if (cache.find(key) != cache.end()) {
        return cache[key];
    }
    else {
        return Response("");
    }

}
void Cache::put(std::string key, Response res) {
    cache[key] = res;
}
void Cache::revalidate(Response & res, Request & req, int server_fd) {
    std::string etag = res.getEtag();
    std::string last_modified = res.getLastModified();
    // if (etag == "" && last_modified == "") {
    //     return true;
    // }
    // std::cout << "req header (in revalidate()): " << req_header << std::endl << std::endl;
    
    std::string new_msg = req.getHeader();
    //std::cout << "original_msg: " << new_msg << std::endl << std::endl;
    //std::cout << "original_response: " << res.getResponse() << std::endl << std::endl;
    if (etag == "" && last_modified == "") {
        std::string request = req.getRequest();
        std::vector<char> new_req_msg(request.begin(), request.end());
        int send_len = send(server_fd, new_req_msg.data(), new_req_msg.size(), 0);
        //int send_len = send(server_fd, request.data(), request.size() + 1, 0);
        if (send_len <= 0) {
            std::cerr << "Error: send failed!" << std::endl;
        }
        //std::cout << "unchanged_msg: " << request << std::endl << std::endl;
        return;
    }
    if (etag != "") {
        new_msg = new_msg + "\r\n" + "If-None-Match: " + etag + "\r\n\r\n";
    }
    else if (last_modified != "") {
        new_msg = new_msg + "\r\n" + "If-Modified-Since: " + last_modified + "\r\n\r\n";
    }
    //std::cout << "new_msg: " << new_msg << std::endl << std::endl;
    std::vector<char> new_req_msg(new_msg.begin(), new_msg.end());
    int send_len = send(server_fd, new_req_msg.data(), new_req_msg.size(), 0);
    if (send_len <= 0) {
        std::cerr << "Error: send failed!" << std::endl;
    }
}