#ifndef EPOLL_H
#define EPOLL_H

#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

class Channel;

class Epoll {
 private:
  static constexpr int MAXEVENTS = 100;
  int epollfd_ = -1;
  epoll_event events_[MAXEVENTS];

 public:
  Epoll();
  ~Epoll();

  // void addfd(int fd, uint32_t op);
  void update_channel(Channel *ch);
  std::vector<Channel *> loop(int timeout = -1);
};

#endif