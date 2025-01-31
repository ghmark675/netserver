#include <arpa/inet.h>
#include <debug.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

class Ctcpclient {
 public:
};

int main(int argc, char *argv[]) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  if (argc != 3) {
    std::cerr << "./client 127.0.0.1 5005\n";
    return -1;
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    std::cerr << "client create socket failed\n";
    return -1;
  }

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));

  struct hostent *h;
  if ((h = gethostbyname(argv[1])) == nullptr) {
    std::cerr << "gethostbyname() failed\n";
    close(sockfd);
    return -1;
  }
  memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    std::cerr << "connect\n";
    close(sockfd);
    return -1;
  }

  char buffer[1024];
  for (int i = 0; i < 10; i++) {
    int iret;
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "this is %d", i + 1);
    if ((iret = send(sockfd, buffer, strlen(buffer), 0)) <= 0) {
      std::cerr << "send " << i << "\n";
      break;
    }
    std::cout << "send: " << buffer << std::endl;
    memset(buffer, 0, sizeof(buffer));
    if ((iret = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0) {
      std::cerr << "recv " << i << '\n';
      break;
    }
    std::cout << "recv: " << buffer << std::endl;
    sleep(1);
  }

  close(sockfd);

  return 0;
}