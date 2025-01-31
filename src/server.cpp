#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

int main(int argc, char *argv[]) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  if (argc != 2) {
    std::cerr << "./server 5005\n";
    return -1;
  }

  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd == -1) {
    std::cerr << "listenfd error\n";
    return -1;
  }

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[1]));
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    std::cerr << "listenfd bind error\n";
    close(listenfd);
    return -1;
  }

  if (listen(listenfd, 5) == -1) {
    std::cerr << "listen error\n";
    close(listenfd);
    return -1;
  }

  int clientfd = accept(listenfd, 0, 0);
  if (clientfd == -1) {
    std::cerr << "clientfd socket error\n";
    close(listenfd);
    return -1;
  }

  std::cout << "client is connected" << std::endl;

  char buffer[1024];
  while (true) {
    int iret;
    memset(buffer, 0, sizeof(buffer));
    if ((iret = recv(clientfd, buffer, sizeof(buffer), 0)) <= 0) {
      std::cerr << "recv iret=" << iret << '\n';
      break;
    }

    std::cout << "recv: " << buffer << std::endl;
    strcpy(buffer, "ok");
    if ((iret = send(clientfd, buffer, strlen(buffer), 0)) <= 0) {
      std::cerr << "send iret=" << iret << '\n';
      break;
    }
    std::cout << "send: " << buffer << std::endl;
  }

  close(listenfd);
  close(clientfd);

  return 0;
}