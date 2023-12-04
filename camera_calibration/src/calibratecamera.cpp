#include "calibratecamera.hpp"
#include "camera.hpp"
void Calibratecamera::Cali(std::vector<cv::Vec3d> rvec, std::vector<cv::Vec3d> tvec)
{
    // cv::VideoCapture capture(0);////////
    // if (!capture.isOpened())
    // {
    //     cout << "\nCouldn't open the camera\n";
    //     exit(-1);
    // }
    std::string imagePath = "/home/ruiling/Desktop/camera_bd/images/0.png";
    cv::Mat image0, image;
    // 加载图片
    image0 = cv::imread(imagePath);

    // 检查是否成功加载图片
    if (image0.empty())
    {
        std::cerr << "错误：无法打开或找到图片！" << std::endl;
        return;
    }
    // MVCamera mvcamera(3000);
    // double last_captured_timestamp = 0;
    cv::Size image_size;
    // 循环捕获图像进行标定
    // while (image_points.size() < (size_t)n_boards)
    // {
    // 获取图片
    // image0 = mvcamera.getFrame();
    image_size = image0.size();
    cv::resize(image0, image, cv::Size(), image_sf, image_sf, cv::INTER_LINEAR);
    // 寻找标定板的角点
    vector<cv::Point2f> corners;
    // bool found = cv::findChessboardCorners(image, board_sz, corners);
    bool found = cv::findCirclesGrid(image, board_sz, corners);
    // 绘制角点
    drawChessboardCorners(image, board_sz, corners, found);
    // 显示图像
    cv::imshow("WindowName", image);
    cv::waitKey(0); // Wait for a key press

    // 如果找到了好的标定板，将其添加到数据中
    // double timestamp = (double)clock() / CLOCKS_PER_SEC;
    if (found )//&& timestamp - last_captured_timestamp > 1)
    {
        // last_captured_timestamp = timestamp;
        image ^= cv::Scalar::all(255);
        cv::Mat mcorners(corners);   // do not copy the data
        mcorners *= (1. / image_sf); // 缩放角点坐标
        image_points.push_back(corners);
        object_points.push_back(vector<cv::Point3f>());
        vector<cv::Point3f> &opts = object_points.back();
        opts.resize(board_n);
        for (int j = 0; j < board_n; j++)
            opts[j] = cv::Point3f((float)(j / board_w)*40, (float)(j % board_w)*40, 0.f);//mm
    }
    // }
    // 进行相机标定
    double err = cv::calibrateCamera(
        object_points,
        image_points,
        image_size,
        cameraMatrix, // 输出参数，包含相机内参数的3x3矩阵。
        distCoeffs,
        rvec, // 旋转向量的输出参数。每个输入图像对应一个旋转向量。
        tvec, // 平移向量的输出参数。每个输入图像对应一个平移向量。
        cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT);
    return;
}
