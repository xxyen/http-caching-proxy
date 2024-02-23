#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

class Server {
private:
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = NULL;
  const char *port;
public:
  Server(const char *_port) {
    port = _port;
  }
  void createServer();
  int acceptClient();
  ~Server() {
    freeaddrinfo(host_info_list);
    close(socket_fd);
  };
};

class Client {
private:
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname;
  const char *port;
public:
  Client(const char *_hostname, const char *_port){
    hostname = _hostname;
    port = _port;
  }
  void createClient();
  ~Client() {
    freeaddrinfo(host_info_list);
    close(socket_fd);
  };
};
