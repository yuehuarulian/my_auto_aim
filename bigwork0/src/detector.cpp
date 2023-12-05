#include "detector.hpp"
#include <fmt/format.h>
#include "img_tools.hpp"
#include "solvepnp.hpp"
#include "getv.hpp"
extern double durationInSeconds;
namespace auto_aim
{
std::vector<Armor> Detector::detect(const cv::Mat & bgr_img)
{
  // 彩色图转灰度图
  cv::Mat gray_img;
  cv::cvtColor(bgr_img, gray_img, cv::COLOR_BGR2GRAY);//彩色图转灰度图
  // cv::imshow("gray", gray_img);  // 调试用

  // 进行二值化           :把高于100变成255，低于100变成0
  cv::Mat binary_img;
  cv::threshold(gray_img, binary_img, 100, 255, cv::THRESH_BINARY);
  // cv::imshow("binary", binary_img);  // 调试用

  // 获取轮廓点
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(binary_img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);//external找外部区域

  // // 显示轮廓点，调试用
  // cv::Mat drawcontours = bgr_img.clone();
  // for (const auto & contour : contours) {
  //   tools::draw_points(drawcontours, contour);
  // }
  // cv::imshow("drawcontours", drawcontours);

  // // 获取旋转矩形并显示，示范用
  // std::vector<cv::RotatedRect> rotated_rects;
  // for (const auto & contour : contours) {
  //   auto rotated_rect = cv::minAreaRect(contour);
  //   rotated_rects.emplace_back(rotated_rect);
  // }
  // cv::Mat drawrect = bgr_img.clone();
  // for (const auto & rotated_rect : rotated_rects) {
  //   std::vector<cv::Point2f> points(4);
  //   rotated_rect.points(&points[0]);
  //   tools::draw_points(drawrect, points);
  // }
  // cv::imshow("drawrect", drawrect);

  // 获取灯条
  std::vector<Lightbar> lightbars;
  for (const auto & contour : contours) {
    auto rotated_rect = cv::minAreaRect(contour);
    auto lightbar = Lightbar(rotated_rect);

    if (!this->check_geometry(lightbar)) continue;

    lightbar.color = this->get_color(bgr_img, contour, rotated_rect);
    lightbars.emplace_back(lightbar);
  }

  // 显示灯条，调试用
  // cv::Mat drawlightBar = bgr_img.clone();
  // for (const auto & lightbar : lightbars) {
  //   tools::draw_points(drawlightBar, lightbar.points);
  //   tools::draw_text(drawlightBar, lightbar.color, lightbar.top);
  // }
  // cv::imshow("drawlightBar", drawlightBar);

  // 将灯条从左到右排序
  std::sort(lightbars.begin(), lightbars.end(), [](const Lightbar & a, const Lightbar & b) {
    return a.center.x < b.center.x;
  });

  // 获取装甲板  //两两配对灯条检测合理性
  std::vector<Armor> armors;
  for (auto left = lightbars.begin(); left != lightbars.end(); left++) {
    for (auto right = left + 1; right != lightbars.end(); right++) {
      if (left->color != right->color) continue;

      auto armor = Armor(*left, *right);
      if (!this->check_geometry(armor)) continue;

      this->set_pattern(armor, gray_img);

      this->classifier.classify(armor);  // 下节课会用到

      if (!this->check_class(armor)) continue;  // 下节课会用到

      armors.emplace_back(armor);
    }
  }

  // // 显示装甲板，调试用
  cv::Mat draw_armor = bgr_img.clone();
  int i = 0;
  for (const auto & armor : armors) {
    // 熟悉的绘图函数
    tools::draw_points(draw_armor, armor.points);
    tools::draw_text(draw_armor, fmt::format("{}{:.2f}", armor.class_name, armor.confidence), armor.left.top);
    //solvepnp获取角度
    Solvepnp solvepnp(armor, 135, 56);//mm
    tools::draw_text(draw_armor, fmt::format("roll:{:.2f} yaw:{:.2f} pitch:{:.2f}", solvepnp.euler_angles[0],solvepnp.euler_angles[1],solvepnp.euler_angles[2]), armor.left.bottom);
    // tools::draw_text(draw_armor, fmt::format("x:{:.2f} y:{:.2f} z:{:.2f}", solvepnp.tvec[0],solvepnp.tvec[1],solvepnp.tvec[2]), armor.right.bottom);
    //getv获取速度
    cv::Vec3d vector=solvepnp.tvec;
    // std::cout<<vector[0]<<' '<<vector[1]<<' '<<vector[2]<<std::endl;
    static Getv getv;
    if(getv.Getcamerapoint(vector)&&getv.angle_radians)//true
    {
      fmt::print("angle:{:.2f}\ntime:{:.2f}\nRPM:{:6f}\nangle_v:{:6f}rad/s\n\n",getv.angle_radians,durationInSeconds,getv.RPM,getv.angle_v);
    }
    tools::draw_text(draw_armor, fmt::format("angle_v:{:.6f}rad/s   RPM:{:6f}", getv.angle_v,getv.RPM), cv::Point(50,800));
    i++;
  }
  cv::imshow("draw_armor", draw_armor);

  return armors;
}



bool Detector::check_geometry(const Lightbar & lightbar)
{
  auto angle_ok = std::abs(lightbar.angle - CV_PI / 2) < CV_PI / 4;
  //  auto angle_ok = std::abs(lightbar.angle - CV_PI / 2) < CV_PI / 2;
  auto ratio_ok = lightbar.ratio > 2 && lightbar.ratio < 15;
  // auto ratio_ok = lightbar.ratio > 2 && lightbar.ratio < 20;
  return angle_ok && ratio_ok;
}

// 待完成: 检查装甲板几何特征的合理性，防止误识别！
// 输入armor对象，返回一个bool判断值，可参考上面灯条代码规范
bool Detector::check_geometry(const Armor & armor)
{
  auto angle_ok_1=std::fabs(armor.angle1-CV_PI/2)<CV_PI/10;
  auto angle_ok_2=std::fabs(armor.angle2-CV_PI/2)<CV_PI/10;
  auto angle_ok_3=std::fabs(armor.angle3)<CV_PI/10;
  auto ratio_ok_1=armor.ratio1>1 &&armor.ratio1<5;// 2.3  4
  auto ratio_ok_2=armor.ratio2<0.6;//(you-zuo)/you
  return angle_ok_1 && angle_ok_2&& ratio_ok_1 && ratio_ok_2&& angle_ok_3;  // 需要修改这个函数&& angle_ok_3;
}


//下节课会用到
bool Detector::check_class(const Armor & armor)
{
  // auto name_ok = armor.class_name != "no_pattern";
  auto name_ok = armor.class_name == "small_outpost";
  auto confidence_ok = armor.confidence > 0.8;
  return name_ok && confidence_ok;
}

std::string Detector::get_color(
  const cv::Mat & bgr_img, const std::vector<cv::Point> & contour,
  const cv::RotatedRect & rotated_rect)
{
  auto rect = rotated_rect.boundingRect();
  int start_x = rect.x < 0 ? 0 : rect.x;
  int end_x = rect.x + rect.width > bgr_img.cols ? bgr_img.cols : rect.x + rect.width;
  int start_y = rect.y < 0 ? 0 : rect.y;
  int end_y = rect.y + rect.height > bgr_img.rows ? bgr_img.rows : rect.y + rect.height;

  int red_sum = 0, blue_sum = 0;

  for (int i = start_y; i < end_y; i++) {
    for (int j = start_x; j < end_x; j++) {
      if (cv::pointPolygonTest(contour, cv::Point2f(j, i), false) < 0) continue;
      red_sum += bgr_img.at<cv::Vec3b>(i, j)[2];
      blue_sum += bgr_img.at<cv::Vec3b>(i, j)[0];
    }
  }

  return blue_sum > red_sum ? "blue" : "red";
}

void Detector::set_pattern(Armor & armor, const cv::Mat & gray_img)
{
  // 获取装甲板图案四个角点，变换前
  auto top_left = armor.left.center - armor.left.delta * 0.9;
  auto bottom_left = armor.left.center + armor.left.delta * 0.9;
  auto top_right = armor.right.center - armor.right.delta * 0.9;
  auto bottom_right = armor.right.center + armor.right.delta * 0.9;
  std::vector<cv::Point2f> from_points = {top_left, top_right, bottom_right, bottom_left};

  // 获取装甲板图案四个角点，变换后，裁剪前
  int margin = 50;
  int h = 100;
  int w = 100 + margin * 2;
  std::vector<cv::Point2f> to_points = {
    cv::Point2f(0, 0), cv::Point2f(w, 0), cv::Point2f(w, h), cv::Point2f(0, h)};
  
  // 进行透视变换
  auto transform = cv::getPerspectiveTransform(from_points, to_points);
  cv::warpPerspective(gray_img, armor.pattern, transform, cv::Size(w, h));

  // 裁剪两侧灯条，因为灯条亮度高
  armor.pattern = armor.pattern(cv::Range(0, h), cv::Range(margin, w - margin));

  // 用高斯模糊+大津法提取图案  // return ???
  cv::GaussianBlur(armor.pattern, armor.pattern, cv::Size(5, 5), 0);
  cv::threshold(armor.pattern, armor.pattern, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}
}  // namespace auto_aim
