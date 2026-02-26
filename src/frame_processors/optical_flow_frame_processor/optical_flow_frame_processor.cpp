#include "optical_flow_frame_processor.h"
#include <opencv2/highgui.hpp>
#include "logger/logger.h"
void OpticalFlowFrameProcessor::processFrame(Mat &currentFrame)
{
  if (previousFrame.empty())
  {
    previousFrame = currentFrame.clone();
    return;
  }

  Mat currentGray;
  Mat previousGray;
  cvtColor(currentFrame, currentGray, COLOR_BGR2GRAY);
  cvtColor(previousFrame, previousGray, COLOR_BGR2GRAY);
  if (!previousGray.empty())
  {
    // Initialize parameters for the optical flow algorithm
    float pyrScale = 0.5;
    int numLevels = 3;
    int windowSize = 15;
    int numIterations = 3;
    int neighborhoodSize = 5;
    float stdDeviation = 1.2;
    Mat flowImage(previousGray.size(), CV_32FC2);
    // Calculate optical flow map using Farneback algorithm
    calcOpticalFlowFarneback(previousGray, currentGray, flowImage, pyrScale,
                             numLevels, windowSize, numIterations,
                             neighborhoodSize, stdDeviation, 0);
    Mat result = previousFrame.clone();

    vector<Rect> rects = findRects(flowImage);

    // Draw the optical flow map
    if (need2DrawOpticalFlow)
    {
      drawOpticalFlowOnResult(flowImage, result);
    }
    if (need2DrawRectangles)
    {
      drawRectanglesOnResult(rects, result);
    }
    previousFrame = currentFrame.clone();
    Logger.trace("Fire frame processed(optical flow processor): " + rects.size());
    fireFrameProcessed(result, rects);
  }
}

void OpticalFlowFrameProcessor::drawOpticalFlowOnResult(const Mat &flowImage, Mat &result)
{
  int stepSize = 16;
  Scalar color = Scalar(0, 255, 0);

  // Draw the uniform grid of points on the input image along with the motion
  // vectors
  for (int y = 0; y < result.rows; y += stepSize)
  {
    for (int x = 0; x < result.cols; x += stepSize)
    {
      // Circles to indicate the uniform grid of points
      int radius = 2;
      int thickness = -1;
      circle(result, Point(x, y), radius, color, thickness);

      // Lines to indicate the motion vectors
      Point2f pt = flowImage.at<Point2f>(y, x);
      line(result, Point(x, y), Point(cvRound(x + pt.x), cvRound(y + pt.y)),
           color);
    }
  }
}

vector<Rect> OpticalFlowFrameProcessor::findRects(const Mat &flowImage)
{
  vector<Rect> rects;
  double threshold = 4.0;
  cv::Mat magnitude, angle;
  std::vector<cv::Mat> flow_channels;
  cv::split(flowImage, flow_channels);
  cv::cartToPolar(flow_channels[0], flow_channels[1], magnitude, angle);
  cv::Mat motion_mask = magnitude > threshold;
  cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(40, 40));
  cv::morphologyEx(motion_mask, motion_mask, cv::MORPH_CLOSE, structuringElement);
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(motion_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  // Рисуем ограничивающие прямоугольники
  for (size_t i = 0; i < contours.size(); i++)
  {
    if (cv::contourArea(contours[i]) > 1000)
    { // Игнорируем маленькие контуры
      cv::Rect bounding_box = cv::boundingRect(contours[i]);
      rects.push_back(bounding_box);
    }
  }
  return rects;
}

void OpticalFlowFrameProcessor::drawRectanglesOnResult(vector<Rect> &rects, Mat &result)
{
  // Рисуем ограничивающие прямоугольники
  for (size_t i = 0; i < rects.size(); i++)
  {
    if (rects[i].area() > 1000)
    { // Игнорируем маленькие контуры
      cv::Rect bounding_box = rects[i];
      cv::rectangle(result, bounding_box, cv::Scalar(0, 255, 0), 2);
    }
  }

  // cvtColor(motion_mask,result,COLOR_GRAY2BGR);
}


