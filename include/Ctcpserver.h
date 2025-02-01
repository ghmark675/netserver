#ifndef C_TCP_SERVER_H
#define C_TCP_SERVER_H

#include "Ctcpsocket.h"
class Ctcpserver : public Ctcpsocket {
 public:
  Ctcpserver();
  bool init_server(const unsigned short _port);
  bool accept();
  const std::string& client_ip() const;
  bool close_client();
  bool close_listen();
  ~Ctcpserver();

 protected:
  int serverfd;
};

#endif