#include "../include/Channel.h"

#include "../include/Epoll.h"
#include "../include/InetAddress.h"
#include "../include/Socket.h"

Channel::Channel(Epoll *ep, int fd, bool is_listen)
    : fd_(fd), ep_(ep), is_listen_(is_listen) {}
Channel::~Channel() {}

int Channel::fd() { return fd_; }

void Channel::useet() { events_ = events_ | EPOLLET; }

void Channel::enable_reading() {
  events_ |= EPOLLIN;
  ep_->update_channel(this);
}

void Channel::set_inepoll() { inepoll_ = true; }

void Channel::set_revents(uint32_t ev) { revents_ = ev; }

bool Channel::inpoll() { return inepoll_; }

uint32_t Channel::events() { return events_; }

uint32_t Channel::revents() { return revents_; }

void Channel::handle_event(Socket *servsock) {
  if (revents() & EPOLLRDHUP) {
    std::cout << "disconnected: " << fd_ << std::endl;
    ::close(fd_);
    return;
  }
  if (revents() & (EPOLLIN | EPOLLPRI)) {  // 接收缓冲区有数据可以读
    if (is_listen_) {
      InetAddress clientaddr;
      Socket *clientsock = new Socket(servsock->accept(clientaddr));
      std::cout << "accept client(fd=" << clientsock->fd()
                << ",ip=" << clientaddr.ip() << ",port=" << clientaddr.port()
                << ")" << std::endl;
      Channel *clientchannel = new Channel(ep_, clientsock->fd(), false);
      clientchannel->useet();
      clientchannel->enable_reading();
    } else {
      char buffer[1024];
      while (true) {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd_, buffer, sizeof(buffer));
        if (nread > 0) {
          std::cout << "recv from " << fd_ << ": " << buffer << std::endl;
          ::send(fd_, buffer, strlen(buffer), 0);
          continue;
        }
        if (nread == -1 && errno == EINTR) continue;
        if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) break;
        if (nread == 0) {
          std::cout << "disconnected: " << fd_ << std::endl;
          ::close(fd_);
          break;
        }
      }
    }
    return;
  }
  if (revents_ & EPOLLOUT) return;
  std::cerr << "error " << fd_ << '\n';
  ::close(fd_);
}