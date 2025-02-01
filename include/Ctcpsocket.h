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
  Ctcpsocket();
  bool send(const std::string &buffer);
  bool recv(std::string &buffer, const size_t maxlen);
  bool close();
  ~Ctcpsocket();

 protected:
  int sock;
  unsigned short port;
  std::string ip;
};

#endif