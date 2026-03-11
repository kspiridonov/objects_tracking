#include "movement_rect.h"

void MovementRect::incDebounce() {
    if (state == MovementRectState::ADDED)return;
    debounce++;
    if (state == MovementRectState::ADDING && debounce >= MAX_ADDING_DEBOUNCE) {
        setState(MovementRectState::ADDED);
    } else if (state == MovementRectState::SPLITTING && debounce >= MAX_SPLITTING_DEBOUNCE) {
        setState(MovementRectState::SPLITTED);
    } else if (state == MovementRectState::REMOVING && debounce >= MAX_REMOVING_DEBOUNCE) {
        setState(MovementRectState::REMOVED);
    }
}

void MovementRect::setRect(Rect rect) {
    if (state == MovementRectState::REMOVING) {
        debounce = 0;
        state = MovementRectState::ADDED;
    }
    this->rect = rect;
}
