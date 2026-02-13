#include "ObjectDetector.h"
#include <fstream>
#include <random>
ObjectDetector::ObjectDetector() {
  initModel();
  loadClassesFromFile();
}

std::vector<Detection> ObjectDetector::detect(const cv::Mat &input) {
  if (debounceCounter == 0) {
    cv::Mat clonedInput = input.clone();
    clonedInput = formatToSquare(clonedInput);
    cv::Mat blob;
    cv::dnn::blobFromImage(clonedInput, blob, 1.0 / 255.0, modelInputShape,
                           cv::Scalar(), true, false);
    net.setInput(blob);
    std::vector<cv::Mat> outputs;
    std::vector<double> layersTimes;
    double freq = cv::getTickFrequency() / 1000;
    net.forward(outputs, net.getUnconnectedOutLayersNames());
    double t = net.getPerfProfile(layersTimes) / freq;
    std::cout << "time inference: " << t << std::endl;
    cv::Mat output = postProcessNetOutput2Mat(outputs[0]);
    BoxesInfo boxesInfo = getAllBoxesInfoFromNetOutput(clonedInput, output);
    savedDetections = filterBoxesNMS(boxesInfo);
    debounceCounter++;
  } else {
    debounceCounter++;
    if (debounceCounter == 3)
      debounceCounter = 0;
  }
  return savedDetections;
}

cv::Mat ObjectDetector::detectAndDraw(const cv::Mat &input) {
  cv::Mat result = input.clone();
  std::vector<Detection> detections = detect(result);
  for (int i = 0; i < detections.size(); ++i) {
    Detection detection = detections[i];

    cv::Rect box = detection.getBox();
    cv::Scalar color = detection.getColor();

    // Detection box
    cv::rectangle(result, box, color, 2);

    // Detection box text
    std::string classString =
        detection.getClassName() + ' ' +
        std::to_string(detection.getConfidence()).substr(0, 4);
    cv::Size textSize =
        cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
    cv::Rect textBox(box.x, box.y - 40, textSize.width + 10,
                     textSize.height + 20);

    cv::rectangle(result, textBox, color, cv::FILLED);
    cv::putText(result, classString, cv::Point(box.x + 5, box.y - 10),
                cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
  }
  return result;
}

BoxesInfo ObjectDetector::getAllBoxesInfoFromNetOutput(cv::Mat &input,
                                                       cv::Mat &output) {
  std::vector<int> classIds;
  std::vector<float> confidences;
  std::vector<cv::Rect> boxes;
  int rows = output.size[0];
  int cols = output.size[1];
  float *data = (float *)output.data;

  float xFactor = input.cols / modelInputShape.width;
  float yFactor = input.rows / modelInputShape.height;
  for (int i = 0; i < rows; ++i) {
    float *classesScores = data + 4;

    cv::Mat scores(1, classes.size(), CV_32FC1, classesScores);
    cv::Point classId;
    double maxClassScore;

    minMaxLoc(scores, 0, &maxClassScore, 0, &classId);

    if (maxClassScore > modelScoreThreshold) {
      confidences.push_back(maxClassScore);
      classIds.push_back(classId.x);

      float x = data[0];
      float y = data[1];
      float w = data[2];
      float h = data[3];

      int left = int((x - 0.5 * w) * xFactor);
      int top = int((y - 0.5 * h) * yFactor);

      int width = int(w * xFactor);
      int height = int(h * yFactor);

      boxes.push_back(cv::Rect(left, top, width, height));
    }
    data += cols;
  }
  return BoxesInfo{classIds, confidences, boxes};
}

std::vector<Detection>
ObjectDetector::filterBoxesNMS(const BoxesInfo &boxesInfo) {
  std::vector<Detection> detections;
  std::vector<int> nmsResult;
  cv::dnn::NMSBoxes(boxesInfo.boxes, boxesInfo.confidences, modelScoreThreshold,
                    modelNMSThreshold, nmsResult);
  for (unsigned long i = 0; i < nmsResult.size(); ++i) {
    int idx = nmsResult[i];

    auto classId = boxesInfo.classIds[idx];
    auto confidence = boxesInfo.confidences[idx];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(100, 255);
    auto color = cv::Scalar(dis(gen), dis(gen), dis(gen));

    auto className = classes[classId];
    auto box = boxesInfo.boxes[idx];

    detections.push_back(Detection(classId, className, confidence, color, box));
  }
  return detections;
}

cv::Mat ObjectDetector::formatToSquare(const cv::Mat &source) {
  int col = source.cols;
  int row = source.rows;
  int _max = MAX(col, row);
  cv::Mat result = cv::Mat::zeros(_max, _max, CV_8UC3);
  source.copyTo(result(cv::Rect(0, 0, col, row)));
  return result;
}

void ObjectDetector::initModel() {
  net = cv::dnn::readNetFromONNX(onnxModelPath);
  net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
  net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
}

void ObjectDetector::loadClassesFromFile() {
  std::ifstream inputFile(classesTxtFile);
  if (inputFile.is_open()) {
    std::string classLine;
    while (std::getline(inputFile, classLine))
      classes.push_back(classLine);
    inputFile.close();
  }
}

cv::Mat ObjectDetector::postProcessNetOutput2Mat(cv::Mat &output) {
  // yolov8 has an output of shape (batchSize, 84,  8400) (Num classes +
  // box[x,y,w,h])
  int dimensions = output.size[1];

  output = output.reshape(1, dimensions);
  cv::transpose(output, output);
  return output;
}
