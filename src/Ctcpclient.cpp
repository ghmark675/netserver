#include "../include/Ctcpclient.h"

#include <arpa/inet.h>

#include <iostream>

Ctcpclient::Ctcpclient() : sock(-1) {}

bool Ctcpclient::send(const std::string &buffer) {
  return Ctcpsocket::send(sock, buffer);
}

bool Ctcpclient::recv(std::string &buffer, const size_t maxlen) {
  return Ctcpsocket::recv(sock, buffer, maxlen);
}

bool Ctcpclient::connect(const std::string &_ip, const unsigned short _port) {
  if (sock != -1) return false;
  port = _port;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) return false;
  std::cerr << "socket ok!\n";

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = inet_addr(_ip.c_str());

  if (::connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    ::close(sock);
    sock = -1;
    return false;
  }
  std::cerr << "connect ok!\n";

  return true;
}

Ctcpclient::~Ctcpclient() {
  if (sock != -1) close(sock);
}