#include "../include/Ctcpclient.h"

bool Ctcpclient::connect(const std::string &_ip, const unsigned short _port) {
  if (sock != -1) return false;
  ip = _ip;
  port = _port;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) return false;

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  struct hostent *h;
  if ((h = gethostbyname(ip.c_str())) == nullptr) {
    ::close(sock);
    sock = -1;
    return false;
  }
  memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);

  if (::connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    ::close(sock);
    sock = -1;
    return false;
  }

  return true;
}