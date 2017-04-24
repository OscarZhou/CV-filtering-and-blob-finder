#include "Point.h"

Point_Oscar::Point_Oscar()
{
    //ctor
}

Point_Oscar::~Point_Oscar()
{
    //dtor
}

Point_Oscar::Point_Oscar(int x, int y)
{
    this->x = x;
    this->y = y;
}

bool Point_Oscar::operator< (const Point_Oscar& pt) const
{
    if(this->x < pt.x)
    {
        return true;
    }
    else if(this->x == pt.x)
    {
        if(this->y < pt.y)
        {
            return true;
        }
        else if(this->y == pt.y)
        {
            return false;
        }
        else{
            return false;
        }
    }
    else
    {
        return false;
    }
}
