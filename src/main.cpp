#include <sys/epoll.h>

#include <iostream>

#include "../include/Ctcpclient.h"
#include "../include/Socket.h"

void print_tutorial() {
  std::cerr << "client: ./main -c 127.0.0.1 5005\n";
  std::cerr << "server: ./main -s <ip_addr> <port>\n";
}

void client(const std::string &ip, const unsigned short port);
void server(const unsigned short port);

int main(int argc, char *argv[]) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::string opt(argv[1]);
  if (opt == "-c") {
    // client
    if (argc != 4) {
      print_tutorial();
      return -1;
    }
    client(argv[2], atoi(argv[3]));
  } else if (opt == "-s") {
    // server
    if (argc != 3) {
      print_tutorial();
      return -1;
    }
    server(atoi(argv[2]));
  } else {
    print_tutorial();
    return -1;
  }

  return 0;
}

void client(const std::string &ip, const unsigned short port) {
  Ctcpclient client;
  if (!client.connect(ip, port)) {
    std::cerr << "connect error()\n";
    return;
  }
  std::cout << "connect ok!" << std::endl;

  std::string buffer;
  for (int i = 0; i < 200000; i++) {
    std::cout << "input: ";
    std::cout.flush();
    std::cin >> buffer;
    if (!client.send(buffer)) {
      std::cerr << "send() failed\n";
      return;
    }
    if (!client.recv(buffer, 1024)) {
      std::cerr << "recv() failed\n";
      return;
    }
    std::cout << "recv: " << buffer << std::endl;
  }
}

void server(const unsigned short port) {
  Socket servsock(createnonblocking());
  InetAddress servaddr(port);
  servsock.setreuseport(true);
  servsock.settcpnodelay(true);
  servsock.setkeepalive(true);
  servsock.setreuseport(true);
  servsock.bind(servaddr);
  servsock.listen();

  int epollfd = epoll_create(1);

  epoll_event ev;
  ev.data.fd = servsock.fd();
  ev.events = EPOLLIN;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, servsock.fd(), &ev);
  epoll_event evs[10];

  while (true) {
    int fds = epoll_wait(epollfd, evs, 10, -1);
    if (fds < 0) {
      perror("epoll_wait() failed");
      break;
    }
    if (fds == 0) {
      perror("epoll_wait() timeout");
      continue;
    }

    for (int i = 0; i < fds; i++) {
      if (evs[i].events & EPOLLRDHUP) {
        std::cout << "disconnected: " << evs[i].data.fd << std::endl;
        ::close(evs[i].data.fd);
        continue;
      }
      if (evs[i].events & (EPOLLIN | EPOLLPRI)) {  // 接收缓冲区有数据可以读
        if (evs[i].data.fd == servsock.fd()) {
          InetAddress clientaddr;
          Socket *clientsock = new Socket(servsock.accept(clientaddr));
          std::cout << "accept client(fd=" << clientsock->fd()
                    << ",ip=" << clientaddr.ip()
                    << ",port=" << clientaddr.port() << ")" << std::endl;
          ev.data.fd = clientsock->fd();
          ev.events = EPOLLIN | EPOLLET;
          epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock->fd(), &ev);
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
  }
}