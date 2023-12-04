#ifndef SOCKETCAN_HPP
#define SOCKETCAN_HPP

#include <linux/can.h>
#include <net/if.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstring>
#include <functional>
#include <stdexcept>
#include <thread>

constexpr int MAX_EVENTS = 10;

namespace can
{
class SocketCAN
{
public:
  SocketCAN(const std::string & interface, std::function<void(const can_frame & frame)> handler)
  : socket_fd_(-1), epoll_fd_(-1), handler_(handler), quit_(false)
  {
    socket_fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_fd_ < 0) throw std::runtime_error("Error opening socket");

    std::strncpy(ifr_.ifr_name, interface.c_str(), IFNAMSIZ - 1);
    if (ioctl(socket_fd_, SIOCGIFINDEX, &ifr_) < 0)
      throw std::runtime_error("Error getting interface index");

    sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr_.ifr_ifindex;
    if (bind(socket_fd_, (sockaddr *)&addr, sizeof(addr)) < 0) {
      ::close(socket_fd_);
      throw std::runtime_error("Error binding socket to interface");
    }

    epoll_event ev;
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1) throw std::runtime_error("Error creating epoll file descriptor");

    ev.events = EPOLLIN;
    ev.data.fd = socket_fd_;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, ev.data.fd, &ev))
      throw std::runtime_error("Error adding socket to epoll file descriptor");

    // 接收线程
    read_thread_ = std::thread([this]() {
      can_frame frame;
      while (!quit_) {
        int num_events = epoll_wait(epoll_fd_, events_, MAX_EVENTS, -1);
        if (num_events == -1) throw std::runtime_error("Error wating for events");

        for (int i = 0; i < num_events; i++) {
          ssize_t num_bytes = recv(socket_fd_, &frame, sizeof(frame), MSG_DONTWAIT);
          if (num_bytes == -1) throw std::runtime_error("Error reading from SocketCAN");

          handler_(frame);
        }
      }
      return;
    });
  }

  ~SocketCAN()
  {
    quit_ = true;
    if (read_thread_.joinable()) read_thread_.join();

    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, socket_fd_, NULL);
    ::close(epoll_fd_);
    ::close(socket_fd_);
  }

  void write(can_frame * frame) const
  {
    if (::write(socket_fd_, frame, sizeof(can_frame)) == -1)
      throw std::runtime_error("Unable to write");
  }

private:
  int socket_fd_;
  ifreq ifr_;
  int epoll_fd_;
  bool quit_;
  std::thread read_thread_;
  epoll_event events_[MAX_EVENTS];
  std::function<void(const can_frame & frame)> handler_;
};
}  // namespace can

#endif  // SOCKETCAN_HPP