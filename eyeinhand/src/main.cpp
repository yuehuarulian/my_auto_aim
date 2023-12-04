#include "io/cboard.hpp"
#include <chrono>
#include <thread>
#include "tools/exiter.hpp"
#include "tools/logger.hpp"
#include "tools/math_tools.hpp"
#include "Calibratecamera.hpp"
#include <iostream>
#include <fmt/format.h>
#include <cmath>
#include <Eigen/Dense>

#include <opencv2/opencv.hpp>
#include <opencv2/core/quaternion.hpp>
#include <opencv2/core/eigen.hpp>
using namespace std::chrono_literals;
using namespace std;
const int circle_width = 40;
const int board_w = 10;
const int board_h = 7;
const int board_n = 20;
const cv::Mat T_gimbal_to_imu = (cv::Mat_<double>(3, 3) << 0, 0, 1, -1, 0, 0, 0, -1, 0); // 云台到imu的矩阵

// void K_TO_CAMERA()
// {
// 相机标定
// Calibratecamera calibratecamera(board_w, board_h, board_n, circle_width);
// std::vector<cv::Vec3d> rvecs, tvecs; // 旋转向量 位移向量
// cv::Mat r, t;
// calibratecamera.Cali(R_k_to_camera, T_k_to_camera, timestamps);
// for (auto rvec : rvecs)
// {
//   cv::Rodrigues(rvec, r); // 罗德里格斯变换将旋转向量（rvec）转换为旋转矩阵（R）
//   R_k_to_camera.push_back(r);
// }
// // 将Eigen矩阵的数据复制到cv::Mat中
// for (auto tvec : tvecs)
// {
//   t = (cv::Mat_<double>(3, 1) << tvec[0], tvec[1], tvec[2]); // 一个3x1的列向量T，表示相机的平移向量。该向量包含在tvec数组中。
//   T_k_to_camera.push_back(t);
// }
// }
int main()
{
  std::vector<cv::Vec3d> R_k_to_camera, T_k_to_camera;       // 世界坐标到相机坐标矩阵 相机标定 标定板
  std::vector<cv::Mat> R_gimbal_to_earth, T_gimbal_to_earth; // imu到earth坐标矩阵
  std::vector<std::chrono::steady_clock::time_point> timestamps;
  cv::Mat R_camera_to_gimbal, T_camera_to_gimbal;
  io::CBoard cboard("can0");
  // 相机标定
  Calibratecamera calibratecamera(board_w, board_h, board_n, circle_width);
  calibratecamera.Cali(R_k_to_camera, T_k_to_camera, timestamps);
  // gimbal_to_earth
  for (auto timestamp : timestamps)
  {
    // 四元数（Quaternion）
    auto q = cboard.imu_at(timestamp);
    // 将四元数转换为旋转矩阵
    Eigen::Matrix3d R = q.toRotationMatrix();
    cv::Mat r;
    cv::eigen2cv(R.cast<float>().eval(), r);
    T_gimbal_to_earth.push_back(r * T_gimbal_to_imu);
    R_gimbal_to_earth.push_back((cv::Mat_<double>(3, 3) << 0, 0, 0, 0, 0, 0, 0, 0, 0));
  }
  cv::calibrateHandEye(R_gimbal_to_earth, T_gimbal_to_earth,   // imu到earth坐标矩阵
                       T_k_to_camera, T_k_to_camera,           // 世界坐标到相机坐标坐标矩阵
                       R_camera_to_gimbal, T_camera_to_gimbal, // 要求的矩阵 相机坐标到云台坐标矩阵
                       cv::CALIB_HAND_EYE_TSAI);
  std::cout << "Rotation Matrix (R):\n"
            << T_camera_to_gimbal << "\n";
  std::cout << "Translation Vector (T):\n"
            << T_camera_to_gimbal << "\n";
}

// void Eye_in_hand::GAMBAL_TO_EARTH()
// {
//     // tools::set_logger();

//     // io::CBoard cboard("can0");

//     // while (!exiter.exit())
//     // {
//     //     auto timestamp = std::chrono::steady_clock::now();
//     //     std::this_thread::sleep_for(1ms);
//     //     // 四元数（Quaternion）
//     //     auto q = cboard.imu_at(timestamp);
//     //     // 将四元数转换为旋转矩阵
//     //     Eigen::Matrix3d R = q.toRotationMatrix();
//     //     cv::Mat r;
//     //     cv::eigen2cv(R.cast<float>().eval(), r);
//     //     T_gimbal_to_earth.push_back(r * T_gimbal_to_imu);
//     //     R_gimbal_to_earth.push_back((cv::Mat_<double>(3, 3) << 0, 0, 0, 0, 0, 0, 0, 0, 0));
//     //     // auto eulers = tools::eulers(q, 2, 0, 1) * 57.3;
//     //     // spdlog::debug("{:.2f} {:.2f} {:.2f}", eulers[0], eulers[1], eulers[2]);
//     // }
// }

// using namespace std;

// int main()
// {
//   Eye_in_hand eih;
//   eih.CAMERA_TO_GIMBAL();
//   std::cout << "Rotation Matrix (R):\n" << eih.T_camera_to_gimbal << "\n";
//   std::cout << "Translation Vector (T):\n" << eih.T_camera_to_gimbal << "\n";

//   // can::SocketCAN socketcan("can0", sendUDP);
//   // while (true)
//   // {
//   //   using namespace std::chrono_literals;
//   //   std::this_thread::sleep_for(1ms);
//   // }
// }
