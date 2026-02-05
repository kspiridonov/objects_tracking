#ifndef DETECTION_H
#define DETECTION_H
#include <opencv2/opencv.hpp>
#include <string>
class Detection {
public:
  Detection(int classId, std::string className, float confidence,
            cv::Scalar color, cv::Rect box)
      : classId(classId), className(className), confidence(confidence),
        color(color), box(box) {}

public:
  int getClassId() const;
  std::string getClassName() const;
  float getConfidence() const;
  cv::Scalar getColor() const;
  cv::Rect getBox() const;

private:
  int classId = 0;
  std::string className;
  float confidence = 0.0;
  cv::Scalar color;
  cv::Rect box;
};

inline int Detection::getClassId() const { return classId; }

inline std::string Detection::getClassName() const { return className; }

inline float Detection::getConfidence() const { return confidence; }

inline cv::Scalar Detection::getColor() const { return color; }

inline cv::Rect Detection::getBox() const { return box; }
#endif // DETECTION_H
