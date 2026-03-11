//
// Created by ksp on 05.03.2026.
//
#pragma once
#include "core/rect/base_rect.h"


class OpticalFlowRect : public BaseRect {
public:
    OpticalFlowRect(Rect rect, Rect previousRect) : BaseRect(rect) {
        this->previousRect = previousRect;
    }
    string toString() const override {
        return "OpticalFlowRect: id: " + id
               + ", rect: (x: " + to_string(rect.x) + ", y: " + to_string(rect.y) + ", w: " + to_string(rect.width) +
               ", h: " + to_string(rect.height);
    }
public:
    Rect getPreviousRect() const { return previousRect; }

private:
    Rect previousRect;
};
