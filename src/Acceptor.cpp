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
  acceptchannel_->set_readcallback(
      std::bind(&Channel::newconnection, acceptchannel_, servsock_));
  acceptchannel_->enable_reading();
}

Acceptor::~Acceptor() {
  delete servsock_;
  delete acceptchannel_;
}