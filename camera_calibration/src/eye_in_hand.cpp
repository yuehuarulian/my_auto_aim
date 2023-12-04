#include "eye_in_hand.hpp"
const int handeyecou = 40;///
void Eye_in_hand::CAMERA_TO_GIMBAL()
{
    can::SocketCAN socketcan("can0", GAMBAL_TO_EARTH); //???
    while (true)
    {
        K_TO_CAMERA();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
        if (T_k_to_camera.size() > handeyecou && T_gimbal_to_earth.size() > handeyecou)
            break;
    }
    cv::calibrateHandEye(R_gimbal_to_earth, T_gimbal_to_earth,   // imu到earthi坐标矩阵
                         T_k_to_camera, T_k_to_camera,           // 世界坐标到相机坐标坐标矩阵
                         R_camera_to_gimbal, T_camera_to_gimbal, // 要求的矩阵 相机坐标到云台坐标矩阵
                         cv::CALIB_HAND_EYE_TSAI);
}
void Eye_in_hand::K_TO_CAMERA()
{
    // 相机标定
    Calibratecamera calibratecamera(30, 30, 30); // w h n
    cv::Vec3d rvec, tvec;                        // 旋转向量 位移向量
    cv::Mat r, t;
    calibratecamera.Cali(rvec, tvec);
    cv::Rodrigues(rvec, r); // 罗德里格斯变换将旋转向量（rvec）转换为旋转矩阵（R）
    // 将Eigen矩阵的数据复制到cv::Mat中
    t = (cv::Mat_<double>(3, 1) << tvec[0], tvec[1], tvec[2]); // 一个3x1的列向量T，表示相机的平移向量。该向量包含在tvec数组中。
    T_k_to_camera.push_back(t);
    R_k_to_camera.push_back(r);
}
void Eye_in_hand::GAMBAL_TO_EARTH(const can_frame &frame)
{
    auto x = (int16_t)(frame.data[0] << 8 | frame.data[1]) / 1e4;
    auto y = (int16_t)(frame.data[2] << 8 | frame.data[3]) / 1e4;
    auto z = (int16_t)(frame.data[4] << 8 | frame.data[5]) / 1e4;
    auto w = (int16_t)(frame.data[6] << 8 | frame.data[7]) / 1e4;
    // 定义四元数
    Eigen::Quaterniond quaternion(w, x, y, z);
    // 将四元数转换为旋转矩阵
    Eigen::Matrix3d R = quaternion.toRotationMatrix();
    cv::Mat r;
    cv::eigen2cv(R.cast<float>().eval(), r);
    T_gimbal_to_earth.push_back(r * T_gimbal_to_imu);
    R_gimbal_to_earth.push_back((cv::Mat_<double>(3, 3) << 0, 0, 0, 0, 0, 0, 0, 0, 0));
}
