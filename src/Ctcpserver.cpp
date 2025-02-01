#include "../include/Ctcpserver.h"

#include <arpa/inet.h>

Ctcpserver::Ctcpserver() : serverfd(-1) {}

bool Ctcpserver::init_server(const unsigned short _port) {
  if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) return false;
  port = _port;

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (::bind(serverfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
    ::close(serverfd);
    serverfd = -1;
    return false;
  }

  if (::listen(serverfd, 5) == -1) {
    ::close(serverfd);
    serverfd = -1;
    return false;
  }

  return true;
}

bool Ctcpserver::accept() {
  struct sockaddr_in caddr;
  socklen_t addrlen = sizeof(caddr);
  if ((sock = ::accept(serverfd, (struct sockaddr*)&caddr, &addrlen)) == -1)
    return false;
  ip = inet_ntoa(caddr.sin_addr);
  return true;
}
const std::string& Ctcpserver::client_ip() const { return ip; }

bool Ctcpserver::close_client() {
  if (sock == -1) return false;

  ::close(sock);
  sock = -1;
  return true;
}

bool Ctcpserver::close_listen() {
  if (serverfd == -1) return false;

  ::close(serverfd);
  serverfd = -1;
  return true;
}

Ctcpserver::~Ctcpserver() {
  close_client();
  close_listen();
}