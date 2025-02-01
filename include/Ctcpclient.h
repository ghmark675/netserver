#ifndef C_TCP_CLIENT_H
#define C_TCP_CLIENT_H

#include <string>

#include "Ctcpsocket.h"

class Ctcpclient : public Ctcpsocket {
 public:
  bool connect(const std::string &_ip, const unsigned short _port);
};

#endif
