#include "classifier.hpp"
// #include <fmt/format.h>
#include "img_tools.hpp"

// #include <string>
namespace auto_aim//class_name=???,confidence==???
{
    Classifier::Classifier(std::string model)
    {
        error = false;
        net = cv::dnn::readNet(model);
    }

    void Classifier::classify(Armor &armor)
    {
        // Process frames.
        cv::Mat frame, blob;
        frame=armor.pattern.clone();
        if (frame.empty())
        {
            error = true;
            return;
        }
        // 预处理输入图像：
        // resize(frame, frame, cv::Size(rszWidth, rszHeight));//???
        frame=frame/255;//???
        // Create a 4D blob from a frame
        cv::dnn::blobFromImage(frame, blob, scale, cv::Size(inpWidth, inpHeight));
        // Set the input blob for the neural network
        net.setInput(blob);
        // Forward pass to get predictions
        cv::Mat outputs = net.forward();
        //Softmax
        float max=*std::max_element(outputs.begin<float>(), outputs.end<float>());
        cv::exp(outputs-max, outputs);
        float sum = (float)cv::sum(outputs)[0];
        outputs/=sum;
        // Find the class with the highest confidence
        cv::Point classIdPoint;
        double confidence;
        minMaxLoc(outputs.reshape(1, 1), 0, &confidence, 0, &classIdPoint);
        int classId = classIdPoint.x;
        armor.class_name = CLASS_NAMES[classId];
        armor.confidence=confidence;
    }

} // namespace auto_aim