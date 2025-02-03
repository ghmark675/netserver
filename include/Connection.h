#ifndef CONNECTION_H
#define CONNECTION_H

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
class Connection {
 private:
  EventLoop *loop_;
  Socket *clientsock_;
  Channel *clientchannel_;

 public:
  Connection(EventLoop *loop, Socket *clientsock);
  ~Connection();

  int fd() const;
  std::string ip() const;
  uint16_t port() const;

  void close_callback();
  void error_callback();
};

#endif