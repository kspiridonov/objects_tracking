#pragma once
#include "../detection_rect/detection_rect_type.h"
#include "core/rect/base_rect.h"
#include "core/rect/detection_rect/detection_rect.h"
#include "core/rect/movement_rect/movement_rect.h"
#include "utils/enum_util.h"
#include "logger/logger.h"

class ObjectRect : public BaseRect {
public:
    ObjectRect(const MovementRect &movementRect, const vector<DetectionRect> &detections)
        : BaseRect(movementRect.getRect()), movementRect(movementRect), detections(detections) {
    }

    [[nodiscard]] DetectionRectType getType() const;

    [[nodiscard]] size_t getNumDetections() const {
        return detections.size();
    }


    void setMovementRect(const MovementRect &movementRect) {
        this->movementRect = movementRect;
        this->rect = movementRect.getRect();
    }

    void setDetections(const vector<DetectionRect> &detections) {
        this->detections = detections;
    }

    int getNumOfTry() const {
        return numOfTry;
    }

    void incNuOfTry() {
        ++numOfTry;
    }

private:
    MovementRect movementRect;
    vector<DetectionRect> detections;
    int numOfTry = 0;
};
