#ifndef C_TCP_SOCKET_H
#define C_TCP_SOCKET_H

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <string>

class Ctcpsocket {
 public:
  bool send(const int sockfd, const std::string &buffer);
  bool recv(const int sockfd, std::string &buffer, const size_t maxlen);
};

#endif