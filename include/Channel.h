#ifndef CHANNEL_H
#define CHANNEL_H

#include <sys/epoll.h>

class Epoll;
class Socket;

class Channel {
 private:
  int fd_ = -1;
  Epoll *ep_ = nullptr;
  bool inepoll_ = false;
  uint32_t events_ = 0;
  uint32_t revents_ = 0;
  bool is_listen_ = false;

 public:
  Channel(Epoll *ep, int fd, bool is_listen);
  ~Channel();

  int fd();
  void useet();
  void enable_reading();
  void set_inepoll();
  void set_revents(uint32_t ev);
  bool inpoll();
  uint32_t events();
  uint32_t revents();

  void handle_event(Socket *servsock);
};

#endif