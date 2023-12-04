#include "Calibratecamera.hpp"
#include <spdlog/spdlog.h>

#include <opencv2/opencv.hpp>

#include "io/mv_camera.hpp"
#include "tools/exiter.hpp"
#include "tools/logger.hpp"

const std::string keys =
  "{help h usage ? |     | 输出命令行参数说明}"
  "{@exposure      | 3.0 | 曝光时间，单位ms  }"
  "{@gamma         | 0.5 | 伽马值，基准为1   }"
  "{d display      |     | 显示视频流       }";

void Calibratecamera::Cali(std::vector<cv::Vec3d> rvec, std::vector<cv::Vec3d> tvec,std::vector<std::chrono::steady_clock::time_point> timestamps)
{
    /*
int main(int argc, char * argv[])
{
  tools::set_logger();

  cv::CommandLineParser parser(argc, argv, keys);
  if (parser.has("help")) {
    parser.printMessage();
    return 0;
  }

  tools::Exiter exiter;

  auto display = parser.has("display");
  auto exposure = parser.get<double>(0);
  auto gamma = parser.get<double>(1);
  io::MvCamera camera(exposure, gamma);

  cv::Mat img;
  std::chrono::steady_clock::time_point timestamp;
  auto last_stamp = std::chrono::steady_clock::now();
  while (!exiter.exit()) {
    camera.read(img, timestamp);

    std::chrono::duration<double> since = timestamp - last_stamp;
    last_stamp = timestamp;
    auto dt = since.count();

    spdlog::debug("{:.2f} fps", 1 / dt);

    if (!display) continue;
    cv::imshow("img", img);
    if (cv::waitKey(1) == 'q') break;
  }
}
*/  
    
    // tools::Exiter exiter;
    io::MvCamera mvcamera(3000, 0.5);
    // cv::Mat img;
    std::chrono::steady_clock::time_point timestamp;
    // auto last_stamp = std::chrono::steady_clock::now();
    // while (!exiter.exit()) {  //exit
    // mvcamera.read(img, timestamp);
    // }
    // std::chrono::duration<double> since = timestamp - last_stamp;
    // last_stamp = timestamp;
    // auto dt = since.count();
    // spdlog::debug("{:.2f} fps", 1 / dt);
    // if (!display) continue;
    // cv::imshow("img", img);
    // if (cv::waitKey(1) == 'q') break;
    // }

    cv::Size image_size;
    // 循环捕获图像进行标定
    while (image_points.size() < (size_t)n_boards)
    {
        cv::Mat image0, image;
        // 获取图片
        mvcamera.read(image0,timestamp);
        // image0 = mvcamera.getFrame();
        image_size = image0.size();
        cv::resize(image0, image, cv::Size(), image_sf, image_sf, cv::INTER_LINEAR);
        // 寻找标定板的角点
        vector<cv::Point2f> corners;
        bool found = cv::findCirclesGrid(image, board_sz, corners);
        // 绘制角点
        drawChessboardCorners(image, board_sz, corners, found);
        // 如果找到了好的标定板，将其添加到数据中
        // double timestamp = (double)clock() / CLOCKS_PER_SEC;
        if (found)// && timestamp - last_captured_timestamp > 1)
        {
            // last_captured_timestamp = timestamp;
            // image ^= cv::Scalar::all(255);
            cv::Mat mcorners(corners);   // do not copy the data
            mcorners *= (1. / image_sf); // 缩放角点坐标
            image_points.push_back(corners);
            object_points.push_back(vector<cv::Point3f>());
            timestamps.push_back(timestamp);
            vector<cv::Point3f> &opts = object_points.back();
            opts.resize(board_n);
            for (int j = 0; j < board_n; j++)
                opts[j] = cv::Point3f((float)(j / board_w)*circle_w, (float)(j % board_w)*circle_w, 0.f);
        }
    }
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
