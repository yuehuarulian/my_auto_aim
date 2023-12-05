#ifndef AUTO_AIM__GETV_HPP
#define AUTO_AIM__GETV_HPP

#include "types.hpp"
#include "solvepnp.hpp"
#include <vector>
#include <opencv2/opencv.hpp>

namespace auto_aim
{
struct Getv{
private:
    std::vector<cv::Point3f> camerapoint;
    std::vector<double> angle_v_jh;
    void Getangle();
public:
    // std::vector<cv::Vec3d> camerapoint;
    bool Getcamerapoint(cv::Vec3d vector);
    double angle_radians = 0;
    double angle_v=0;
    double RPM;
    bool gd=0;
};



}  // namespace auto_aim

#endif  // AUTO_AIM__GETV_HPP