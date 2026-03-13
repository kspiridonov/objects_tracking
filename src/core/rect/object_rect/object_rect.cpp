//
// Created by ksp on 12.03.2026.
//

#include "object_rect.h"

DetectionRectType ObjectRect::getType() const {
    if (getNumDetections() == 1) {
        return detections[0].getType();
    } else if (getNumDetections() == 0) {
        return DetectionRectType::UNKNOWN;
    } else {
        return DetectionRectType::GROUP;
    }
}
