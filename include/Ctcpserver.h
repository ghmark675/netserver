#ifndef C_TCP_SERVER_H
#define C_TCP_SERVER_H

#include <sys/epoll.h>
#include <sys/select.h>

#include "Ctcpsocket.h"

constexpr static int MAX_EPOLL_EVENT_NUM = 10;

class Ctcpserver : public Ctcpsocket {
 public:
  Ctcpserver();
  bool init_server(const unsigned short _port);
  int accept();
  bool close_listen();
  bool select();
  bool epoll();
  ~Ctcpserver();

 protected:
  int listenfd, maxfd, epollfd;
  unsigned short port;
  fd_set readfds;
  epoll_event ev, evs[MAX_EPOLL_EVENT_NUM];
};

#endif