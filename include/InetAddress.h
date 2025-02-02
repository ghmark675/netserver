#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <netinet/in.h>
#include <sys/socket.h>

#include <string>

class InetAddress {
 private:
  sockaddr_in addr_;

 public:
  InetAddress();
  InetAddress(uint16_t port, const std::string &ip = "");
  InetAddress(const sockaddr_in addr);
  ~InetAddress();

  const char *ip() const;
  uint16_t port() const;
  const sockaddr *addr() const;
  void setaddr(sockaddr_in clientaddr);
};

#endif