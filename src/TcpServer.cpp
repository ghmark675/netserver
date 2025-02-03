#include "../include/TcpServer.h"

TcpServer::TcpServer(const uint16_t port) {
  acceptor_ = new Acceptor(&loop_, port);
  acceptor_->set_newconnectioncb(
      std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
}
TcpServer::~TcpServer() {
  delete acceptor_;
  for (auto &aa : conns_) {
    delete aa.second;
  }
}

void TcpServer::start() { loop_.run(); }

void TcpServer::newconnection(Socket *clientsock) {
  Connection *conn = new Connection(&loop_, clientsock);

  std::cout << "accept client(fd=" << conn->fd() << ",ip=" << conn->ip().c_str()
            << ",port=" << conn->port() << ")" << std::endl;

  conns_[conn->fd()] = conn;
}