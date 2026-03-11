//
// Created by ksp on 05.03.2026.
//
#pragma once
#include <opencv2/opencv.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/generators.hpp>
using namespace cv;
using namespace std;

class BaseRect {

public:
    explicit BaseRect(Rect rect) : rect(rect) {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        id = boost::uuids::to_string(uuid);
    }

    bool operator<(const BaseRect &other) const {
        return id < other.id;
    }

    bool operator==(const BaseRect &other) const {
        return id == other.id;
    }


    int getSquare() const {
        return rect.area();
    }

    Rect getInterception(const BaseRect &otherBaseRect) const {
        auto otherRect = otherBaseRect.getRect();
        auto intersection = rect & otherRect;
        return intersection;
    }

    int getIntersectionSquare(const BaseRect &otherBaseRect) const {
        auto otherRect = otherBaseRect.getRect();
        auto intersection = rect & otherRect;
        return intersection.area();
    }

    int getIntersectionSquare(const Rect &otherRect) const {
        auto intersection = rect & otherRect;
        return intersection.area();
    }

    Rect getRect() const {
        return rect;
    }

    string getId() const { return id; }

    virtual void setRect(Rect rect) { this->rect = rect; }

    virtual string toString() const {
        std::ostringstream ss;
        ss << "BaseRect: " << this->id;
        return ss.str();
    }

protected:
    Rect rect;
    string id;
};
