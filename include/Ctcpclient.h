#ifndef C_TCP_CLIENT_H
#define C_TCP_CLIENT_H

#include <string>

#include "Ctcpsocket.h"

class Ctcpclient : public Ctcpsocket {
 public:
  Ctcpclient();
  bool connect(const std::string &_ip, const unsigned short _port);
  bool send(const std::string &buffer);
  bool recv(std::string &buffer, const size_t maxlen);
  ~Ctcpclient();

 private:
  int sock;
  unsigned short port;
};

#endif
