#include "../include/Ctcpserver.h"

#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>

#include <iostream>

Ctcpserver::Ctcpserver() : listenfd(-1) { FD_ZERO(&readfds); }

bool Ctcpserver::init_server(const unsigned short _port) {
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) return false;
  port = _port;

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (::bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
    ::close(listenfd);
    listenfd = -1;
    return false;
  }

  if (::listen(listenfd, 5) == -1) {
    ::close(listenfd);
    listenfd = -1;
    return false;
  }

  FD_SET(listenfd, &readfds);
  maxfd = listenfd;

  return true;
}

int Ctcpserver::accept() {
  struct sockaddr_in caddr;
  socklen_t addrlen = sizeof(caddr);
  return ::accept(listenfd, (struct sockaddr*)&caddr, &addrlen);
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
      std::cout << "accept client: " << eventfd << std::endl;
      FD_SET(clientsock, &readfds);
      if (maxfd < clientsock) maxfd = clientsock;
      continue;
    }
    if (!recv(eventfd, buffer, 1024)) {
      std::cout << "client(eventfd=" << eventfd << ")disconnected" << std::endl;
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
    std::cout << "recv(eventfd=" << eventfd << "): " << buffer << std::endl;
    buffer = "ok";
    send(eventfd, buffer);
  }
  return true;
}

Ctcpserver::~Ctcpserver() { close_listen(); }