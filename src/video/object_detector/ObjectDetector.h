#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H
#include "video/object_detector/Detection.h"
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
// see https://github.com/JustasBart/yolov8_CPP_Inference_OpenCV_ONNX.git
struct BoxesInfo {
  std::vector<int> classIds;
  std::vector<float> confidences;
  std::vector<cv::Rect> boxes;
};
class ObjectDetector {
public:
  ObjectDetector();
  std::vector<Detection> detect(const cv::Mat &input);
  cv::Mat detectAndDraw(const cv::Mat &input);

protected:
  cv::Mat formatToSquare(const cv::Mat &source);
  void initModel();
  void loadClassesFromFile();
  cv::Mat postProcessNetOutput2Mat(cv::Mat &output);
  BoxesInfo getAllBoxesInfoFromNetOutput(cv::Mat &input, cv::Mat &output);
  std::vector<Detection> filterBoxesNMS(const BoxesInfo &boxesInfo);

private:
  std::vector<std::string> classes;
  std::string onnxModelPath = "res/models/yolov8s.onnx";
  cv::Size2f modelInputShape = cv::Size(640, 480);
  std::string classesTxtFile = "res/classes/classes.txt";

private:
  float modelConfidenseThreshold{0.25};
  float modelScoreThreshold{0.45};
  float modelNMSThreshold{0.50};
  cv::dnn::Net net;
  int debounceCounter = 0;
  std::vector<Detection> savedDetections;
};

#endif // OBJECTDETECTOR_H
