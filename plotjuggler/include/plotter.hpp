#ifndef TOOLS__PLOTTER_HPP
#define TOOLS__PLOTTER_HPP

#include <arpa/inet.h>   // htons, inet_addr
#include <netinet/in.h>  // sockaddr_in
#include <sys/socket.h>  // socket, sendto
#include <sys/types.h>   // uint16_t
#include <unistd.h>      // close

#include <iostream>
#include <string>

namespace tools
{
class Plotter
{
public:
  Plotter(std::string host = "127.0.0.1", uint16_t port = 9870)
  {
    socket_ = ::socket(AF_INET, SOCK_DGRAM, 0);

    destination_.sin_family = AF_INET;
    destination_.sin_port = htons(port);
    destination_.sin_addr.s_addr = inet_addr(host.c_str());
  }

  ~Plotter() { ::close(socket_); }

  void plot(const std::string & json)
  {
    ::sendto(
      socket_, json.c_str(), json.length(), 0, reinterpret_cast<sockaddr *>(&destination_),
      sizeof(destination_));
      std::cout << json << std::endl;
  }

private:
  int socket_;
  sockaddr_in destination_;
};

}  // namespace tools

#endif  // TOOLS__PLOTTER_HPP