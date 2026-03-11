#pragma once
#include <opencv2/opencv.hpp>
#include "frame_processor_listener.h"
#include <map>
#include "logger/logger.h"
using namespace std;
using namespace cv;
template <typename T>
class BaseFrameProcessor
{

public:
    virtual void processFrame(Mat &currentFrame) = 0;
    void fireFrameProcessed(Mat &currentFrame, const vector<T> & list)
    {
        for (auto const &entry : listeners)
        {
            entry.second->frameProcessed(currentFrame, list);
        }
    }
    void addListener(FrameProcessorListener<T> *listener)
    {
        auto id = listener->getId();
        if (listeners.find(id) != listeners.end())
        {
            throw ListenerAlreadyExists("Listener with id: " + id + " already exists");
        }
        listeners[id] = listener;
    }
    void removeListener(FrameProcessorListener<T> *listener)
    {
        auto id = listener->getId();
        auto it = listeners.find(id);
        if (it != listeners.end())
        {
            listeners.erase(it);
        }
    }

private:
    map<string, FrameProcessorListener<T> *> listeners;
};