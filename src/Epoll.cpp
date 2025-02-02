#include "../include/Epoll.h"

Epoll::Epoll() {
  if ((epollfd_ = epoll_create(1)) == -1) {
    printf("epoll_create() failed(%d).\n", errno);
    exit(-1);
  }
}
Epoll::~Epoll() { ::close(epollfd_); }

void Epoll::addfd(int fd, uint32_t op) {
  epoll_event ev;
  ev.data.fd = fd;
  ev.events = op;
  if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("epoll_ctl() failed(%d)\n", errno);
    exit(-1);
  }
}
std::vector<epoll_event> Epoll::loop(int timeout) {
  std::vector<epoll_event> evs;
  bzero(events_, sizeof(events_));
  int fds = epoll_wait(epollfd_, events_, MAXEVENTS, timeout);
  if (fds < 0) {
    perror("epoll_wait() failed");
    exit(-1);
  }
  if (fds == 0) {
    perror("epoll_wait() timeout");
    return evs;
  }

  for (int i = 0; i < fds; i++) {
    evs.push_back(events_[i]);
  }

  return evs;
}