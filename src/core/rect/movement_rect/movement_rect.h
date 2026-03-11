#pragma once
#include <opencv2/opencv.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/generators.hpp>
#include <magic_enum/magic_enum.hpp>
#include "core/rect/base_rect.h"
using namespace cv;
using namespace std;

enum class MovementRectState { ADDING, ADDED, SPLITTING, SPLITTED, REMOVING, REMOVED };

class MovementRect : public BaseRect {
public:
    explicit MovementRect(Rect rect) : BaseRect(rect) {
        state = MovementRectState::ADDING;
    }

    string toString() const override {
        return "MovementRect: id: " + id
               + ", state: " + std::string(magic_enum::enum_name(state))
               + ", rect: (x: " + to_string(rect.x) + ", y: " + to_string(rect.y) + ", w: " + to_string(rect.width) +
               ", h: " + to_string(rect.height) + ", debounce: " + to_string(debounce);
    }

    void setRect(Rect rect) override;

    void incDebounce();

    MovementRectState getState() const {
        return state;
    }

    void setState(MovementRectState state) {
        debounce = 0;
        this->state = state;
    }

    set<string> getParentIds() const {
        return parentIds;
    }

    void addParentId(const string &parentId) {
        this->parentIds.insert(parentId);
    }

private:
    int debounce = 0;
    constexpr static int MAX_ADDING_DEBOUNCE = 4;
    constexpr static int MAX_REMOVING_DEBOUNCE = 4;
    constexpr static int MAX_SPLITTING_DEBOUNCE = 4;
    MovementRectState state = MovementRectState::ADDING;
    set<string> parentIds;
};
