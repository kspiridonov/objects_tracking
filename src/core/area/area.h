#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
class Area
{
public:
    Area(Rect rect) : rect(rect) {}
    int getSquare();
    Area getIntercection(const Area &otherArea);
    int getIntercectionSquare(const Area &otherArea);
private:
    Rect rect;
protected:
    Rect getRect() const{
        return rect;
    }    
};