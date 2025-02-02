#include <iostream>

#include "../include/Ctcpclient.h"
#include "../include/Ctcpserver.h"

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
  Ctcpserver server;
  if (!server.init_server(port)) {
    std::cerr << "init_server() error\n";
    return;
  }

  std::cout << "server init done!" << std::endl;

  while (true) {
    server.epoll();
  }
}