#include "proxy.hpp"
#include "user.hpp"
#include "request.hpp"
#include "response.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <fstream>
#include <ctime>

// std::ofstream logDoc("/var/log/erss/proxy.log");
std::ofstream logDoc("proxy.log");

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
    char message[65536] = { 0 };
    int status = recv(user->getClientFd(), message, sizeof(message), 0);
    if (status <= 0) {
        pthread_mutex_lock(&mutex);
        logDoc << user->getThreadId() << ": ERROR Invalid receive!" << endl;
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    std::string req = std::string(message, status);

    Request* request = new Request(req);
    pthread_mutex_lock(&mutex);
    logDoc << user->getThreadId() << ": \"" << request->getRequestLine() << "\" from " << user->getIp() << " @ " << getCurrentTime();
    pthread_mutex_unlock(&mutex);
    std::string tmp = request->getHost();
    const char* hostname = tmp.c_str();
    const char* port = request->getPort().c_str();
    Client* proxy_client = new Client(hostname, port);
    int server_fd = proxy_client->createClient();

    if (request->getMethod() == "CONNECT") {

    }
    else if (request->getMethod() == "GET") {

    }
    else if (request->getMethod() == "POST") {

    }
    else {

    }
    close(user->getClientFd());
    delete proxy_client;
    return NULL;
}

std::string Proxy::getCurrentTime() {
    std::time_t currentTime = std::time(NULL);
    std::tm* timeInfo = std::localtime(&currentTime);
    return asctime(timeInfo);
}
