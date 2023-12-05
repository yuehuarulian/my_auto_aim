#ifndef AUTO_AIM__TYPES_HPP
#define AUTO_AIM__TYPES_HPP

#include <algorithm>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace auto_aim
{
  // 暂时还用不到
  const std::vector<std::string> CLASS_NAMES = {
    "big_one",      "big_three",     "big_four",   "big_five",   "big_base",
    "small_two",    "small_three",   "small_four", "small_five", "small_base",
    "small_sentry", "small_outpost", "no_pattern"};
  const double armor_r=278.3;
//
struct Lightbar
{
  std::string color;
  cv::Point2f center, top, bottom, delta;
  std::vector<cv::Point2f> points;
  double angle, length, ratio;

  explicit Lightbar(const cv::RotatedRect & rotated_rect)
  {
    std::vector<cv::Point2f> points(4);
    rotated_rect.points(&points[0]);
    std::sort(points.begin(), points.end(), [](const cv::Point2f & a, const cv::Point2f & b) {
      return a.y < b.y;
    });
    
    this->center = rotated_rect.center;
    this->top = (points[0] + points[1]) / 2;
    this->bottom = (points[2] + points[3]) / 2;
    this->delta = this->bottom - this->top;

    this->points.emplace_back(this->top);
    this->points.emplace_back(this->bottom);

    auto width = cv::norm(points[0] - points[1]);

    // 参考这些判断条件！！
    this->angle = std::atan2(this->delta.y, this->delta.x);
    this->length = cv::norm(this->delta);
    this->ratio = this->length / width;
  }
};

struct Armor          //
{
  std::string color;
  Lightbar left, right;
  cv::Point2f center;
  std::vector<cv::Point2f> points;
  // 作业2：在这里加一些几何判断条件
  double angle1,angle2,angle3;
  double ratio1,ratio2;//

  std::string class_name;
  double confidence;
  cv::Mat pattern;

  cv::Mat rvec, tvec;

  Armor(const Lightbar & left, const Lightbar & right) : left(left), right(right)
  {
    this->color = left.color;
    this->center = (left.center + right.center) / 2;

    this->points.emplace_back(left.top);
    this->points.emplace_back(right.top);
    this->points.emplace_back(right.bottom);
    this->points.emplace_back(left.bottom);

    auto delta = right.center - left.center;
    auto angle = std::atan2(delta.y, delta.x);
    auto width = cv::norm(delta);
    auto max_lightbar_length = std::max(left.length, right.length);
    auto min_lightbar_length = std::min(left.length, right.length);

    // 作业2：在这里加一些几何判断条件
    auto delta_r=right.top-right.bottom;
    auto delta_l=left.top-left.bottom;
    auto angle_r= std::atan2(delta_r.y, delta_r.x);
    auto angle_l= std::atan2(delta_l.y, delta_l.x);

    this->angle1=std::fabs(angle-angle_r);//jia jiao 1
    this->angle2=std::fabs(angle-angle_l);//jia jiao 2
    this->angle3=std::fabs(angle_l-angle_r);
    
    this->ratio1 = width / ((max_lightbar_length+min_lightbar_length)/2);//Chang kuan bi
    this->ratio2=fabs(right.length-left.length)/right.length;
    
  }
};
}  // namespace auto_aim

#endif  // AUTO_AIM__TYPES_HPP