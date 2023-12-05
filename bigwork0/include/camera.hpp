#ifndef AUTO_AIM__CAMERA_HPP
#define AUTO_AIM__CAMERA_HPP
#include "CameraApi.h" //相机SDK的API头文件
#include "types.hpp"

namespace auto_aim
{
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

} // namespace auto_aim

#endif // AUTO_AIM__CAMERA_HPP