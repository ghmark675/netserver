#include "../include/Ctcpserver.h"

#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <cerrno>
#include <iomanip>
#include <iostream>

Ctcpserver::Ctcpserver() : listenfd(-1) { FD_ZERO(&readfds); }

bool Ctcpserver::init_server(const unsigned short _port) {
  if ((listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1)
    return false;
  port = _port;
  int opt = 1;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt,
             static_cast<socklen_t>(sizeof opt));  // 必须的。
  setsockopt(listenfd, SOL_SOCKET, TCP_NODELAY, &opt,
             static_cast<socklen_t>(sizeof opt));  // 必须的。
  setsockopt(
      listenfd, SOL_SOCKET, SO_REUSEPORT, &opt,
      static_cast<socklen_t>(sizeof opt));  // 有用，但是，在Reactor中意义不大。
  setsockopt(
      listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt,
      static_cast<socklen_t>(sizeof opt));  // 可能有用，但是，建议自己做心跳。

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (::bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    ::close(listenfd);
    listenfd = -1;
    return false;
  }

  if (::listen(listenfd, 5) == -1) {
    ::close(listenfd);
    listenfd = -1;
    return false;
  }
  // for select
  FD_SET(listenfd, &readfds);
  maxfd = listenfd;

  // for epoll
  epollfd = epoll_create(1);
  ev.data.fd = listenfd;
  ev.events = EPOLLIN;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

  return true;
}

int Ctcpserver::accept() {
  sockaddr_in caddr;
  socklen_t addrlen = sizeof(caddr);
  return ::accept4(listenfd, (struct sockaddr *)&caddr, &addrlen,
                   SOCK_NONBLOCK);
}

bool Ctcpserver::close_listen() {
  if (listenfd == -1) return false;

  ::close(listenfd);
  listenfd = -1;
  return true;
}

void Ctcpserver::print_info(const std::string &pre, int fd,
                            const std::string &suf) {
  sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  int res = getpeername(fd, (sockaddr *)&addr, &addr_len);
  std::string client_info =
      "client(fd=" + std::to_string(fd) + ",ip=" + inet_ntoa(addr.sin_addr) +
      ",port=" + std::to_string(ntohs(addr.sin_port)) + ") ";
  if (res == 0) {
    std::cout << std::left << std::setw(15) << pre << std::left << std::setw(45)
              << client_info << std::left << std::setw(10) << suf << std::endl;
  } else {
    std::cerr << "in print_info(), getpeername() failed\n";
  }
}

bool Ctcpserver::select() {
  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  fd_set tmpfds = readfds;
  int fds = ::select(maxfd + 1, &tmpfds, 0, 0, 0);
  if (fds < 0) {
    std::cerr << "select() failed\n";
    return false;
  }
  if (fds == 0) {
    std::cerr << "select() timeout\n";
    return false;
  }

  std::string buffer;
  for (int eventfd = 0; eventfd <= maxfd; eventfd++) {
    if (FD_ISSET(eventfd, &tmpfds) == 0) continue;
    if (eventfd == listenfd) {
      int clientsock = accept();
      if (clientsock == -1) {
        std::cerr << "accept() failed\n";
        continue;
      }
      print_info("accept", eventfd);
      FD_SET(clientsock, &readfds);
      if (maxfd < clientsock) maxfd = clientsock;
      continue;
    }
    if (!recv(eventfd, buffer, 1024)) {
      print_info("disconnected", eventfd);
      ::close(eventfd);
      FD_CLR(eventfd, &readfds);
      if (eventfd == maxfd) {
        for (int i = maxfd; i > 0; i--) {
          if (FD_ISSET(i, &readfds)) {
            maxfd = i;
            break;
          }
        }
      }
      continue;
    }
    print_info("recv", eventfd, buffer);
    buffer = "ok";
    send(eventfd, buffer);
  }
  return true;
}

bool Ctcpserver::epoll() {
  int fds = epoll_wait(epollfd, evs, MAX_EPOLL_EVENT_NUM, -1);
  if (fds < 0) {
    std::cerr << "epoll_wait() failed\n";
    return false;
  }
  if (fds == 0) {
    std::cerr << "epoll_wait() timeout\n";
    return false;
  }

  for (int i = 0; i < fds; i++) {
    if (evs[i].events & EPOLLRDHUP) {
      print_info("disconnected", evs[i].data.fd);
      ::close(evs[i].data.fd);
      continue;
    }
    if (evs[i].events & (EPOLLIN | EPOLLPRI)) {  // 接收缓冲区有数据可以读
      if (evs[i].data.fd == listenfd) {
        int clientfd = accept();
        if (clientfd < 0) {
          std::cerr << "accept() failed\n";
          continue;
        }
        print_info("accept", clientfd);
        ev.data.fd = clientfd;
        ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
      } else {
        char buffer[1024];
        while (true) {
          bzero(&buffer, sizeof(buffer));
          ssize_t nread = read(evs[i].data.fd, buffer, sizeof(buffer));
          if (nread > 0) {
            print_info("recv", evs[i].data.fd, buffer);
            ::send(evs[i].data.fd, buffer, strlen(buffer), 0);
            continue;
          }
          if (nread == -1 && errno == EINTR) continue;
          if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
            break;
          if (nread == 0) {
            print_info("disconnected", evs[i].data.fd);
            break;
          }
        }
      }
      continue;
    }
    if (evs[i].events & EPOLLOUT) continue;
    print_info("error", evs[i].data.fd);
    ::close(evs[i].data.fd);
  }
  return true;
}

Ctcpserver::~Ctcpserver() { close_listen(); }