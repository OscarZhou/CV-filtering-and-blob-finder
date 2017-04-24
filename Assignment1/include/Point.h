#ifndef POINT_H
#define POINT_H






class Point_Oscar
{
    public:

        int x;
        int y;

        Point_Oscar();
        virtual ~Point_Oscar();

        Point_Oscar(int x, int y);
        bool operator< (const Point_Oscar& pt) const;

    protected:
    private:
};

#endif // POINT_H
