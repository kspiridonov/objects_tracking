#ifndef OPTICALFLOW_H
#define OPTICALFLOW_H
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "frame_processors/base_frame_processor/base_frame_processor.h"
using namespace cv;
using namespace std;
class OpticalFlowFrameProcessor: public BaseFrameProcessor<Rect>
{
public:
  OpticalFlowFrameProcessor(bool need2DrawOpticalFlow, bool need2DrawRectangles) : need2DrawOpticalFlow(need2DrawOpticalFlow), need2DrawRectangles(need2DrawRectangles) {}
  void processFrame(Mat &currentFrame) override;

protected:
  vector<Rect> findRects(const Mat &flowImage);
  void drawOpticalFlowOnResult(const Mat &flowImage, Mat &result);
  void drawRectanglesOnResult(vector<Rect> &rects, Mat &result);

private:
  bool need2DrawOpticalFlow;
  bool need2DrawRectangles;
  Mat previousFrame;
};

#endif // OPTICALFLOW_H
