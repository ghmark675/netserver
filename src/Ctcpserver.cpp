#include "../include/Ctcpserver.h"

#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <cerrno>
#include <iostream>

#include "../include/InetAddress.h"

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

  // struct sockaddr_in servaddr;
  // memset(&servaddr, 0, sizeof(servaddr));
  // servaddr.sin_family = AF_INET;
  // servaddr.sin_port = htons(port);
  // servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  InetAddress servaddr(port);

  if (::bind(listenfd, servaddr.addr(), sizeof(sockaddr)) == -1) {
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
      std::cout << "accept: " << eventfd << std::endl;
      FD_SET(clientsock, &readfds);
      if (maxfd < clientsock) maxfd = clientsock;
      continue;
    }
    if (!recv(eventfd, buffer, 1024)) {
      std::cout << "disconnected: " << eventfd << std::endl;
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
    std::cout << "recv from " << eventfd << ": " << buffer << std::endl;
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
      std::cout << "disconnected: " << evs[i].data.fd << std::endl;
      ::close(evs[i].data.fd);
      continue;
    }
    if (evs[i].events & (EPOLLIN | EPOLLPRI)) {  // 接收缓冲区有数据可以读
      if (evs[i].data.fd == listenfd) {
        sockaddr_in peeraddr;
        socklen_t len = sizeof(peeraddr);
        int clientfd =
            ::accept4(listenfd, (sockaddr *)&peeraddr, &len, SOCK_NONBLOCK);
        InetAddress clientaddr(peeraddr);
        if (clientfd < 0) {
          std::cerr << "accept() failed\n";
          continue;
        }
        std::cout << "accept client(fd=" << clientfd
                  << ",ip=" << clientaddr.ip() << ",port=" << clientaddr.port()
                  << ")" << std::endl;
        ev.data.fd = clientfd;
        ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
      } else {
        char buffer[1024];
        while (true) {
          bzero(&buffer, sizeof(buffer));
          ssize_t nread = read(evs[i].data.fd, buffer, sizeof(buffer));
          if (nread > 0) {
            std::cout << "recv from " << evs[i].data.fd << ": " << buffer
                      << std::endl;
            ::send(evs[i].data.fd, buffer, strlen(buffer), 0);
            continue;
          }
          if (nread == -1 && errno == EINTR) continue;
          if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
            break;
          if (nread == 0) {
            std::cout << "disconnected: " << evs[i].data.fd << std::endl;
            break;
          }
        }
      }
      continue;
    }
    if (evs[i].events & EPOLLOUT) continue;
    std::cerr << "error " << evs[i].data.fd << '\n';
    ::close(evs[i].data.fd);
  }
  return true;
}

Ctcpserver::~Ctcpserver() { close_listen(); }