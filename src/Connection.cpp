#include "../include/Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock)
    : loop_(loop), clientsock_(clientsock) {
  clientchannel_ = new Channel(loop_, clientsock->fd());
  clientchannel_->set_readcallback(
      std::bind(&Channel::onmessage, clientchannel_));
  clientchannel_->set_closecallback(
      std::bind(&Connection::close_callback, this));
  clientchannel_->set_errorcallback(
      std::bind(&Connection::error_callback, this));
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

void Connection::close_callback() {
  std::cout << "disconnected: " << fd() << std::endl;
  ::close(fd());
}
void Connection::error_callback() {
  std::cerr << "error " << fd() << '\n';
  ::close(fd());
}