#include "movement_areas_frame_processor.h"

MovementAreasFrameProcessor::MovementAreasFrameProcessor() : FrameProcessorListener<OpticalFlowRectInfo>("MovedAreasFrameProcessor")
{
    opticalFlowFrameProcessor.addListener(this);
}

MovementAreasFrameProcessor::~MovementAreasFrameProcessor()
{
    opticalFlowFrameProcessor.removeListener(this);
}

void MovementAreasFrameProcessor::frameProcessed(Mat &frame, vector<OpticalFlowRectInfo> list)
{
    vector<MovementArea> areas;
    for (auto const &entry : list)
    {
        areas.push_back(MovementArea(entry.getCurrentRect()));
    }
    fireFrameProcessed(frame, areas);
}

void MovementAreasFrameProcessor::processFrame(Mat &currentFrame)
{
    opticalFlowFrameProcessor.processFrame(currentFrame);
}
