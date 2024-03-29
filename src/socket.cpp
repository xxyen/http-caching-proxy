#include "socket.hpp"

int Server::createServer() {
  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host --server" << endl;
    return -1;
  }

  socket_fd = socket(host_info_list->ai_family,
    host_info_list->ai_socktype,
    host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    return -1;
  }

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    return -1;
  }

  status = listen(socket_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl;
    return -1;
  }
  return socket_fd;
}

int Server::acceptClient(std::string* ip) {
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_connection_fd;
  client_connection_fd = accept(socket_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);
  if (client_connection_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    return -1;
  }
  struct sockaddr_in* addr = (struct sockaddr_in*)&socket_addr;
  struct in_addr addrin = addr->sin_addr;
  char* client_ip = inet_ntoa(addrin);
  *ip = client_ip;
  return client_connection_fd;
}

int Client::createClient() {
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  cerr << "before:  hostname:" << hostname << endl;
  cerr << "before:  port:" << port << endl;
  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host --client " << gai_strerror(status) << endl;
    cerr << "hostname:" << hostname << endl;
    cerr << "port:" << port << endl;
    return -1;
  }

  socket_fd = socket(host_info_list->ai_family,
    host_info_list->ai_socktype,
    host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    return -1;
  }

  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    return -1;
  }
  cout << "success connect!" << endl;
  return socket_fd;
}