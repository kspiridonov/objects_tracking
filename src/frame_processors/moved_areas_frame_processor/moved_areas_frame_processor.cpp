#include "moved_areas_frame_processor.h"

MovedAreasFrameProcessor::MovedAreasFrameProcessor() : FrameProcessorListener<Rect>("MovedAreasFrameProcessor")
{
    opticalFlowFrameProcessor.addListener(this);
}

MovedAreasFrameProcessor::~MovedAreasFrameProcessor()
{
    opticalFlowFrameProcessor.removeListener(this);
}

void MovedAreasFrameProcessor::frameProcessed(Mat &frame, vector<Rect> list)
{
    vector<Area> areas;
    for (auto const &entry : list)
    {
        areas.push_back(Area(entry));
    }
    fireFrameProcessed(frame, areas);
}

void MovedAreasFrameProcessor::processFrame(Mat &currentFrame)
{
    opticalFlowFrameProcessor.processFrame(currentFrame);
}
