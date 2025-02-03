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

int Connection::fd() const { return clientsock_->fd(); }

std::string Connection::ip() const { return clientsock_->ip(); }

uint16_t Connection::port() const { return clientsock_->port(); }