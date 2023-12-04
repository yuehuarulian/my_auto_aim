#include <chrono>
#include <iostream>
#include <thread>
#include <fmt/format.h>
#include <cmath>
#include <Eigen/Dense>
#include "math_tools.hpp"
#include "calibratecamera.hpp"
using namespace std;
// void sendUDP(const can_frame &frame)
// {
//   auto x = (int16_t)(frame.data[0] << 8 | frame.data[1]) / 1e4;
//   auto y = (int16_t)(frame.data[2] << 8 | frame.data[3]) / 1e4;
//   auto z = (int16_t)(frame.data[4] << 8 | frame.data[5]) / 1e4;
//   auto w = (int16_t)(frame.data[6] << 8 | frame.data[7]) / 1e4;
//   // 定义四元数
//   Eigen::Quaterniond quaternion(w, x, y, z);
//   // 将四元数转换为旋转矩阵
//   Eigen::Matrix3d T_imu_to_earth = quaternion.toRotationMatrix();
//   // 从旋转矩阵中提取欧拉角inline Eigen::Vector3d eulers(Eigen::Quaterniond q, int axis0, int axis1, int axis2, bool extrinsic = false)
//   // Eigen::Vector3d euler_angles = rotation_matrix.eulerAngles(2, 1, 0); // ZYX
//   Eigen::Vector3d euler_angles = tools::eulers(quaternion,2,1,0);
//   // 将弧度转换为度
//   euler_angles = euler_angles * 180.0 / M_PI;
//   // 输出欧拉角,ZYX顺序表示Roll(z)、Pitch(x)和Yaw(y)
//   std::string yaw = std::to_string(euler_angles[0]);
//   std::string roll = std::to_string(euler_angles[1]);
//   std::string pitch = std::to_string(euler_angles[2]);
//   std::string json = fmt::format(R"({{"roll":{},"pitch":{},"yaw":{}}})", roll, pitch, yaw);
//   // std::cout << json << std::endl;///
//   tools::Plotter ploter("192.168.58.128");//net 192.168.58.128/24 brd 192.168.58.255 scope global dynamic noprefixroute ens33
//   ploter.plot(json);
// }

int main()
{

  int w=10;
  int h=7;
  int n=1;
  Calibratecamera cal(w,h,n);
  vector<cv::Vec3d> rvec,tvec;
  cal.Cali(rvec,tvec);
  cout<<cal.cameraMatrix<<endl;
  cout<<cal.distCoeffs<<endl;

}