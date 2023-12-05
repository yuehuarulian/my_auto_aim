#ifndef AUTO_AIM__SOLVEPNP_HPP
#define AUTO_AIM__SOLVEPNP_HPP

#include <algorithm>
#include <opencv2/opencv.hpp>
#include <vector>
#include "types.hpp"

namespace auto_aim
{
struct Solvepnp{
private:
    std::vector<cv::Point3f> objectPoints;
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << 1289.3547817933504, 0, 616.37198981108247, 0,1289.7605632176849, 533.56820711339208, 0, 0, 1);
    cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << -0.49601990997390721, 0.31252059781156255,-0.00030082090350255081, 0.0014378155986921799,-0.13485901706118947);
    
    // cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << 1660.2821278459987, 0, 633.591316919562, 0, 1661.2255942186246, 492.758241706417, 0, 0, 1);
    // cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << -0.4867021881433142, 0.3129895439924416, -0.0010718344822318786, 0.0009860762382269605, -0.19085813610691532);
    
    void Rvec();
    // void toEulerAngles(float q0, float q1, float q2, float q3);
    void getQuaternion(cv::Mat R);
    void getEulerAngles();
    double Q[4];
    std::vector<cv::Point2f> imagePoints;
    
public:
    Solvepnp(const struct Armor armor,float lightbar_wid,float lightbar_len);
    // cv::Vec3d Position(cv::Point2f armorpoint);
    cv::Vec3d euler_angles;
    cv::Vec3d rvec, tvec;//旋转向量 位移向量 
    cv::Mat R,T; // 旋转矩阵 位移矩阵
};


}
#endif  // AUTO_AIM__SOLVEPNP_HPP