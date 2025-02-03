#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
class Acceptor {
 private:
  EventLoop *loop_;
  Socket *servsock_;
  Channel *acceptchannel_;

 public:
  Acceptor(EventLoop *loop, uint16_t port);
  ~Acceptor();
};

#endif