#include "ObjUnit.h"

ObjUnit::ObjUnit()
{
    //ctor


}

ObjUnit::~ObjUnit()
{
    //dtor
}

ObjUnit::ObjUnit(point_set ptSet)
{
    // Ensure that the numbers were copied into set but the pointer
    for(point_set::iterator it=ptSet.begin(); it!=ptSet.end(); it++)
    {

        this->setOfPoint.insert(*it);
    }
}


// Encapsulate insert method of set container
int ObjUnit::add_point(Point_Oscar pt)
{

    this->setOfPoint.insert(pt);
    return this->setOfPoint.size();
}
