#include "../include/InetAddress.h"

#include <arpa/inet.h>
#include <sys/socket.h>

InetAddress::InetAddress(uint16_t port, const std::string &ip) {
  addr_.sin_family = AF_INET;
  if (ip == "") addr_.sin_addr.s_addr = htonl(INADDR_ANY);
  addr_.sin_port = htons(port);
}

InetAddress::InetAddress(const sockaddr_in addr) : addr_(addr) {}
InetAddress::~InetAddress() {}

const char *InetAddress::ip() const { return inet_ntoa(addr_.sin_addr); }
uint16_t InetAddress::port() const { return ntohs(addr_.sin_port); }
const sockaddr *InetAddress::addr() const { return (sockaddr *)&addr_; }