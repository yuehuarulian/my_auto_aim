#include <stdio.h>
#include <fmt/format.h>
#include "img_tools.hpp"
#include "detector.hpp"
#include "types.hpp"
#include "camera.hpp"
using namespace auto_aim;
using namespace cv;
using namespace std;
std::chrono::_V2::system_clock::time_point startTime, endTime;
double durationInSeconds;
bool time_again=0;
int main()
{
  cv::Mat img;
  MVCamera camera(1500);
  if (camera.error == true)
    return 0;
  Detector detector;
    startTime = std::chrono::high_resolution_clock::now();
  while (true)
  {
    img = camera.getFrame();
    // imshow("Display window", img);
    auto armors = detector.detect(img);
    if (cv::waitKey(25) == 27)
      break;
  }
  return 0;
}
