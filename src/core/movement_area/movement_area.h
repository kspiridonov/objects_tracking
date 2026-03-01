#pragma once
#include <opencv2/opencv.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/generators.hpp>
using namespace cv;
using namespace std;
class MovementArea
{
public:
    MovementArea(Rect rect) : rect(rect) {
       boost::uuids::uuid uuid = boost::uuids::random_generator()();
       id = boost::uuids::to_string(uuid);
    }
    int getSquare();
    MovementArea getIntercection(const MovementArea &otherArea);
    int getIntercectionSquare(const MovementArea &otherArea);
    Rect getRect() const
    {
        return rect;
    }

private:
    Rect rect;
    string id;
};