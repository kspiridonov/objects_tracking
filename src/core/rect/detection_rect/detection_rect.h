#pragma once
#include "detection_rect_type.h"
#include "core/rect/base_rect.h"
#include "logger/logger.h"
#include "utils/enum_util.h"

class DetectionRect : public BaseRect {
public:
    DetectionRect(const Rect &rect, const string &classLabel, int classId,
                  float confidence) : BaseRect(rect), classLabel(classLabel), classId(classId), confidence(confidence) {
        try {
            type = EnumUtil::fromString<DetectionRectType>(classLabel);
        } catch (Exception &e) {
            Logger.warn("Undefined class label value: " + classLabel);
        }
    };


    [[nodiscard]] string getClassLabel() const {
        return classLabel;
    }

    [[nodiscard]] int getClassId() const {
        return classId;
    }

    [[nodiscard]] float getConfidence() const {
        return confidence;
    }

    [[nodiscard]] DetectionRectType getType() const {
        return type;
    }

private:
    string classLabel;
    int classId;
    float confidence;
    DetectionRectType type = DetectionRectType::UNKNOWN;
};
