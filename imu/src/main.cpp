#include <chrono>
#include <iostream>
#include <thread>
#include <fmt/format.h>
#include <cmath>
#include <Eigen/Dense>

#include "socketcan.hpp"
#include "plotter.hpp"
#include "math_tools.hpp"
/*

struct Quaternion {
    double x, y, z, w;
};

struct EulerAngle {
    double roll, pitch, yaw;
};

EulerAngle quaternionToEulerAngles(const Quaternion& q) {
    EulerAngle angles;

    // 计算欧拉角
    double t0 = +2.0 * (q.w * q.x + q.y * q.z);
    double t1 = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
    angles.roll = std::atan2(t0, t1);

    double t2 = +2.0 * (q.w * q.y - q.z * q.x);
    t2 = (t2 > 1.0) ? 1.0 : t2;
    t2 = (t2 < -1.0) ? -1.0 : t2;
    angles.pitch = std::asin(t2);

    double t3 = +2.0 * (q.w * q.z + q.x * q.y);
    double t4 = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
    angles.yaw = std::atan2(t3, t4);

    return angles;
}

int main() {
    // 假设你有一个四元数
    Quaternion quaternion;
    quaternion.x = static_cast<double>(x);
    quaternion.y = static_cast<double>(y);
    quaternion.z = static_cast<double>(z);
    quaternion.w = static_cast<double>(w);

    // 将四元数转换为欧拉角
    EulerAngle angles = quaternionToEulerAngles(quaternion);

    // 输出欧拉角
    std::cout << "Roll: " << angles.roll << ", Pitch: " << angles.pitch << ", Yaw: " << angles.yaw << std::endl;

    return 0;
}
*/
void sendUDP(const can_frame &frame)
{
  auto x = (int16_t)(frame.data[0] << 8 | frame.data[1]) / 1e4;
  auto y = (int16_t)(frame.data[2] << 8 | frame.data[3]) / 1e4;
  auto z = (int16_t)(frame.data[4] << 8 | frame.data[5]) / 1e4;
  auto w = (int16_t)(frame.data[6] << 8 | frame.data[7]) / 1e4;
  // 定义四元数
  Eigen::Quaterniond quaternion(w, x, y, z);
  // 将四元数转换为旋转矩阵
  Eigen::Matrix3d rotation_matrix = quaternion.toRotationMatrix();

  // 从旋转矩阵中提取欧拉角inline Eigen::Vector3d eulers(Eigen::Quaterniond q, int axis0, int axis1, int axis2, bool extrinsic = false)
  // Eigen::Vector3d euler_angles = rotation_matrix.eulerAngles(2, 1, 0); // ZYX
  Eigen::Vector3d euler_angles = tools::eulers(quaternion,2,1,0);

  // 将弧度转换为度
  euler_angles = euler_angles * 180.0 / M_PI;

  // 输出欧拉角,ZYX顺序表示Roll(z)、Pitch(x)和Yaw(y)
  std::string yaw = std::to_string(euler_angles[0]);
  std::string roll = std::to_string(euler_angles[1]);
  std::string pitch = std::to_string(euler_angles[2]);
  std::string json = fmt::format(R"({{"roll":{},"pitch":{},"yaw":{}}})", roll, pitch, yaw);
  // std::cout << json << std::endl;///
  tools::Plotter ploter("192.168.58.128");//net 192.168.58.128/24 brd 192.168.58.255 scope global dynamic noprefixroute ens33
  ploter.plot(json);
}

int main()
{
  can::SocketCAN socketcan("can0", sendUDP);
  while (true)
  {
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