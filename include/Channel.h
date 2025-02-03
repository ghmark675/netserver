#ifndef CHANNEL_H
#define CHANNEL_H

#include <sys/epoll.h>
#include <sys/socket.h>

#include <cstring>
#include <functional>

#include "EventLoop.h"

class Socket;
class EventLoop;

class Channel {
 private:
  int fd_ = -1;
  EventLoop *loop_ = nullptr;
  bool inepoll_ = false;
  uint32_t events_ = 0;
  uint32_t revents_ = 0;
  std::function<void()> readcallback_;
  std::function<void()> closecallback_;
  std::function<void()> errorcallback_;

 public:
  Channel(EventLoop *loop, int fd);
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

  void onmessage();
  void set_readcallback(std::function<void()> fn);
  void set_closecallback(std::function<void()> fn);
  void set_errorcallback(std::function<void()> fn);
};

#endif