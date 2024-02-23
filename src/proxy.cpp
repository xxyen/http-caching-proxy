#include "proxy.hpp"
#include <iostream>
#include <cstring>
#include <pthread.h>
std::ofstream logDoc("/var/log/erss/proxy.log");

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

Proxy::Proxy(char* port) : port(port) {}

Proxy::~Proxy() {
    // If threads are joinable or need to be cleaned up, do it here
    // pthread_join or pthread_detach can be used depending on the use case
}

void Proxy::start() {
    Server server(port);
    int server_fd = server.createServer();
    if (server_fd < 0) {
        pthread_mutex_lock(&mutex);
        logFile << "(no-id): ERROR in creating socket!" << endl;
        pthread_mutex_unlock(&mutex);
        return;
    }
    int thread_id = 0;
    while (true) {
        std::string ip;
        int client_fd = server.acceptClient(&ip);
        if (client_fd < 0) {
            pthread_mutex_lock(&mutex);
            logFile << "(no-id): ERROR Failed to accept client connection." << endl;
            pthread_mutex_unlock(&mutex);
            continue;
        }

        pthread_t thread;

        pthread_mutex_lock(&mutex);

        thread_id++;
        pthread_mutex_unlock(&mutex);

        pthread_create(&thread, NULL, Proxy::handleRequest, client);
    }
}

void* Proxy::handleRequest(void* client) {

}
