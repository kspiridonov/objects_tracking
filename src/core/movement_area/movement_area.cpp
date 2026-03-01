#include "movement_area.h"
int MovementArea::getSquare()
{
    return rect.area();
}

MovementArea MovementArea::getIntercection(const MovementArea &otherArea)
{
    auto otherRect = otherArea.getRect();
    auto intersection = rect & otherRect;
    return MovementArea(intersection);
}

int MovementArea::getIntercectionSquare(const MovementArea &otherArea)
{
    auto otherRect = otherArea.getRect();
    auto intersection = rect & otherRect;
    return intersection.area();
}