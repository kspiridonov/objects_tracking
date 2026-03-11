#pragma once
#include "core/rect/movement_rect/movement_rect.h"
#include "frame_processors/optical_flow_frame_processor/optical_flow_frame_processor.h"
#include "frame_processors/base_frame_processor/base_frame_processor.h"
#include "frame_processors/base_frame_processor/frame_processor_listener.h"

struct MovementRectChanges {
    set<MovementRect> forRemove;
    set<OpticalFlowRect> forAdd;
    map<OpticalFlowRect, set<MovementRect> > forMerge;
    map<MovementRect, set<OpticalFlowRect> > forSplit;
    map<MovementRect, OpticalFlowRect> forUpdate;
};

class MovementRectsFrameProcessor : public BaseFrameProcessor<MovementRect>,
                                    public FrameProcessorListener<OpticalFlowRect> {
public:
    MovementRectsFrameProcessor();

    void processFrame(Mat &currentFrame) override;

    void frameProcessed(Mat &frame, const vector<OpticalFlowRect> &list) override;

    ~MovementRectsFrameProcessor();

protected:
    MovementRectChanges getChanges(const vector<OpticalFlowRect> &oRects);

    set<OpticalFlowRect> findForAdd(map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
                                    map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect);

    set<MovementRect> findForRemove(map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
                                    map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect,
                                    set<OpticalFlowRect> &forAdd);

    map<OpticalFlowRect, set<MovementRect> > findForMerge(
        map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
        map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect);

    map<MovementRect, set<OpticalFlowRect> > findForSplit(
        map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
        map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect);

    map<MovementRect, OpticalFlowRect> findForUpdate(
        map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
        map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect);

    map<OpticalFlowRect, shared_ptr<MovementRect> > findLinksForORectsWithMRects(const vector<OpticalFlowRect> &oRects);

    shared_ptr<MovementRect> findLinkedMRect(const OpticalFlowRect &rect);

    map<MovementRect, shared_ptr<OpticalFlowRect> > findLinksForMRectsWithORects(const vector<OpticalFlowRect> &oRects);

    shared_ptr<OpticalFlowRect> findLinkedORect(const MovementRect &rect, const vector<OpticalFlowRect> &oRects);

    shared_ptr<MovementRect> isORectPresentIn(const OpticalFlowRect &oRect,
                                              map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect);

    shared_ptr<OpticalFlowRect> isMRectPresentIn(const MovementRect &mRect,
                                                 map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect);

    void processChanges(const MovementRectChanges &changes);

protected:
    OpticalFlowFrameProcessor opticalFlowFrameProcessor = OpticalFlowFrameProcessor(false);
    set<MovementRect> mRects;
};
