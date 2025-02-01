#include "../include/Ctcpsocket.h"

#include <sys/socket.h>
#include <unistd.h>

bool Ctcpsocket::send(const int sockfd, const std::string &buffer) {
  if (sockfd == -1) return false;
  if ((::send(sockfd, buffer.data(), buffer.size(), 0)) <= 0) return false;
  return true;
}

bool Ctcpsocket::recv(const int sockfd, std::string &buffer,
                      const size_t maxlen) {
  buffer.clear();
  buffer.resize(maxlen);
  int readn = ::recv(sockfd, &buffer[0], buffer.size(), 0);
  if (readn <= 0) {
    buffer.clear();
    return false;
  }
  buffer.resize(readn);
  return true;
}
