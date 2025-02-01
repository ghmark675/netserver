#include "../include/Ctcpsocket.h"

#include <sys/socket.h>
#include <unistd.h>

Ctcpsocket::Ctcpsocket() : sock(-1) {}

bool Ctcpsocket::send(const std::string &buffer) {
  if (sock == -1) return false;
  if ((::send(sock, buffer.data(), buffer.size(), 0)) <= 0) return false;
  return true;
}

bool Ctcpsocket::recv(std::string &buffer, const size_t maxlen) {
  buffer.clear();
  buffer.resize(maxlen);
  int readn = ::recv(sock, &buffer[0], buffer.size(), 0);
  if (readn <= 0) {
    buffer.clear();
    return false;
  }
  buffer.resize(readn);
  return true;
}

bool Ctcpsocket::close() {
  if (sock == -1) return false;
  ::close(sock);
  sock = -1;
  return true;
}

Ctcpsocket::~Ctcpsocket() { close(); }