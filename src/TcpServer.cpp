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
  conn->set_closecallback(
      std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
  conn->set_errorcallback(
      std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));

  std::cout << "accept client(fd=" << conn->fd() << ",ip=" << conn->ip().c_str()
            << ",port=" << conn->port() << ")" << std::endl;

  conns_[conn->fd()] = conn;
}

void TcpServer::closeconnection(Connection *conn) {
  std::cout << "disconnected: " << conn->fd() << std::endl;
  conns_.erase(conn->fd());
  delete conn;
}

void TcpServer::errorconnection(Connection *conn) {
  std::cerr << "error " << conn->fd() << '\n';
  conns_.erase(conn->fd());
  delete conn;
}