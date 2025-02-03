#include "../include/Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock)
    : loop_(loop), clientsock_(clientsock) {
  clientchannel_ = new Channel(loop_, clientsock->fd());
  clientchannel_->set_readcallback(
      std::bind(&Channel::onmessage, clientchannel_));
  clientchannel_->useet();
  clientchannel_->enable_reading();
}

Connection::~Connection() {
  delete clientchannel_;
  delete clientsock_;
}