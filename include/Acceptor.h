#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>

#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"

class Connection;

class Acceptor {
 private:
  EventLoop *loop_;
  Socket *servsock_;
  Channel *acceptchannel_;
  std::function<void(Socket *)> newconnectioncb_;

 public:
  Acceptor(EventLoop *loop, uint16_t port);
  ~Acceptor();
  void newconnection();
  void set_newconnectioncb(std::function<void(Socket *)> fn);
};

#endif