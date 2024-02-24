#include"cache.hpp"
#include <sstream>
#include <iostream>
#include<vector>
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
void Cache::revalidate(Response& res, std::string req_header, int server_fd) {
    std::string etag = res.getEtag();
    std::string last_modified = res.getLastModified();
    // if (etag == "" && last_modified == "") {
    //     return true;
    // }
    std::string new_msg;
    if (etag != "") {
        new_msg = req_header + "\r\n" + "If-None-Match: " + etag + "\r\n\r\n";
    }
    if (last_modified != "") {
        new_msg = req_header + "\r\n" + "If-Modified-Since: " + last_modified + "\r\n\r\n";
    }
    std::vector<char> new_req_msg(new_msg.begin(), new_msg.end());
    int send_len = send(server_fd, new_req_msg.data(), new_req_msg.size(), 0);
    if (send_len <= 0) {
        std::cerr << "Error: send failed!" << std::endl;
    }
}