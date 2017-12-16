#include <opencv/cv.h>  
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>  
#include <iostream>
#include <stdio.h>  


using namespace cv;
using namespace std;


int main(){
    char buf_path[60];          //路径字符缓冲区
    
    
    for(int i=0; i<8;i++){
        vector<Point> points;
        int count =0;
        Mat src,resized ,bi_img ,canny;
        sprintf(buf_path,"/media/vanke/DATA/opencvpro/ubuntuShare/%d.jpg",i+1); //生成图片路径
        src = imread(buf_path, IMREAD_GRAYSCALE);
        
        // 截取中间 2000*1000
        // Rect rect1(1500,1000, 1500,800);
        // Mat subImg1(src,rect1);

        //二值化 
        threshold(src, bi_img,100,255,cv::THRESH_BINARY);
        
        // //resize
        resize( bi_img,resized, Size(bi_img.cols,bi_img.rows));
        //找出边缘 Canny 
        Canny(resized,canny, 0, 120);

        //提取轮廓
        vector< vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(canny, contours, hierarchy,
        RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
        
        //vector<Point> 点集合 
        for(int j=0;j< contours.size();j++)
            for(int m=0; m<contours[j].size(); m++){
                points.push_back(contours[j][m]);
               
                // count ++;
                // cout << "count :" << count  <<"\n";
            }
        
        
        

        // cout << "vector size" <<points.size ()<<"\n";
        //算出矩形框
        Rect rect = boundingRect(points);
        
        //画矩形框
        rectangle(canny,rect,Scalar(255,255,255),2);
        // 抠出图形
        Mat subImg2(resized,rect);
       
        imshow("bi",subImg2);
        char imgPath[60];
        sprintf(imgPath,"/media/vanke/DATA/opencvpro/my2/%d.png",i+1); //生成图片路径
        imwrite(imgPath,subImg2);
        waitKey(300);

    }




    // Mat src ,src_gray,resizedImg;
    // // src = imread( argv[1], IMREAD_COLOR );
    // cv::Size size = cv::Size(src.cols/2, src.rows/2);
    // resize(src,resizedImg,size,CV_INTER_CUBIC);
    // cvtColor( src, src_gray, COLOR_BGR2GRAY );

    // //二值化
    // Mat thresholded;
    // threshold(src_gray,thresholded,40,255,cv::THRESH_BINARY);

    // imshow("bi",thresholded);
    // imwrite("my.jpg", thresholded); 

    // waitKey(10000);

}