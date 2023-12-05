#ifndef AUTO_AIM__CLASSIFIER_HPP
#define AUTO_AIM__CLASSIFIER_HPP

#include "types.hpp"

namespace auto_aim
{
    class Classifier
    {
    public:
      bool error;
      Classifier(std::string model = "model/model.onnx");
      void classify(Armor & armor);
    private:
      cv::dnn::Net net;
      const int inpWidth = 50;
      const int inpHeight = 50;
      const int rszWidth = 50;
      const int rszHeight = 50;
      const double scale = 1.0;
      bool needSoftmax = true;
    };

}

#endif  // AUTO_AIM__CLASSIFIER_HPP