#include "../include/TcpServer.h"

TcpServer::TcpServer(const uint16_t port) {
  acceptor_ = new Acceptor(&loop_, port);
}
TcpServer::~TcpServer() { delete acceptor_; }

void TcpServer::start() { loop_.run(); }