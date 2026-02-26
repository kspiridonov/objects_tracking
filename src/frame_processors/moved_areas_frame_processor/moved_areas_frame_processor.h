#pragma once
#include "frame_processors/optical_flow_frame_processor/optical_flow_frame_processor.h"
#include "frame_processors/base_frame_processor/base_frame_processor.h"
#include "frame_processors/base_frame_processor/frame_processor_listener.h"
#include "core/area/area.h"
class MovedAreasFrameProcessor: public BaseFrameProcessor<Area>, public FrameProcessorListener<Rect>
{
public:
    MovedAreasFrameProcessor();
    void processFrame(Mat &currentFrame) override;
    void frameProcessed(Mat &frame, vector<Rect> list) override;
    ~MovedAreasFrameProcessor();

private:
    OpticalFlowFrameProcessor opticalFlowFrameProcessor = OpticalFlowFrameProcessor(false, false);
};
