#include "OpticalFlow.h"
Mat OpticalFlow::calcOpticaFlow(Mat &currentFrame) {
  if (previousFrame.empty()) {
    previousFrame = currentFrame.clone();
    return currentFrame;
  }

  return currentFrame;
}

void OpticalFlow::drawOpticalFlow(const Mat &flowImage, Mat &flowImageGray) {
  int stepSize = 16;
  Scalar color = Scalar(0, 255, 0);

  // Draw the uniform grid of points on the input image along with the motion
  // vectors
  for (int y = 0; y < flowImageGray.rows; y += stepSize) {
    for (int x = 0; x < flowImageGray.cols; x += stepSize) {
      // Circles to indicate the uniform grid of points
      int radius = 2;
      int thickness = -1;
      circle(flowImageGray, Point(x, y), radius, color, thickness);

      // Lines to indicate the motion vectors
      Point2f pt = flowImage.at<Point2f>(y, x);
      line(flowImageGray, Point(x, y),
           Point(cvRound(x + pt.x), cvRound(y + pt.y)), color);
    }
  }
}
