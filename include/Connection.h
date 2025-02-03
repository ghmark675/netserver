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
  std::function<void(Connection *)> closecallback_;
  std::function<void(Connection *)> errorcallback_;

 public:
  Connection(EventLoop *loop, Socket *clientsock);
  ~Connection();

  int fd() const;
  std::string ip() const;
  uint16_t port() const;

  void close_callback();
  void error_callback();

  void set_closecallback(std::function<void(Connection *)> fn);
  void set_errorcallback(std::function<void(Connection *)> fn);
};

#endif