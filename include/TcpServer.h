#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <map>

#include "Acceptor.h"
#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"

class TcpServer {
 private:
  EventLoop loop_;
  Acceptor *acceptor_;
  std::map<int, Connection *> conns_;

 public:
  TcpServer(const uint16_t port);
  ~TcpServer();

  void start();
  void newconnection(Socket *clientsock);
  void closeconnection(Connection *conn);
  void errorconnection(Connection *conn);
};

#endif