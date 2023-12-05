#include "solvepnp.hpp"
#include <fmt/format.h>
#include "img_tools.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core/quaternion.hpp>
#include <vector>
namespace auto_aim
{
    Solvepnp::Solvepnp(const struct Armor armor, float lightbar_wid, float lightbar_len)
    {
        // objectPoints.push_back(cv::Point2f{0, 0});
        objectPoints.push_back(cv::Point3f{-lightbar_wid / 2, -lightbar_len / 2, 0});
        objectPoints.push_back(cv::Point3f{lightbar_wid / 2, -lightbar_len / 2, 0});
        objectPoints.push_back(cv::Point3f{lightbar_wid / 2, lightbar_len / 2, 0});
        objectPoints.push_back(cv::Point3f{-lightbar_wid / 2, lightbar_len / 2, 0});
        
        imagePoints.push_back(armor.left.top);
        imagePoints.push_back(armor.right.top);
        imagePoints.push_back(armor.right.bottom);
        imagePoints.push_back(armor.left.bottom);
        Rvec();
    }

    void Solvepnp::getQuaternion(cv::Mat R) // 2
    {
        double trace = R.at<double>(0, 0) + R.at<double>(1, 1) + R.at<double>(2, 2);

        if (trace > 0.0)
        {
            double s = sqrt(trace + 1.0);
            Q[3] = (s * 0.5);
            s = 0.5 / s;
            Q[0] = ((R.at<double>(2, 1) - R.at<double>(1, 2)) * s);
            Q[1] = ((R.at<double>(0, 2) - R.at<double>(2, 0)) * s);
            Q[2] = ((R.at<double>(1, 0) - R.at<double>(0, 1)) * s);
        }

        else
        {
            int i = R.at<double>(0, 0) < R.at<double>(1, 1) ? (R.at<double>(1, 1) < R.at<double>(2, 2) ? 2 : 1) : (R.at<double>(0, 0) < R.at<double>(2, 2) ? 2 : 0);
            int j = (i + 1) % 3;
            int k = (i + 2) % 3;

            double s = sqrt(R.at<double>(i, i) - R.at<double>(j, j) - R.at<double>(k, k) + 1.0);
            Q[i] = s * 0.5;
            s = 0.5 / s;

            Q[3] = (R.at<double>(k, j) - R.at<double>(j, k)) * s;
            Q[j] = (R.at<double>(j, i) + R.at<double>(i, j)) * s;
            Q[k] = (R.at<double>(k, i) + R.at<double>(i, k)) * s;
        }
        cv::Quat quat(Q[3],Q[0],Q[1],Q[2]);
        euler_angles=quat.toEulerAngles(cv::QuatEnum::INT_ZYX)*180/CV_PI;

    }
    void Solvepnp::Rvec() // 1
    {
        solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec, false, cv::SOLVEPNP_IPPE);
        cv::Rodrigues(rvec, R);
        T=(cv::Mat_<double>(3, 1)<<tvec[0],tvec[1],tvec[2]);
        getQuaternion(R); // 四元数
    }
    // cv::Vec3d Solvepnp::Position(cv::Point2f armorpoint)
    // {
    //     cv::Mat point= (cv::Mat_<double>(3, 1)<<armorpoint.x,armorpoint.y,0);
    //     // cv::Mat Ta;
    //     // cv::hconcat(R, T, Ta);  // 将两个Mat水平拼接成一个新的Mat
    //     // cv::Mat m=(cv::Mat_<double>(1, 4)<<0,0,0,1);
    //     // cv::vconcat(Ta,m, Ta);  // 将两个Mat竖直拼接成一个新的Mat
    //     cv::Mat camerapoint=1/tvec[2]*cameraMatrix*(R*point+T);//1/Z*K(Ra*Va+ta)
    //     cv::Vec3d CAMERAPOINT(camerapoint);
    //     return CAMERAPOINT;
    // }




}