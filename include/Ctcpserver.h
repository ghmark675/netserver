#ifndef C_TCP_SERVER_H
#define C_TCP_SERVER_H

#include <sys/select.h>

#include "Ctcpsocket.h"

class Ctcpserver : public Ctcpsocket {
 public:
  Ctcpserver();
  bool init_server(const unsigned short _port);
  int accept();
  bool close_listen();
  bool select();
  ~Ctcpserver();

 protected:
  int listenfd, maxfd;
  unsigned short port;
  fd_set readfds;
};

#endif