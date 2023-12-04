#ifndef CALIBRATECAMERA_HPP
#define CALIBRATECAMERA_HPP
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <opencv2/core.hpp> // CV_PI
using namespace std;
class Calibratecamera
{
public:
    Calibratecamera(int w, int h, int n, int circle_w, float del = 1., float im_sf = 0.5)
    {
        // Will be set by input list
        board_w = w;
        board_h = h;
        delay = del;
        image_sf = im_sf;
        board_n = board_w * board_h;
        n_boards = n;
        board_sz = cv::Size(board_w, board_h);
    }
    void Cali(std::vector<cv::Vec3d> rvec, std::vector<cv::Vec3d> tvec,std::vector<std::chrono::steady_clock::time_point> timestamps);
    cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);  // 初始化相机内参数矩阵为单位矩阵
    cv::Mat distCoeffs = cv::Mat::zeros(5, 1, CV_64F);  // 初始化畸变系数为零

private:
    int n_boards = 0; // Will be set by input list
    float image_sf = 0.5f;
    float delay = 1.f;
    int board_w = 0;
    int board_h = 0;
    int board_n;
    int circle_w;
    cv::Size board_sz;
    cv::Size image_size;
    // ALLOCATE STORAGE
    
    vector<vector<cv::Point2f>> image_points;
    vector<vector<cv::Point3f>> object_points;
};
#endif