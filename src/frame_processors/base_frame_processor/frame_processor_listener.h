#pragma once
#include <string>
#include <map>
#include <opencv2/opencv.hpp>
#include "core/exception/listener_already_exists_exception.h"
using namespace std;
using namespace cv;
template <typename T>
class FrameProcessorListener
{
public:
    FrameProcessorListener(std::string id) : id(id) {}
    virtual std::string getId() { return id; };
    virtual void frameProcessed(Mat &frame, vector<T> list) = 0;
    
private:
    string id;
};