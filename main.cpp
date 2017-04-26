#include <iostream>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

#include <vector>
#include <set>


using namespace std;
using namespace cv;


#define Mpixel(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)]

#define MpixelB(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)*((image).channels())]
#define MpixelG(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)*((image).channels())+1]
#define MpixelR(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)*((image).channels())+2]


typedef std::set<Point> point_set;
typedef vector<point_set> set_vector;


class Point
{
    public:

        int x;
        int y;

        Point();
        virtual ~Point();

        Point(int x, int y) {this->x = x; this->y = y;};
        bool operator< (const Point& pt) const{
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
        };

    protected:
    private:
};


/************************************************************************
*
* Function declaration
*
*************************************************************************/
Mat do_median_filter(Mat imgOri);
Mat do_threshold_filter(Mat imgOri, int flag);

int main(int argc, char** argv)
{
    if(argc!=2) {
        cout<<"needs 2 argument, e.g.image.jpg"<<endl;
        exit(0);
	}

    namedWindow("OriginalImage", 0);
    Mat imgOri = imread(argv[1], IMREAD_GRAYSCALE);
	imshow("OriginalImage", imgOri);

    namedWindow("MedianFilterImage", 0);
    Mat imgMedianFilter = do_median_filter(imgOri);
	imshow("MedianFilterImage", imgMedianFilter);

    namedWindow("ThresholdFilterImage", 0);
    Mat imgThresholdFilter = do_threshold_filter(imgMedianFilter, 0);
	imshow("ThresholdFilterImage", imgThresholdFilter);


    waitKey();
    return 0;
}

/************************************************************************
*
* The compartor, define the rules to compare the value in qsort
* Parameter description:
* a: value1, b: value2
*
*************************************************************************/
int compare_for_median(const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

/************************************************************************
*
* The impletation of median filter
* Parameter description:
* imgOri : input image with salt and pepper
*
*************************************************************************/
Mat do_median_filter(Mat imgOri)
{
    // Store the temperary data used for qsort function
    int pMask[9] = {0, 0, 0,
                    0, 0, 0,
                    0, 0, 0};
    Mat imgExtension;
    imgExtension.create(imgOri.rows + 2, imgOri.cols + 2, CV_8UC1);

    /************************************************************************
    *
    * Expand outer boundary for original image
    *
    *************************************************************************/
    for(int y=0; y<imgExtension.rows; y++)
        for(int x=0; x<imgExtension.cols; x++)
        {
            if(x==0)//left;
            {
                if(y==0)//left top
                {
                    Mpixel(imgExtension, x, y) = Mpixel(imgOri, x, y);
                }
                else if(y==imgExtension.rows-1) //left bottom
                {
                    Mpixel(imgExtension, x, y) = Mpixel(imgOri, x, y-2);
                }
                else // left except left top and left bottom
                {
                    Mpixel(imgExtension, x, y) = Mpixel(imgOri, x, y-1);
                }
            }
            else if(x==imgExtension.cols-1)//right
            {
                if(y==0)//right top
                {
                    Mpixel(imgExtension, x, y) = Mpixel(imgOri, x-2, y);
                }
                else if(y==imgExtension.rows-1) //right bottom
                {
                    Mpixel(imgExtension, x, y) = Mpixel(imgOri, x-2, y-2);
                }
                else // right except right top and right bottom
                {
                    Mpixel(imgExtension, x, y) = Mpixel(imgOri, x-2, y-1);
                }
            }
            else if(y==0)//top
            {
                if(x!=0 && x!=imgExtension.cols-1) //top except left top and right top
                {
                    Mpixel(imgExtension, x, y) = Mpixel(imgOri, x-1, y);
                }
            }
            else if(y==imgExtension.rows-1)//bottom
            {
                if(x!=0 && x!=imgExtension.cols-1) //bottom except left bottom and right bottom
                {
                    Mpixel(imgExtension, x, y) = Mpixel(imgOri, x-1, y-2);
                }
            }
            else
                Mpixel(imgExtension, x, y) = Mpixel(imgOri, x-1, y-1);

        }

    Mat imgPcd;
    imgPcd.create(imgOri.size(), CV_8UC1);
    /************************************************************************
    *
    * Calculating the median of each window then assigning new value into
    *
    *************************************************************************/
    for(int y=1; y<imgExtension.rows-1; y++)
        for(int x=1; x<imgExtension.cols-1; x++)
        {
            pMask[0] = Mpixel(imgExtension, x-1, y-1);
            pMask[1] = Mpixel(imgExtension, x, y-1);
            pMask[2] = Mpixel(imgExtension, x+1, y-1);
            pMask[3] = Mpixel(imgExtension, x-1, y);
            pMask[4] = Mpixel(imgExtension, x, y);
            pMask[5] = Mpixel(imgExtension, x+1, y);
            pMask[6] = Mpixel(imgExtension, x-1, y+1);
            pMask[7] = Mpixel(imgExtension, x, y+1);
            pMask[8] = Mpixel(imgExtension, x+1, y+1);

            qsort(pMask, 9, sizeof(int), compare_for_median);

            // The important part. Improve the accuracy
            //if(pMask[4]<30)
            //    Mpixel(imgPcd, x-1, y-1) = 0;
            //else
                Mpixel(imgPcd, x-1, y-1) = pMask[4];
        }
    return imgPcd;
}

/************************************************************************
*
* The impletation of median filter
* Parameter description:
* imgOri : input image with salt and pepper
*
*************************************************************************/
Mat do_threshold_filter(Mat imgOri, int flag)
{
    Mat imgPcd;
    imgPcd.create(imgOri.size(), CV_8UC1);
    if(flag == 0)
    {
        threshold(imgOri, imgPcd, 0 ,255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    }
    return imgPcd;
}

/************************************************************************
*
* The impletation of median filter
* Parameter description:
* imgOri : input image with salt and pepper
*
*************************************************************************/







