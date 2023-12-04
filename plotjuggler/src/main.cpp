#include <chrono>
#include <iostream>
#include <thread>
#include <fmt/format.h>


#include "socketcan.hpp"
#include "plotter.hpp"

void sendUDP(const can_frame &frame)
{
  auto degree = (int16_t)(frame.data[0] << 8 | frame.data[1]) / 8192.0 * 360.0;
  // 将数据转为字符串
  std::string data = std::to_string(degree);
  // 使用 fmt::format 将数据格式化为 JSON 字符串
  std::string json = fmt::format(R"({{"data":{}}})", data);
  // std::cout << data << std::endl;
  tools::Plotter ploter("192.168.58.129");
  ploter.plot(json);
}

int main()
{
  can::SocketCAN socketcan("can0", sendUDP);
  while (true){
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1ms);
  }
  // while (true)
  // {
  //   int16_t voltage = 3000;

  //   can_frame frame;
  //   frame.can_id = 0x1ff;
  //   frame.len = 8;
  //   frame.data[2] = voltage >> 8;
  //   frame.data[3] = voltage;

  //   socketcan.write(&frame);

  //   using namespace std::chrono_literals;
  //   std::this_thread::sleep_for(1ms);
  // }
}