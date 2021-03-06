#include <opencv/cv.h>  
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp> 
#include <iostream>
#include <stdio.h>  

using namespace std;
using namespace cv;

bool use_mask;
Mat img; Mat templ; Mat mask; Mat result;

const char* image_window = "Source Image";
const char* result_window = "Result window";
int match_method;
int max_Trackbar = 5;

void MatchingMethod( int, void* );
Mat dealImg(Mat *src);
Rect rectCenterScale(Rect rect,Size size);
int main( int argc, char** argv )
{
    if (argc < 3)
    {
        cout << "Not enough parameters" << endl;
        cout << "Usage:\n./MatchTemplate_Demo <image_name> <template_name> [<mask_name>]" << endl;
        return -1;
    }
        img = imread( argv[1], IMREAD_COLOR );
        // img = dealImg(&img);
        templ = imread( argv[2], IMREAD_COLOR );
        // int resize_height = 256;
        // int resize_width = 256;
        // resize(templ,reSizedImg,cv::Size(resize_width, resize_height), (0, 0), (0, 0), cv::INTER_LINEAR);
        if(argc > 3) {
        use_mask = true;
        mask = imread( argv[3], IMREAD_COLOR );
    }
    if(img.empty() || templ.empty() || (use_mask && mask.empty()))
    {
      cout << "Can't read one of the images" << endl;
     return -1;
    }
    namedWindow( image_window, WINDOW_AUTOSIZE );
    namedWindow( result_window, WINDOW_AUTOSIZE );
    const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
    createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );
    MatchingMethod( 0, 0 );
    waitKey(0);
    return 0;

}

void MatchingMethod( int, void* )
{

    Mat img_display;
    img.copyTo( img_display );
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    result.create( result_rows, result_cols, CV_32FC1 );
    bool method_accepts_mask = (CV_TM_SQDIFF == match_method || match_method == CV_TM_CCORR_NORMED);
    if (use_mask && method_accepts_mask)
    { matchTemplate( img, templ, result, match_method, mask); }
    else
    { matchTemplate( img, templ, result, match_method); }
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;
    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
    cout << "similarity is : "<< maxVal <<"\n";
    if( match_method  == TM_SQDIFF || match_method == TM_SQDIFF_NORMED )
    { matchLoc = minLoc; }
    else
    { matchLoc = maxLoc; }
    rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(255), 2, 8, 0 );
    rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(255), 2, 8, 0 );
    namedWindow(result_window,CV_WINDOW_NORMAL);
    imshow( image_window, img_display );
    imshow( result_window, result );
    return;

}
Mat dealImg(Mat  *src ){
    cv::Size size = cv::Size(400, 300);
    Mat src_gray,resizedImg;
    resize(*src,resizedImg,size,CV_INTER_CUBIC);
    cvtColor( resizedImg, src_gray, COLOR_BGR2GRAY );
      //二值化
    Mat thresholded;
    threshold(src_gray,thresholded,185,255,cv::THRESH_BINARY);
    //  //提取轮廓
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(thresholded,contours, hierarchy,
    RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
    //   //算出矩形框
    Rect rect = boundingRect(contours[0]);
    // //画矩形
    rectangle(thresholded,rect,Scalar(0,0,0));
    // //抠出矩形 area-of-interest
    // Mat ROI = thresholded(rect);
    Mat subImg(thresholded,rect);
    //把抠出来的矩形再resize 
    Mat finalSize;
    // resize(subImg,finalSize,cv::Size(subImg.cols*6, subImg.rows*6));
    
    return subImg;
}

Rect rectCenterScale(Rect rect, Size size)  {
    rect = rect + size;   
    Point pt;  
    pt.x = cvRound(size.width/2.0);  
    pt.y = cvRound(size.height/2.0);  
    return (rect-pt);  
    
}
     
