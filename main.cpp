#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <chrono>
#include <opencv2/opencv.hpp>

#include <vector>
#include <set>


using namespace cv;
using namespace std;
using namespace chrono;


#define Mpixel(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)]

#define MpixelB(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)*((image).channels())]
#define MpixelG(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)*((image).channels())+1]
#define MpixelR(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)*((image).channels())+2]

class CPoint
{
    public:

        int x;
        int y;

        CPoint();
        CPoint(int x, int y);
        virtual ~CPoint();

        bool operator< (const CPoint& pt) const;


    protected:
    private:
};

CPoint::CPoint()
{

}

CPoint::~CPoint()
{

}

CPoint::CPoint(int x, int y)
{
    this->x = x;
    this->y = y;
}

bool CPoint::operator< (const CPoint& pt) const
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
};

typedef std::set<CPoint> point_set;
typedef vector<point_set> set_vector;

/************************************************************************
*
* Function declaration
*
*************************************************************************/
Mat do_median_filter(Mat imgOri);
Mat do_threshold_filter(Mat imgOri, int flag);
int count_object(Mat imgOri);

Mat frame;//, image;
int main(int argc, char** argv)
{
    VideoCapture cap;
    cap.open(0);
    if (!cap.isOpened())
     {
         cout << "Failed to open camera" << endl;
         return 0;
     }
    cout << "Opened camera" << endl;
    namedWindow("WebCam", 1);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    //   cap.set(CV_CAP_PROP_FRAME_WIDTH, 960);
    //   cap.set(CV_CAP_PROP_FRAME_WIDTH, 1600);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    //   cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
    //   cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1200);
    cap >> frame;
    printf("frame size %d %d \n",frame.rows, frame.cols);
    int key=0;

    double fps=0.0;
    while (1){
     system_clock::time_point start = system_clock::now();
     //for(int a=0;a<10;a++){
       cap >> frame;
       if( frame.empty() )
       break;

        Mat imgMedianFilter = do_median_filter(frame);
        Mat imgThresholdFilter = do_threshold_filter(imgMedianFilter, 1);
        Mat imgMedianFilter2 = do_median_filter(imgThresholdFilter);
        fps = (double)count_object(imgMedianFilter2);

       char printit[100];
       sprintf(printit,"the number of objects is %2.1f",fps);
       putText(frame, printit, cvPoint(10,30), FONT_HERSHEY_PLAIN, 2, cvScalar(255,255,255), 2, 8);
       imshow("WebCam", frame);
       key=waitKey(1);
       if(key==113 || key==27) return 0;//either esc or 'q'

     //}
     system_clock::time_point end = system_clock::now();
     double seconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
     //fps = 1000000*10.0/seconds;
     fps = 1000000/seconds;
     cout << "frames " << fps << " seconds " << seconds << endl;
    }



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
* The impletation of threshold filter
* Parameter description:
* imgOri : input image
* flag: 0, Otsu method  1, normal method
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
    else if(flag == 1)
    {
        threshold(imgOri, imgPcd, 50, 255, THRESH_BINARY); // threshold is set as 50 which is for increasing accuracy
    }
    return imgPcd;
}

/************************************************************************
*
* The impletation of blob algorithm
* Parameter description:
* imgOri : input image with salt and pepper
*
*************************************************************************/
int count_object(Mat imgOri)
{
    // Store all the sets which stand for the object
    set_vector vec;
    int counter = -1, s1, s2;

    int width = imgOri.cols;
    int height = imgOri.rows;
    int* matrixA = new int[width * height];
    //memset(matrixA, -1, width * height);

    for(int y=0; y< height; y++)
    {
        for(int x=0; x<width; x++)
        {
            matrixA[ x + y * width] = -1;
        }
    }
    /************************************************************************
    *
    * The implementation of object labeling algorithm using 4-adjacency
    *
    *************************************************************************/
    for(int y=1; y<height; y++)
    {
        for(int x=1; x<width; x++)
        {
            if((int)Mpixel(imgOri, x, y)  != 0)
            {
                if((int)Mpixel(imgOri, x-1, y) != 0 || (int)Mpixel(imgOri, x, y-1) != 0)
                {
                    s1 = matrixA[(x - 1) + (y * width)];
                    s2 = matrixA[x +  ((y - 1) * width)];


                    if(s1 != -1)
                    {
                        point_set* pset = &vec[s1];
                        pset->insert(CPoint(x, y));
                        matrixA[x + y * width] = s1;
                    }

                    if(s2 != -1)
                    {
                        point_set* pset = &vec[s2];
                        pset->insert(CPoint(x, y));
                        matrixA[x + y * width] = s2;
                    }

                    if((s1 != s2) && (s1 != -1) && (s2 != -1))
                    {
                        point_set* pset = &vec[s2];

                        for(point_set::iterator it=pset->begin(); it!=pset->end(); it++)
                        {
                           //((point_set)vec[s1]).insert(*it);
                            matrixA[((CPoint)*it).x + ((CPoint)*it).y * width] = s1;
                        }

                        ((point_set)vec[s1]).insert(pset->begin(), pset->end());
                        pset->clear();
                        pset = NULL;
                    }
                }
                else
                {
                    counter++;
                    point_set setofobj;

                    setofobj.insert(CPoint(x, y));
                    vec.push_back(setofobj);
                    matrixA[x + y * width] = counter;
                }
            }

        }
    }
    free(matrixA);
    /************************************************************************
    *
    * count the number of valid set, that is, the number of objects
    *
    *************************************************************************/
    int num = 0;
    if(!vec.empty())
    {
        for(set_vector::iterator it=vec.begin(); it!=vec.end(); it++)
        {
            point_set poset = *it;
            //cout<<"the numbers of points in poset: "<<poset.size()<<endl;
            if(!poset.empty() && poset.size()>20) // size>20 is for increasing the accuracy
                num++;
        }
    }


	return num;
}
