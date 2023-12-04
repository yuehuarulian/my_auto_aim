#include "camera.hpp"

extern std::chrono::_V2::system_clock::time_point startTime, endTime;
extern double durationInSeconds;
extern bool time_again;
MVCamera::MVCamera(double Time = 1500)
{
    error = false;
    exposureTime = Time;
    int iCameraCounts = 1;
    int iStatus = -1;
    tSdkCameraDevInfo tCameraEnumList;
    tSdkCameraCapbility tCapability; // 设备描述信息
    int iDisplayFrames = 10000;
    int channel = 3;
    CameraSdkInit(1);
    // 枚举设备，并建立设备列表
    iStatus = CameraEnumerateDevice(&tCameraEnumList, &iCameraCounts);
    // 没有连接设备
    if (iCameraCounts == 0)
    {
        error = true;
        std::cout << "没有连接设备" << std::endl;
        return;
    }
    // 相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    iStatus = CameraInit(&tCameraEnumList, -1, -1, &hCamera);
    // 初始化失败
    printf("state = %d\n", iStatus);
    if (iStatus != CAMERA_STATUS_SUCCESS)
    {
        std::cout << "初始化失败" << std::endl;
        error = true;
        return;
    }
    // 获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(hCamera, &tCapability);
    g_pRgbBuffer = (unsigned char *)malloc(tCapability.sResolutionRange.iHeightMax * tCapability.sResolutionRange.iWidthMax * 3);
    // g_readBuf = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);

    // 设置曝光时间
    CameraPlay(hCamera);
    CameraSetAeState(hCamera, FALSE);
    if (CameraSetExposureTime(hCamera, exposureTime))
    {
        std::cout << "设置曝光时间失败" << std::endl;
        error = true;
        return;
    }

    // detector channel
    if (tCapability.sIspCapacity.bMonoSensor)
    {
        channel = 1;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);
    }
    else
    {
        channel = 3;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_BGR8);
    }
    // cv::namedWindow("Display window");
    return;
}
cv::Mat MVCamera::getFrame()
{
    CameraReleaseImageBuffer(hCamera, pbyBuffer);
    // get picture
    if (CameraGetImageBuffer(hCamera, &sFrameInfo, &pbyBuffer, 1000) == CAMERA_STATUS_SUCCESS)
    {
        CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer, &sFrameInfo);

        cv::Mat matImage(
            cvSize(sFrameInfo.iWidth, sFrameInfo.iHeight),
            sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
            g_pRgbBuffer);
        return matImage;
    }

    // error = false;
    cv::Mat matImage;
    return matImage;
}
MVCamera::~MVCamera()
{
    CameraUnInit(hCamera);
    // 注意，现反初始化后再free
    free(g_pRgbBuffer);
    cv::destroyAllWindows();
    return;
}

