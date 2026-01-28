#ifndef OPTICALFLOW_H
#define OPTICALFLOW_H
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
using namespace cv;
using namespace std;
class OpticalFlow {
public:
  Mat calcOpticaFlow(Mat &currentFrame);

protected:
  // Function to compute the optical flow map
  void drawOpticalFlow(const Mat &flowImage, Mat &flowImageGray);

private:
  Mat previousFrame;
};

#endif // OPTICALFLOW_H
