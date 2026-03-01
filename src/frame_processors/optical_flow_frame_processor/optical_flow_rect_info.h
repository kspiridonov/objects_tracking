#pragma once
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class OpticalFlowRectInfo {
public:
    OpticalFlowRectInfo(Rect currentRect, Rect movedToPreviousPositionRect):currentRect(currentRect),movedToPreviousPositionRect(movedToPreviousPositionRect){};
private:
    Rect currentRect;

public:
     [[nodiscard]] Rect getCurrentRect() const {
        return currentRect;
    }

    [[nodiscard]] Rect getMovedToPreviousPositionRect() const {
        return movedToPreviousPositionRect;
    }

private:
    Rect movedToPreviousPositionRect;
};
