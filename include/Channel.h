#ifndef CHANNEL_H
#define CHANNEL_H

#include <sys/epoll.h>

#include <functional>

class Epoll;
class Socket;

class Channel {
 private:
  int fd_ = -1;
  Epoll *ep_ = nullptr;
  bool inepoll_ = false;
  uint32_t events_ = 0;
  uint32_t revents_ = 0;
  std::function<void()> readcallback_;

 public:
  Channel(Epoll *ep, int fd);
  ~Channel();

  int fd();
  void useet();
  void enable_reading();
  void set_inepoll();
  void set_revents(uint32_t ev);
  bool inpoll();
  uint32_t events();
  uint32_t revents();

  void handle_event();

  void newconnection(Socket *servsock);
  void onmessage();
  void set_readcallback(std::function<void()> fn);
};

#endif