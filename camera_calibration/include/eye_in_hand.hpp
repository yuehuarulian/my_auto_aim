#ifndef EYE_IN_HAND_HPP
#define EYE_IN_HAND_HPP
#include <chrono>
#include <iostream>
#include <thread>
#include <fmt/format.h>
#include <cmath>
#include <Eigen/Dense>

#include <opencv2/opencv.hpp>
#include <opencv2/core/quaternion.hpp>
#include <opencv2/core/eigen.hpp>

#include "socketcan.hpp"
#include "plotter.hpp"
#include "math_tools.hpp"
#include "Calibratecamera.hpp"
class Eye_in_hand
{
public:
    void CAMERA_TO_GIMBAL();
    cv::Mat R_camera_to_gimbal, T_camera_to_gimbal;
private:
    void K_TO_CAMERA();
    void GAMBAL_TO_EARTH(const can_frame &frame);
    std::vector<cv::Mat> R_k_to_camera, T_k_to_camera; // 世界坐标到相机坐标矩阵 相机标定 标定板
    const cv::Mat T_gimbal_to_imu = (cv::Mat_<double>(3, 3) << 0, 0, 1, -1, 0, 0, 0, -1, 0); // 云台到imu的矩阵
    std::vector<cv::Mat> R_gimbal_to_earth, T_gimbal_to_earth; // imu到earth坐标矩阵
};

#endif