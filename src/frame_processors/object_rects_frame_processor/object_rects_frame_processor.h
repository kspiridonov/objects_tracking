#pragma once
#include "core/rect/movement_rect/movement_rect.h"
#include "core/rect/object_rect/object_rect.h"
#include "frame_processors/base_frame_processor/base_frame_processor.h"
#include "frame_processors/base_frame_processor/frame_processor_listener.h"
#include "frame_processors/movement_rects_frame_processor/movement_rects_frame_processor.h"
#include "object_detector/ObjectDetector.h"

class ObjectRectsFrameProcessor : public BaseFrameProcessor<ObjectRect>,
                                  public FrameProcessorListener<MovementRect> {
public:
    ObjectRectsFrameProcessor();
    void processFrame(Mat &currentFrame) override;

    void frameProcessed(Mat &frame, const vector<MovementRect> &list) override;

    ~ObjectRectsFrameProcessor();
private:
    MovementRectsFrameProcessor movementRectsFrameProcessor;
    ObjectDetector objectDetector;
    map<string, ObjectRect> objectRectByMRectIds;
};
