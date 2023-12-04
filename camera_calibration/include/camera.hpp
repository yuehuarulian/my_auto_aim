#ifndef AUTO_AIM__CAMERA_HPP
#define AUTO_AIM__CAMERA_HPP
#include "CameraApi.h" //相机SDK的API头文件
#include "opencv2/imgproc/imgproc_c.h"
#include <fmt/format.h> //**//
#include <opencv2/highgui.hpp>
#include <iostream>
class MVCamera
{
private:
    double exposureTime;
    tSdkFrameHead sFrameInfo;
    int hCamera;
    BYTE *pbyBuffer;
    unsigned char *g_pRgbBuffer; // 处理后数据缓存区
public:
    int FPS;
    bool error;
    MVCamera(double exposureTime);
    cv::Mat getFrame();
    ~MVCamera();
};


#endif // AUTO_AIM_CAMERA_HPP