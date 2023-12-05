#include <algorithm>
#include "getv.hpp"
extern std::chrono::_V2::system_clock::time_point startTime, endTime;
extern double durationInSeconds;
extern bool time_again;
namespace auto_aim
{
    // void Getv::Getangle()
    // {
    //     if(camerapoint.size()<6)
    //     {
    //         angle_radians=0;///0.2765m
    //         return;
    //     }
    //     cv::RotatedRect ellipse = cv::fitEllipse(camerapoint);//judge???
    //     cv::Point2f circleCenter = ellipse.center;
    //     std::cout<<"circleCenter:"<<circleCenter.x<<' '<<circleCenter.y<<std::endl;
    //     cv::Point2f v1 = camerapoint.back() - circleCenter;
    //     cv::Point2f v2 = camerapoint.front() - circleCenter;
    //     std::cout<<fsqrt(v2.x*v2.x+v2.y*v2.y)<<' '<<fsqrt(v2.x*v2.x+v2.y*v2.y)<<std::endl;
    //     // 计算点积
    //     double dot_product = v1.dot(v2);
    //     // 计算向量的模（长度）
    //     double norm_v1 = cv::norm(v1);
    //     double norm_v2 = cv::norm(v2);
    //     // 计算夹角（弧度）
    //     angle_radians = std::acos(dot_product / (norm_v1 * norm_v2));
    //     angle_v=angle_radians/durationInSeconds;//
    //     RPM = angle_radians / (2 * CV_PI) * 60 / durationInSeconds+0.5;
    // }

    void Getv::Getangle()
    {
        std::cout<<"size:"<<camerapoint.size()<<std::endl;
        if (camerapoint.size() < 6)
        {
            // angle_radians = 0;
            return;
        }
        cv::Point3f l = camerapoint.back() - camerapoint.front();
        double L = fsqrt(l.x * l.x + l.y * l.y + l.z * l.z) / 2; // length
        // 计算夹角（弧度）
        std::cout<<"L:"<<L<<std::endl;
        if (L < 160||L>250)//170
        {
            // angle_radians = 0;
            return;
        }
        angle_radians = 2 * asin(L / armor_r);       /// 278.3mm

        // if(angle_v>2)
        //     angle_radians=2.0;
        durationInSeconds=durationInSeconds*0.64;
        angle_v_jh.push_back(angle_radians / durationInSeconds);
        angle_v=0;
        for(int i=0;i<angle_v_jh.size();i++)
        {
            angle_v+=angle_v_jh[i];
        }
        angle_v/=angle_v_jh.size();
        // angle_v = angle_radians / durationInSeconds; //
        if(l.x>0)
        angle_v=-angle_v;
        RPM = fabs(angle_radians / (2 * CV_PI) * 60 / durationInSeconds);
        // if(angle_radians>1.9&&angle_radians<2.1)//1.9 2.1
        //     gd=1;
    }
    bool Getv::Getcamerapoint(cv::Vec3d vector)
    {
        if(gd)
            return true;
        cv::Point3d a(vector[0], vector[1] , vector[2]); 
        // std::cout<<vector[1]<<std::endl;                                      //(x,z)
        if (camerapoint.empty() || (fabs(camerapoint.back().x - vector[0]) < 200)) // gai
        {
            // std::cout<<a.x<<std::endl;
            camerapoint.push_back(a);
            time_again = 0;
            return false;
        }
        else
        {
            Getangle();
            time_again = 1;
            // startTime = endTime; // 重新计时
            camerapoint.clear();
            return true;
        }
    }

}