#include "../include/Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, uint16_t port) : loop_(loop) {
  servsock_ = new Socket(createnonblocking());
  InetAddress servaddr(port);
  servsock_->setreuseport(true);
  servsock_->settcpnodelay(true);
  servsock_->setkeepalive(true);
  servsock_->setreuseport(true);
  servsock_->bind(servaddr);
  servsock_->listen();

  acceptchannel_ = new Channel(loop_, servsock_->fd());
  acceptchannel_->set_readcallback(std::bind(&Acceptor::newconnection, this));
  acceptchannel_->enable_reading();
}

Acceptor::~Acceptor() {
  delete servsock_;
  delete acceptchannel_;
}

void Acceptor::newconnection() {
  InetAddress clientaddr;
  Socket *clientsock = new Socket(servsock_->accept(clientaddr));
  newconnectioncb_(clientsock);
}

void Acceptor::set_newconnectioncb(std::function<void(Socket *)> fn) {
  newconnectioncb_ = fn;
}