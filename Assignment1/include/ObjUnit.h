#ifndef OBJUNIT_H
#define OBJUNIT_H


#include <set>
#include <Point.h>


typedef std::set<Point_Oscar> point_set;

//push_flag
enum PUSH_MODE{
    ADD_TO_BACK,
    ADD_BY_ORDER
};

class ObjUnit  // Similar to SET[0]
{
    public:

        point_set setOfPoint;

        ObjUnit();
        ObjUnit(point_set ptSet);
        virtual ~ObjUnit();

        int add_point(Point_Oscar pt);

    protected:
    private:

};

#endif // OBJUNIT_H
