#include "../include/TcpServer.h"

TcpServer::TcpServer(const uint16_t port) {
  Socket *servsock = new Socket(createnonblocking());
  InetAddress servaddr(port);
  servsock->setreuseport(true);
  servsock->settcpnodelay(true);
  servsock->setkeepalive(true);
  servsock->setreuseport(true);
  servsock->bind(servaddr);
  servsock->listen();

  Channel *servchannel = new Channel(&loop_, servsock->fd());
  servchannel->set_readcallback(
      std::bind(&Channel::newconnection, servchannel, servsock));
  servchannel->enable_reading();
}
TcpServer::~TcpServer() {}

void TcpServer::start() { loop_.run(); }