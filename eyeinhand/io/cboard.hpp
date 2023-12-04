#ifndef IO__CBOARD_HPP
#define IO__CBOARD_HPP

#include <spdlog/spdlog.h>

#include <Eigen/Geometry>
#include <chrono>
#include <functional>

#include "io/socketcan.hpp"
#include "tools/thread_safe_queue.hpp"

namespace io
{

class CBoard
{
public:
  CBoard(const std::string & interface)
  : can_(interface, std::bind(&CBoard::callback, this, std::placeholders::_1)),
    queue_(5000)
  {
  }

  Eigen::Quaterniond imu_at(std::chrono::steady_clock::time_point timestamp)
  {
    IMUData data_behind;
    while (true) {
      queue_.pop(data_behind);
      if (data_behind.timestamp > timestamp) break;
      last_data_ahead_ = data_behind;
    }

    auto q_a = last_data_ahead_.q;
    auto q_b = data_behind.q;
    auto t_a = last_data_ahead_.timestamp;
    auto t_b = data_behind.timestamp;
    auto t_c = timestamp;
    std::chrono::duration<double> t_ab = t_b - t_a;
    std::chrono::duration<double> t_ac = t_c - t_a;

    // 四元数插值
    auto k = t_ac / t_ab;
    auto q_c = q_a.slerp(k, q_b);

    return q_c;
  }

  // TODO
  // void write(...) const {}

private:
  struct IMUData
  {
    Eigen::Quaterniond q;
    std::chrono::steady_clock::time_point timestamp;
  };

  SocketCAN can_;
  tools::ThreadSafeQueue<IMUData> queue_;
  IMUData last_data_ahead_;

  void callback(const can_frame & frame)
  {
    auto timestamp = std::chrono::steady_clock::now();

    if (frame.can_id == 0x400) {
      auto x = (int16_t)(frame.data[0] << 8 | frame.data[1]) / 1e4;
      auto y = (int16_t)(frame.data[2] << 8 | frame.data[3]) / 1e4;
      auto z = (int16_t)(frame.data[4] << 8 | frame.data[5]) / 1e4;
      auto w = (int16_t)(frame.data[6] << 8 | frame.data[7]) / 1e4;

      queue_.push({{w, x, y, z}, timestamp});
    }

    else if (frame.can_id == 0x401) {
      // TODO
    }
  }
};

}  // namespace io

#endif  // IO__CBOARD_HPP