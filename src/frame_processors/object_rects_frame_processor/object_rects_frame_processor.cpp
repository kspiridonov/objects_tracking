//
// Created by ksp on 12.03.2026.
//
#include "object_rects_frame_processor.h"

ObjectRectsFrameProcessor::ObjectRectsFrameProcessor() : FrameProcessorListener<MovementRect>(
    "ObjectRectsFrameProcessor") {
    movementRectsFrameProcessor.addListener(this);
}

void ObjectRectsFrameProcessor::processFrame(Mat &currentFrame) {
    movementRectsFrameProcessor.processFrame(currentFrame);
}

void ObjectRectsFrameProcessor::frameProcessed(Mat &frame, const vector<MovementRect> &mRects) {
    map<string, MovementRect> processedRectsById;
    for (auto &mRect: mRects) {
        if (auto it = objectRectByMRectIds.find(mRect.getId());
            it == objectRectByMRectIds.end() || it->second.getNumDetections() == 0 && it->second.getNumOfTry() < 5) {
            vector<DetectionRect> detections;
            bool inRect = false;
            if (mRect.inRect(Rect(0, 0, frame.cols, frame.rows), 30)) {
                inRect = true;
                Mat currentFrame;
                //auto roiFrame = currentFrame(mRect.getRect());

                Mat mask = Mat::zeros(frame.size(), CV_8UC1);
                rectangle(mask, mRect.getRect(), Scalar(255), FILLED);
                frame.copyTo(currentFrame, mask);
                detections = objectDetector.detect(currentFrame);
                //frame = objectDetector.detectAndDraw(currentFrame);
                for (auto detectIt = detections.begin(); detectIt != detections.end();) {
                    if (detectIt->getConfidence() < 0.5) {
                        detectIt = detections.erase(detectIt);
                    } else {
                        ++detectIt;
                    }
                }
            }
            if (it == objectRectByMRectIds.end()) {
                ObjectRect oRect = ObjectRect(mRect, detections);
                objectRectByMRectIds.insert_or_assign(mRect.getId(), oRect);
            } else {
                it->second.setMovementRect(mRect);
                it->second.setDetections(detections);
                if (inRect) { it->second.incNuOfTry(); }
            }
        } else {
            it->second.setMovementRect(mRect);
        }
        processedRectsById.insert_or_assign(mRect.getId(), mRect);
    }
    for (auto it = objectRectByMRectIds.begin(); it != objectRectByMRectIds.end();) {
        auto mRectId = it->first;
        auto oRect = it->second;
        auto itProcessedMRects = processedRectsById.find(mRectId);
        if (itProcessedMRects == processedRectsById.end()) {
            Logger.debug("Remove object rect: " + oRect.toString());
            it = objectRectByMRectIds.erase(it);
        } else { ++it; }
    }
    vector<ObjectRect> objectRects;
    for (auto &[id,oRect]: objectRectByMRectIds) {
        objectRects.push_back(oRect);
    }
    fireFrameProcessed(frame, objectRects);
}

ObjectRectsFrameProcessor::~ObjectRectsFrameProcessor() {
    movementRectsFrameProcessor.removeListener(this);
}
