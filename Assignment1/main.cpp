#include <iostream>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

#include <vector>
#include <set>
#include <ObjUnit.h>


using namespace std;
using namespace cv;


#define Mpixel(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)]



#define MpixelB(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)*((image).channels())]
#define MpixelG(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)*((image).channels())+1]
#define MpixelR(image, x, y) ((uchar *)(((image).data)+(y)*((image).step)))[(x)*((image).channels())+2]


typedef vector<point_set> obj_vector;
//typedef vector<ObjUnit> obj_vector;


void PrintArray(int* pArray, int length);

int get_num_of_object(obj_vector vectorOfObj);
int compare_for_median(const void * a, const void * b);
Mat do_median_filter(Mat imageOri);
int count_blob(Mat image);
Mat color_object(Mat imageOri, obj_vector vectorOfObj, int numOfObj);


void print_mat(Mat mat, string text);

int g_counter=0;



void PrintArray(int* pArray, int length)
{

    for(int x=0; x<length; x++)
    {
        if(x== length-1)
        {
            cout<<pArray[x]<<endl;
        }
        else
            cout<<pArray[x]<<" ";
    }
}

void print_vector(obj_vector ps)
{
    if(!ps.empty())
    {
        for(int i=0;i<(int)ps.capacity();i++)
        {
            point_set * ps1= &ps[i];
            cout<<"vector["<<i<<"]"<<endl;
            for(point_set::iterator it= (*ps1).begin(); it!=(*ps1).end();it++)
            {
                cout<<"("<<((Point_Oscar)*it).x<<", "<<((Point_Oscar)*it).y<<")    ";
            }
            cout<<endl;
        }

        cout<<endl;
    }




}


void print_set(point_set ps, int index)
{
    cout<<"index: "<<index<< endl;
    for(point_set::iterator it= ps.begin(); it!=ps.end();it++)
    {
        cout<<"("<<((Point_Oscar)*it).x<<", "<<((Point_Oscar)*it).y<<")    ";
    }
    cout<<endl;
}

void print_set(point_set ps, int index, string text)
{
    cout<<text<<endl;
    cout<<"index: "<<index<< endl;
    for(point_set::iterator it= ps.begin(); it!=ps.end();it++)
    {
        cout<<"("<<((Point_Oscar)*it).x<<", "<<((Point_Oscar)*it).y<<")    ";
    }
    cout<<endl;
}

void print_mat(Mat mat, string text)
{
    cout<<text<<endl;

    for(int y=0; y<mat.rows; y++)
    {
        for(int x=0; x<mat.cols; x++)
        {
            cout<<(int)Mpixel(mat, x, y)<<"  ";
        }
        cout<<endl;
    }
}

void print_A(int mat[6][11], string text)
{
    cout<<text<<endl;
    for(int x=0; x<6; x++)
    {
        for(int y=0; y<11; y++)
        {
            cout<<mat[y][x]<<"\t";
        }
        cout<<endl;
    }
}




int main(int argc, char** argv)
{
    if(argc!=2) {
        cout<<"needs 2 argument, e.g.image.jpg"<<endl;
        exit(0);
	}

    namedWindow("Figure1", 0);
    namedWindow("Figure2", 0);
    //namedWindow("Figure3", 0);


    /************************************************************************
    *
    * Read the original image and then create space for the two Mat being used
    *
    *************************************************************************/

	Mat imageOri = imread(argv[1], IMREAD_GRAYSCALE);
    Mat imageRet = do_median_filter(imageOri);
    for(int i=0; i<10; i++)
        imageRet = do_median_filter(imageRet);

    //------print_mat(imageOri, "------original image-------");
    //imwrite("saltandpepper_ret.jpg", imageRet);
	//imageRet = do_median_filter(imageRet);
	//imshow("Figure3", imageRet);


    cout<< imageOri.cols<<"   "<<imageOri.rows<<endl;
    /*
    for(int y=0; y<imageOri.rows; y++)
    {
        for(int x=0; x<imageOri.cols; x++)
        {
            cout<<(int)Mpixel(imageOri, x, y)<<"\t";
        }
        cout<<endl;
    }

    //
    int arr[5][5] = {{0,0,0,0,0},{0,1,1,1,0},{1,1,1,0,0},{0,0,0,0,1},{0,1,0,1,1}};
    for(int x=0; x<imageOri.cols; x++)
    {
        for(int y=0; y<imageOri.rows; y++)
        {
            Mpixel(imageOri, y, x) = arr[x][y];
        }
    }
*/
	imshow("Figure1", imageOri);
    //print_mat(imageOri, "------original image-----");
    /*
    Mat img2;
    img2.create(imageOri.rows, imageOri.cols+5, CV_8UC1);


    int arr[5][10] = {{0,0,0,0,0,0,1,1,1,0},{0,1,1,1,0,0,0,0,0,0},{1,1,1,0,0,0,0,0,0,0,},{0,0,0,0,0,0,0,0,0,1},{0,0,0,1,0,0,1,0,1,1}};
    for(int y=0; y<imageOri.rows; y++)
    {
        for(int x=0; x<imageOri.cols+5; x++)
        {
            Mpixel(img2, x, y) = arr[y][x];
        }
    }
	imshow("Figure2", img2);

	imwrite("testimage2.jpg", img2);
	*/

	imshow("Figure2", imageRet);
    int num = count_blob(imageRet);

    cout<<"The number of objects is "<<num<<endl;
    cout<<"The number of g_counter is "<<g_counter<<endl;

    //imwrite("~/Downloads/testimage.jpg",imageOri);
    //Mat image = imread("~/Downloads/testimage.jpg", 0);
    //print_mat(image, "------read image again-----");
	//imshow("Figure2", image);
	waitKey(0);
    return 0;
}





int compare_for_median(const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}


Mat do_median_filter(Mat imageOri)
{
    int pMask[9] = {0}; // Store the temperary data used for qsort
    Mat imageRet, imageExtention;
    imageRet.create(imageOri.size(), CV_8UC1);   // Filtered image
    imageExtention.create(imageOri.rows+2, imageOri.cols+2, CV_8UC1);

    /************************************************************************
    *
    * Expand outer boundary for original image
    *
    *************************************************************************/
    for(int y=0; y<imageExtention.rows; y++)
        for(int x=0; x<imageExtention.cols; x++)
        {
            if(x==0)//left;
            {
                if(y==0)//left top
                {
                    Mpixel(imageExtention, x, y) = Mpixel(imageOri, x, y);
                }
                else if(y==imageExtention.rows-1) //left bottom
                {
                    Mpixel(imageExtention, x, y) = Mpixel(imageOri, x, y-2);
                }
                else // left except left top and left bottom
                {
                    Mpixel(imageExtention, x, y) = Mpixel(imageOri, x, y-1);
                }
            }
            else if(x==imageExtention.cols-1)//right
            {
                if(y==0)//right top
                {
                    Mpixel(imageExtention, x, y) = Mpixel(imageOri, x-2, y);
                }
                else if(y==imageExtention.rows-1) //right bottom
                {
                    Mpixel(imageExtention, x, y) = Mpixel(imageOri, x-2, y-2);
                }
                else // right except right top and right bottom
                {
                    Mpixel(imageExtention, x, y) = Mpixel(imageOri, x-2, y-1);
                }
            }
            else if(y==0)//top
            {
                if(x!=0 && x!=imageExtention.cols-1) //top except left top and right top
                {
                    Mpixel(imageExtention, x, y) = Mpixel(imageOri, x-1, y);
                }
            }
            else if(y==imageExtention.rows-1)//bottom
            {
                if(x!=0 && x!=imageExtention.cols-1) //bottom except left bottom and right bottom
                {
                    Mpixel(imageExtention, x, y) = Mpixel(imageOri, x-1, y-2);
                }
            }
            else
                Mpixel(imageExtention, x, y) = Mpixel(imageOri, x-1, y-1);

        }

    /************************************************************************
    *
    * Read the original image and then create space for the two Mat being used
    * Output filtered image
    *
    *************************************************************************/
    for(int y=1; y<imageExtention.rows-1; y++)
        for(int x=1; x<imageExtention.cols-1; x++)
        {
            pMask[0] = Mpixel(imageExtention, x-1, y-1);
            pMask[1] = Mpixel(imageExtention, x, y-1);
            pMask[2] = Mpixel(imageExtention, x+1, y-1);
            pMask[3] = Mpixel(imageExtention, x-1, y);
            pMask[4] = Mpixel(imageExtention, x, y);
            pMask[5] = Mpixel(imageExtention, x+1, y);
            pMask[6] = Mpixel(imageExtention, x-1, y+1);
            pMask[7] = Mpixel(imageExtention, x, y+1);
            pMask[8] = Mpixel(imageExtention, x+1, y+1);

            qsort(pMask, 9, sizeof(int), compare_for_median);

            // Important part
            // Improve the accuracy
            if(pMask[4]<30)
                Mpixel(imageRet, x-1, y-1) = 0;
            else
                Mpixel(imageRet, x-1, y-1) = pMask[4];
        }


    return imageRet;
}


int count_blob(Mat image)
{


    /************************************************************************
    *
    * Binamise the filtered image
    *
    *************************************************************************/
    Mat imgOrigin;     // The original image which is obtained by read function
    threshold(image, imgOrigin, 0 ,255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    obj_vector vectorOfObj;   // The main data structure which controls the entire sperated objects
    int counter=-1, s1, s2;   //counter refers to the index of vectorOfObj.
                            // s1 and s2 are the auxiliary
    int matrixA[imgOrigin.cols+1][imgOrigin.rows+1] = {-1};        //This is a matrix to differiate the objects
    //int matrixA[6][11] = {0};
    Mat imgOriginExtraBoundary;


    //int xlength = imgOrigin.cols+1;
    //int ylength = imgOrigin.rows+1;
    //cout<<"---------------"<<sizeof(imgOrigin.cols)<<endl;
    //int matrixA[xlength][ylength] = {0};
    //print_A(matrixA, xlength, ylength, "----------------------");
    /************************************************************************
    *
    * Read the original image and then create space for the two Mat being used
    *
    *************************************************************************/
    imgOriginExtraBoundary.create(imgOrigin.rows+1, imgOrigin.cols+1, CV_8UC1);
    //imgOrigin.create(image.rows, image.cols, CV_8UC1);


    //namedWindow("Figure5", 0);
    //imshow("Figure5",image);
    /************************************************************************
    *
    * Copy original matrix to imgOriginExtraBoundary and initialize matrixA -1
    *
    *************************************************************************/
    //print_mat(imgOriginExtraBoundary, "------extra boundary image-----");
    for(int y=0; y<imgOriginExtraBoundary.rows; y++)
        for(int x=0; x<imgOriginExtraBoundary.cols; x++)
        {
            if(x==0 || y==0 )
            {
                Mpixel(imgOriginExtraBoundary, x, y) = 0;
            }
            else
            {
                Mpixel(imgOriginExtraBoundary, x, y) = Mpixel(imgOrigin, x-1, y-1);
            }
            matrixA[x][y] = -1;
            /*
            if(x>(imgOriginExtraBoundary.cols-5) && y>(imgOriginExtraBoundary.rows-5))
            {
                cout<<"imgOriginExtraBoundary("<<x<<","<<y<<")="<<(int)imgOriginExtraBoundary.at<uchar>(x,y);
                cout<<"    matrixA("<<x<<","<<y<<")="<<matrixA[x][y]<<endl;
            }
            */
        }


    /************************************************************************
    *
    * Implement the algorithm of Object Labelling using 4-adjacency
    *
    *************************************************************************/
    for(int y=1; y<imgOriginExtraBoundary.rows; y++)
        for(int x=1; x<imgOriginExtraBoundary.cols; x++)
        {
            if((int)Mpixel(imgOriginExtraBoundary, x, y)  != 0)
            {
                if(Mpixel(imgOriginExtraBoundary, x-1, y)!=0 || Mpixel(imgOriginExtraBoundary, x, y-1)!=0)
                {
                    s1 = matrixA[x-1][y];
                    s2 = matrixA[x][y-1];

                    if(s1 != -1)
                    {
                        //ObjUnit* objunit = new ObjUnit(vectorOfObj[s1]);
                        //objunit->add_point(Point_Oscar(x, y));
                        point_set * setOfObj = &vectorOfObj[s1];
                        setOfObj->insert(Point_Oscar(x, y));
                        matrixA[x][y] = s1;
                    }

                    if(s2 != -1)
                    {
                        // Create a new set and insert a point which satisfies the condition
                        point_set * setOfObj = &vectorOfObj[s2];
                        setOfObj->insert(Point_Oscar(x, y));
                        // Find the position of current set in vector, delete it and insert new one at current position
                        //obj_vector::iterator it = vectorOfObj.begin()+s2;
                        //vectorOfObj.erase(it, it+1);
                        //vectorOfObj.insert(it, setOfObj);

                        // Modify the value of current position in current array
                        //print_set(vectorOfObj[s2], s2);
                        matrixA[x][y] = s2;

                    }

                    if((s1 != s2) && (s1 != -1) && (s2 !=-1))
                    {
                        //(vector<vector<Point>>)unionSet((vector<vector<Point> >)SET.at(s1), (vector<vector<Point>> )SET.at(s2)).
                        //print_vector(vectorOfObj);
                        point_set * setOfObj1 = &vectorOfObj[s1];
                        point_set * setOfObj2 = &vectorOfObj[s2];
                        for(point_set::iterator it=setOfObj2->begin(); it!=setOfObj2->end(); it++)
                        {
                            setOfObj1->insert(*it);
                            int x = ((Point_Oscar)*it).x;
                            int y = ((Point_Oscar)*it).y;
                            //cout<<"("<<x<<", "<<y<<")"<<endl;
                            matrixA[x][y] = s1;
                        }
                        //obj_vector::iterator it = vectorOfObj.begin()+s2;
                        //vectorOfObj.erase(it, it+1);
                        setOfObj2->clear();
                        setOfObj2 = NULL;

                        g_counter--;
                        //print_set(vectorOfObj[s1], s1, "-----after merge -----------");
                        //print_set(vectorOfObj[s2], s2, "-----after merge -----------");
                        //print_vector(vectorOfObj);
                    }

                }
                else
                {
                    //print_vector(vectorOfObj);
                    counter++;
                    point_set setOfObj;
                    setOfObj.insert(Point_Oscar(x, y));
                    vectorOfObj.push_back(setOfObj);
                    matrixA[x][y] = counter;
                    //print_set(vectorOfObj[counter], counter);
                    //cout<<" -------------------------------   counter="<<counter<<endl;
                    g_counter++;
                }
            }
            //string text = format("x=%d, y=%d", x,y);
            //print_mat(imgOriginExtraBoundary, text);
            //print_A(matrixA, text);

        }
    int num = get_num_of_object(vectorOfObj);
    Mat imageColor = color_object(imgOrigin, vectorOfObj, num);
    char printit[100];
    sprintf(printit,"%d",num);
    putText(imageColor, printit, cvPoint(10,30), FONT_HERSHEY_PLAIN, 2, cvScalar(255,255,255), 2, 8);

    namedWindow("Figure4", 0);
    imshow("Figure4",imageColor);
    return num;
}

/************************************************************************
*
* Calculate the number of single objects.
*
*************************************************************************/
int get_num_of_object(obj_vector vectorOfObj)
{
    int num = 0;
    if(!vectorOfObj.empty())
    {
        for(obj_vector::iterator it=vectorOfObj.begin(); it!=vectorOfObj.end(); it++)
        {
            if( !((point_set)*it).empty() && ((point_set)*it).size() >30)
            //if( !((point_set)*it).empty())
            {

                //print_set(((point_set)*it), num);
                num++;

                //cout<<"---the size of object "<<num<<"---------------"<<endl;
                //cout<<"-------------"<<((point_set)*it).size()<<"-------------------"<< endl;
                //cout<<endl;
            }
        }
    }
    return num;
}

Mat color_object(Mat imageOri, obj_vector vectorOfObj, int numOfObj)
{
    Mat imageRet;
    imageRet.create(imageOri.size(), CV_8UC3);
    for(int y=0; y<imageOri.rows; y++)
        for(int x=0; x<imageOri.cols; x++)
        {
            MpixelR(imageRet, x, y) = Mpixel(imageOri, x, y);
            MpixelG(imageRet, x, y) = Mpixel(imageOri, x, y);
            MpixelB(imageRet, x, y) = Mpixel(imageOri, x, y);
        }

    if(!vectorOfObj.empty())
    {
        for(obj_vector::iterator it=vectorOfObj.begin(); it!=vectorOfObj.end(); it++)
        {
            //if( !((point_set)*it).empty() && ((point_set)*it).size() >50)
            if( !((point_set)*it).empty())
            {
                point_set setOfObj =  *it;
                for(point_set::iterator it= setOfObj.begin(); it!=setOfObj.end(); it++)
                {
                    //cout<<"("<<((Point_Oscar)*it).x<<", "<<((Point_Oscar)*it).y<<")    ";

                    // The reason about the operation of (x-1) is that
                    // the position of the points recorded in sets is one row and one column more
                    int x = ((Point_Oscar)*it).x-1;
                    int y = ((Point_Oscar)*it).y-1;
                    MpixelR(imageRet, x, y) = 0;
                    MpixelG(imageRet, x, y) = 255;
                    MpixelB(imageRet, x, y) = 255;
                }

            }
        }
    }
    return imageRet;
}


