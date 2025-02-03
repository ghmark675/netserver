#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "Acceptor.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

class TcpServer {
 private:
  EventLoop loop_;
  Acceptor *acceptor_;

 public:
  TcpServer(const uint16_t port);
  ~TcpServer();

  void start();
};

#endif