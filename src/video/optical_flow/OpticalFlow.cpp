#include "OpticalFlow.h"
Mat OpticalFlow::calcOpticaFlow(Mat &currentFrame) {
  if (previousFrame.empty()) {
    previousFrame = currentFrame.clone();
    return currentFrame;
  }

  Mat currentGray;
  Mat previousGray;
  cvtColor(currentFrame, currentGray, COLOR_BGR2GRAY);
  cvtColor(previousFrame, previousGray, COLOR_BGR2GRAY);
  if (!previousGray.empty()) {
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

    // Draw the optical flow map
    drawOpticalFlow(flowImage, result);
    previousFrame = currentFrame.clone();
    return result;
  }
  return currentFrame;
}

void OpticalFlow::drawOpticalFlow(const Mat &flowImage, Mat &result) {
  int stepSize = 16;
  Scalar color = Scalar(0, 255, 0);

  // Draw the uniform grid of points on the input image along with the motion
  // vectors
  for (int y = 0; y < result.rows; y += stepSize) {
    for (int x = 0; x < result.cols; x += stepSize) {
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
