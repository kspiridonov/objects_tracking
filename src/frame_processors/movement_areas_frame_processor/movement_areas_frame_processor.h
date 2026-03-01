#pragma once
#include "frame_processors/optical_flow_frame_processor/optical_flow_frame_processor.h"
#include "frame_processors/base_frame_processor/base_frame_processor.h"
#include "frame_processors/base_frame_processor/frame_processor_listener.h"
#include "core/movement_area/movement_area.h"
class MovementAreasFrameProcessor: public BaseFrameProcessor<MovementArea>, public FrameProcessorListener<OpticalFlowRectInfo>
{
public:
    MovementAreasFrameProcessor();
    void processFrame(Mat &currentFrame) override;
    void frameProcessed(Mat &frame, vector<OpticalFlowRectInfo> list) override;
    ~MovementAreasFrameProcessor();

private:
    OpticalFlowFrameProcessor opticalFlowFrameProcessor = OpticalFlowFrameProcessor(true);
};
