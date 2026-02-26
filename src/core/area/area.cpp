#include "area.h"
int Area::getSquare()
{
    return rect.area();
}

Area Area::getIntercection(const Area &otherArea)
{
    auto otherRect = otherArea.getRect();
    auto intersection = rect & otherRect;
    return Area(intersection);
}

int Area::getIntercectionSquare(const Area &otherArea)
{
    auto otherRect = otherArea.getRect();
    auto intersection = rect & otherRect;
    return intersection.area();
}