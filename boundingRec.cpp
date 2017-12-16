#include <typeinfo>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


using namespace cv;
using namespace std;


cv::Size size = cv::Size(400, 300);

cv::Size size_rect = cv::Size(5, 5);
Rect rectCenterScale(Rect rect,Size size);

int main(int , char** argv){
    Mat src ,src_gray,resizedImg;
    src = imread( argv[1], IMREAD_COLOR );
    resize(src,resizedImg,size,CV_INTER_CUBIC);
    cvtColor( resizedImg, src_gray, COLOR_BGR2GRAY );

    //二值化
    Mat thresholded;
    threshold(src_gray,thresholded,185,255,cv::THRESH_BINARY);
    
    //提取轮廓
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(thresholded,contours, hierarchy,
    RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    //算出矩形框
    Rect rect = boundingRect(contours[0]);

    //可以在这里把矩形框进行一些处理，放大尺寸，不至于弄得刚刚好

    rect =rectCenterScale(rect,size_rect);
  
    //画矩形
    // drawContours(thresholded,contours,-1, Scalar(255, 0, 0), 2, 8, hierarchy, 0, Point());
    rectangle(thresholded,rect,Scalar(0,0,0));
    
    //抠出矩形 area-of-interest
    // Mat ROI = thresholded(rect);
    Mat subImg(thresholded,rect);
    
    
    //把抠出来的矩形再resize 
    Mat finalSize;
    resize(subImg,finalSize,cv::Size(subImg.cols*6, subImg.rows*6));
    
    //template matching
    
    
    
    imshow( "pic", finalSize);
    
    
    cout << "contours size :"<< contours[0].size() << "\n"
    << contours[1].size() << "\n "<< contours[2].size() << "\n";

    waitKey(1000000);


    return(0);


}


Rect rectCenterScale(Rect rect, Size size)  {
    rect = rect + size;   
    Point pt;  
    pt.x = cvRound(size.width/2.0);  
    pt.y = cvRound(size.height/2.0);  
    return (rect-pt);  
    
}
     
