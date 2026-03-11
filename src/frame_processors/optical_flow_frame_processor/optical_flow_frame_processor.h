#ifndef OPTICALFLOW_H
#define OPTICALFLOW_H
#include "core/rect/optical_flow_rect/optical_flow_rect.h"
#include "frame_processors/base_frame_processor/base_frame_processor.h"
using namespace cv;
using namespace std;

class OpticalFlowFrameProcessor : public BaseFrameProcessor<OpticalFlowRect> {
public:
    explicit OpticalFlowFrameProcessor(const bool debug) : showDebugInfo(debug) {
    }

    void processFrame(Mat &currentFrame) override;

protected:
    vector<Rect> findRects(const Mat &flowImage);

    void drawOpticalFlowOnResult(const Mat &flowImage, Mat &result, int *vectorX, int *vectorY);

    void drawRectanglesOnResult(vector<Rect> &rects, Mat &result, Scalar color);

    vector<OpticalFlowRect> getOpticalFlowRects(Mat &frame, const Mat &flowImage, const vector<Rect> &rects);

private:
    bool showDebugInfo = false;
    Mat previousFrame;
};

#endif // OPTICALFLOW_H
