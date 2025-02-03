#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "Epoll.h"

class EventLoop {
 private:
  Epoll *ep_;

 public:
  EventLoop();
  ~EventLoop();

  void run();
  Epoll *ep();
  void update_channel(Channel *ch);
};

#endif
