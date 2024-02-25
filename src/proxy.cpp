#include "proxy.hpp"
#include "user.hpp"
#include "request.hpp"
// #include "response.hpp"
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <fstream>
#include <ctime>

#define MAX_SIZE 65536

// std::ofstream logDoc("/var/log/erss/proxy.log");
std::ofstream logDoc("proxy.log");

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

Cache* cache = new Cache(10);

Proxy::Proxy(const char* port) : port(port) {}

Proxy::~Proxy() {
    // If threads are joinable or need to be cleaned up, do it here
    // pthread_join or pthread_detach can be used depending on the use case
}

void Proxy::start() {
    Server server(port);
    int state = server.createServer();
    if (state < 0) {
        pthread_mutex_lock(&mutex);
        logDoc << "(no-id): ERROR in creating socket!" << endl;
        pthread_mutex_unlock(&mutex);
        return;
    }
    int thread_id = 0;
    int user_fd;
    std::string ip;
    while (true) {
        user_fd = server.acceptClient(&ip);
        if (user_fd < 0) {
            pthread_mutex_lock(&mutex);
            logDoc << "(no-id): ERROR Failed to accept client connection." << endl;
            pthread_mutex_unlock(&mutex);
            continue;
        }

        pthread_t thread;

        pthread_mutex_lock(&mutex);
        User* user = new User(user_fd, thread_id, ip);
        thread_id++;
        pthread_mutex_unlock(&mutex);

        pthread_create(&thread, NULL, handleRequest, user);
    }
}

void* Proxy::handleRequest(void* userInfo) {
    User* user = (User*)userInfo;
    // Cache* cache = new Cache(10);
    vector<char> message(MAX_SIZE, 0);
    int status = recv(user->getClientFd(), message.data(), message.size(), 0);
    if (status <= 0) {
        pthread_mutex_lock(&mutex);
        logDoc << user->getThreadId() << ": ERROR Invalid receive!" << endl;
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    std::string req = message.data();

    Request* request = new Request(req);
    pthread_mutex_lock(&mutex);
    logDoc << user->getThreadId() << ": \"" << request->getRequestLine() << "\" from " << user->getIp() << " @ " << getCurrentTime();
    pthread_mutex_unlock(&mutex);
    std::string tmp = request->getHost();
    const char* hostname = tmp.c_str();
    const char* port = request->getPort().c_str();
    Client* proxy_client = new Client(hostname, port);
    int server_fd = proxy_client->createClient();

    pthread_mutex_lock(&mutex);
    logDoc << user->getThreadId() << ": Requesting \"" << request->getRequestLine() << "\" from " << request->getHost() << std::endl;
    pthread_mutex_unlock(&mutex);

    if (request->getMethod() == "CONNECT") {
        handleConnect(user->getClientFd(), server_fd, user->getThreadId());
        pthread_mutex_lock(&mutex);
        logDoc << user->getClientFd() << ": Tunnel closed" << endl;
        pthread_mutex_unlock(&mutex);
    }
    else if (request->getMethod() == "GET") {
        handleGet(user->getClientFd(), server_fd, user->getThreadId(), message, hostname, cache);
    }
    else if (request->getMethod() == "POST") {
        handlePost(user->getClientFd(), server_fd, user->getThreadId(), message, hostname);
    }
    else {

    }
    close(user->getClientFd());
    delete proxy_client;
    return NULL;
}
void Proxy::handleConnect(int user_fd, int server_fd, int thread_id) {
    const char* msg_to_user = "HTTP/1.1 200 OK\r\n\r\n";
    send(user_fd, msg_to_user, sizeof(msg_to_user), 0);
    pthread_mutex_lock(&mutex);
    logDoc << thread_id << ": Responding \"HTTP/1.1 200 OK\"" << std::endl;
    pthread_mutex_unlock(&mutex);

    fd_set readfds;
    int nfds = max(user_fd, server_fd) + 1;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(user_fd, &readfds);
        FD_SET(server_fd, &readfds);

        select(nfds, &readfds, NULL, NULL, NULL);
        int tmp[2] = { user_fd, server_fd };
        for (int i = 0; i < 2; i++) {
            vector<char> message(MAX_SIZE, 0);
            if (FD_ISSET(tmp[i], &readfds)) {
                int len = recv(tmp[i], message.data(), message.size(), 0);
                if (len <= 0) return;
                else {
                    int len2 = send(tmp[i - i], message.data(), len, 0);
                    if (len2 <= 0) return;
                }
            }
        }
    }

}
void Proxy::handlePost(int user_fd, int server_fd, int thread_id, vector<char> message, const char* hostname) {
    send(server_fd, message.data(), message.size(), 0);
    vector<char> response(MAX_SIZE, 0);
    int len = recv(server_fd, response.data(), response.size(), 0);
    if (len > 0) {
        string res_str = response.data();
        Response res(res_str);
        pthread_mutex_lock(&mutex);
        logDoc << thread_id << ": Received \"" << res.getFirstLine() << "\" from " << hostname << endl;
        pthread_mutex_unlock(&mutex);
        send(user_fd, response.data(), response.size(), 0);

        pthread_mutex_lock(&mutex);
        logDoc << thread_id << ": Responding \"" << res.getFirstLine() << endl;
        pthread_mutex_unlock(&mutex);
    }
    else {
        std::cerr << "Error: cannot get response from server!" << std::endl;
    }
}

void Proxy::handleGet(int user_fd, int server_fd, int thread_id, vector<char> message, const char* hostname, Cache* cache) {
    std::string req = message.data();
    Request req_parse(req);
    std::string uri = req_parse.getUri();
    Response cache_res = cache->get(uri);

    // std::cout << "req uri:" << uri << std::endl << std::endl;
    // std::cout << "cache res:" << cache_res.getResponse() << std::endl << std::endl;
    // std::cout << "cache res2:" << cache->cache[uri].getResponse() << std::endl << std::endl;

    if (cache_res.getResponse() == "") {
        // not in cache
        pthread_mutex_lock(&mutex);
        logDoc << thread_id << ": not in cache" << endl;
        // send request to server
        pthread_mutex_unlock(&mutex);
        send(server_fd, message.data(), message.size(), 0);
        pthread_mutex_lock(&mutex);
        logDoc << thread_id << ": Requesting \"" << req_parse.getRequest() << "\" from " << hostname << std::endl;
        pthread_mutex_unlock(&mutex);
        vector<char> res(MAX_SIZE, 0);
        // get response from server
        int len = recv(server_fd, res.data(), res.size(), 0);
        if (len <= 0) {
            std::cerr << "Error: receive failed" << std::endl;
        }
        std::string recv_res_str = res.data();
        Response recv_res(recv_res_str);
        // check if it is cacheable
        // std::cout << "res cache control: no_store:" << recv_res.no_store << std::endl << std::endl;
        if (recv_res.no_store) {
            pthread_mutex_lock(&mutex);
            logDoc << thread_id << ": not cacheable because cache control is no store" << endl;
            pthread_mutex_unlock(&mutex);
        }
        else if (recv_res.private_cache) {
            pthread_mutex_lock(&mutex);
            logDoc << thread_id << ": not cacheable because cache control is private" << endl;
            pthread_mutex_unlock(&mutex);
        }
        else {
            // save into cache
            // std::cout << "req uri:" << req_parse.getUri() << std::endl << std::endl;
            pthread_mutex_lock(&mutex);
            cache->put(req_parse.getUri(), recv_res);
            pthread_mutex_unlock(&mutex);
            // std::cout << "cache res3:" << cache->cache[uri].getResponse() << std::endl << std::endl;
            if (recv_res.getExpires() != "") {
                pthread_mutex_lock(&mutex);
                logDoc << thread_id << ": cached, expires at " << recv_res.getExpires() << endl;
                pthread_mutex_unlock(&mutex);
            }
            else {
                pthread_mutex_lock(&mutex);
                logDoc << thread_id << ": cached, but requires re-validation" << endl;
                pthread_mutex_unlock(&mutex);
            }
        }
        // send response to user
        send(user_fd, res.data(), res.size(), 0);
        pthread_mutex_lock(&mutex);
        logDoc << thread_id << ": Responding \"" << recv_res.getResponse() << endl;
        pthread_mutex_unlock(&mutex);
    }
    else {
        // in cache
        // check if it is valid
        bool isValid = cache_res.checkStrongCaching(thread_id, logDoc);
        std::string cache_res_str = cache_res.getResponse();
        vector<char> cache_res_msg(cache_res_str.begin(), cache_res_str.end());
        if (isValid == true) {
            // if valid, send response in cache to user
            pthread_mutex_lock(&mutex);
            logDoc << thread_id << ": in cache, valid " << endl;
            pthread_mutex_unlock(&mutex);

            send(user_fd, cache_res_msg.data(), cache_res_msg.size(), 0);
            pthread_mutex_lock(&mutex);
            logDoc << thread_id << ": Responding \"" << cache_res.getFirstLine() << endl;
            pthread_mutex_unlock(&mutex);
        }
        else {
            // if not valid, it needs revalidation
            pthread_mutex_lock(&mutex);
            logDoc << thread_id << ": in cache, requires validation" << endl;
            pthread_mutex_unlock(&mutex);

            // std::cout << "req header: " << req_parse.getHeader() << std::endl << std::endl;
            cache->revalidate(cache_res, req_parse.getHeader(), server_fd);

            vector<char> res(MAX_SIZE, 0);
            // get response from server
            int len = recv(server_fd, res.data(), res.size(), 0);
            if (len <= 0) {
                std::cerr << "Error: receive failed" << std::endl;
            }
            std::string recv_res_str = res.data();
            Response recv_res(recv_res_str);
            pthread_mutex_lock(&mutex);
            logDoc << thread_id << ": Received \"" << recv_res.getFirstLine() << "\" from " << hostname << endl;
            pthread_mutex_unlock(&mutex);

            // check status code
            std::string status = recv_res.getStatus();
            if (status == "304") {
                // 304, send response in cache to user
                send(user_fd, cache_res_msg.data(), cache_res_msg.size(), 0);
                pthread_mutex_lock(&mutex);
                logDoc << thread_id << ": Responding \"" << cache_res.getFirstLine() << endl;
                pthread_mutex_unlock(&mutex);
            }
            else if (status == "200") {
                // 200, update cache, and send response to user

                // maybe wirte a new function? this part is same as previous part when res is not in cache
                if (recv_res.no_store) {
                    pthread_mutex_lock(&mutex);
                    logDoc << thread_id << ": not cacheable because cache control is no store" << endl;
                    pthread_mutex_unlock(&mutex);
                }
                else if (recv_res.private_cache) {
                    pthread_mutex_lock(&mutex);
                    logDoc << thread_id << ": not cacheable because cache control is private" << endl;
                    pthread_mutex_unlock(&mutex);
                }
                else {
                    // save into cache
                    pthread_mutex_lock(&mutex);
                    cache->put(req_parse.getUri(), recv_res);
                    pthread_mutex_unlock(&mutex);
                    if (recv_res.getExpires() != "") {
                        pthread_mutex_lock(&mutex);
                        logDoc << thread_id << ": cached, expires at " << recv_res.getExpires() << endl;
                        pthread_mutex_unlock(&mutex);
                    }
                    else {
                        pthread_mutex_lock(&mutex);
                        logDoc << thread_id << ": cached, but requires re-validation" << endl;
                        pthread_mutex_unlock(&mutex);
                    }
                }

                send(user_fd, res.data(), res.size(), 0);
                pthread_mutex_lock(&mutex);
                logDoc << thread_id << ": Responding \"" << recv_res.getFirstLine() << endl;
                pthread_mutex_unlock(&mutex);
            }
        }


    }
}
std::string Proxy::getCurrentTime() {
    std::time_t current_time = std::time(NULL);
    std::tm* time_info = std::gmtime(&current_time);
    return asctime(time_info);
}

