#include <sys/epoll.h>

#include <iostream>

#include "../include/Ctcpclient.h"
#include "../include/Epoll.h"
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

  Epoll ep;
  ep.addfd(servsock.fd(), EPOLLIN);
  std::vector<epoll_event> evs;

  while (true) {
    evs = ep.loop();
    for (auto &ev : evs) {
      if (ev.events & EPOLLRDHUP) {
        std::cout << "disconnected: " << ev.data.fd << std::endl;
        ::close(ev.data.fd);
        continue;
      }
      if (ev.events & (EPOLLIN | EPOLLPRI)) {  // 接收缓冲区有数据可以读
        if (ev.data.fd == servsock.fd()) {
          InetAddress clientaddr;
          Socket *clientsock = new Socket(servsock.accept(clientaddr));
          std::cout << "accept client(fd=" << clientsock->fd()
                    << ",ip=" << clientaddr.ip()
                    << ",port=" << clientaddr.port() << ")" << std::endl;
          ep.addfd(clientsock->fd(), EPOLLIN | EPOLLET);
        } else {
          char buffer[1024];
          while (true) {
            bzero(&buffer, sizeof(buffer));
            ssize_t nread = read(ev.data.fd, buffer, sizeof(buffer));
            if (nread > 0) {
              std::cout << "recv from " << ev.data.fd << ": " << buffer
                        << std::endl;
              ::send(ev.data.fd, buffer, strlen(buffer), 0);
              continue;
            }
            if (nread == -1 && errno == EINTR) continue;
            if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
              break;
            if (nread == 0) {
              std::cout << "disconnected: " << ev.data.fd << std::endl;
              break;
            }
          }
        }
        continue;
      }
      if (ev.events & EPOLLOUT) continue;
      std::cerr << "error " << ev.data.fd << '\n';
      ::close(ev.data.fd);
    }
  }
}