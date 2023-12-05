#ifndef AUTO_AIM__DETECTOR_HPP
#define AUTO_AIM__DETECTOR_HPP

#include "types.hpp"
#include "classifier.hpp"

namespace auto_aim
{
class Detector
{
public:
  Detector() = default;

  std::vector<Armor> detect(const cv::Mat & bgr_img);

private:
  Classifier classifier;  // 下节课会用到
  bool check_geometry(const Lightbar & lightbar);
  bool check_geometry(const Armor & armor);
  bool check_class(const Armor & armor);
  std::string get_color(
    const cv::Mat & bgr_img, const std::vector<cv::Point> & contour,
    const cv::RotatedRect & rotated_rect);
  void set_pattern(Armor & armor, const cv::Mat & gray_img);
  
};

}  // namespace auto_aim

#endif  // AUTO_AIM__DETECTOR_HPP