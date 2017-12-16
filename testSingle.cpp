#include <opencv/cv.h>  
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>  
#include <iostream>
#include <stdio.h> 
#include <math.h> 


using namespace cv;
using namespace std;

// 记录匹配的信息
typedef struct Match_desc{
	bool init;
	double maxVal;
	Point maxLoc;
	double scale;
	Match_desc(): init(0){}
} Match_desc;


int main(int argc,char *argv[] ){
    
    char buff_result[40];
    FILE *fp;
    fp = fopen("result.txt","w");

    Mat test_img,target_img;
    char buf_test[60];          //路径字符缓冲区
    // sprintf(buf_test,"/home/van/Desktop/opencv/test2/%d.png",i+1); //生成图片路径
    test_img = imread(argv[1]);
    Mat canny;
    Canny(test_img,canny,50,50*4);

    
    //记录每张test img 的匹配情况
    double first =0.0; 
    double second =0.0;
    int  num_first, num_second;
    int j;
    //匹配样本库每一个目标
    for( j=0;j<37;j++){
        char buf_target[60];          //路径字符缓冲区
        sprintf(buf_target,"/media/vanke/DATA/opencvpro/my/%d.png",j+1); //生成图片路径
        target_img = imread(buf_target);
 
        // 对于target 取
        // 判断测试图片的大小，不大于target
        int tW, tH;
        tW = test_img.cols;
        tH = test_img.rows;

        // 用于尺寸变化的
        const float SCALE_START = 0.7;
        const float SCALE_END = 0.2;
        const int SCALE_POINTS = 20;
        namedWindow("testImg", WINDOW_GUI_EXPANDED);
        imshow("testImg",test_img);

        Mat target_resized,target_edged;
        Match_desc found;  // 匹配信息
        // 对图片缩放对比
        for(float scale = SCALE_START; scale >= SCALE_END; scale -= (SCALE_START - SCALE_END)/SCALE_POINTS){
            resize(target_img, target_resized, Size(0,0), scale, scale);// Resize

            // target图小于test图片，则break
            if(tW > target_resized.cols || tH > target_resized.rows) break;
            
            Canny(target_resized, target_edged, 50, 50*4); // Find edges

            //模板匹配
            Mat result;
            matchTemplate(target_edged,canny,result,TM_CCOEFF_NORMED);

            double maxVal; Point maxLoc;
            minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc);
                
            //记录两个最大的匹配数值和图片  
            if(maxVal>first){
                second = first;
                first = maxVal;
                num_first = j+1;
            }else if(maxVal >second){
                second = maxVal;

                num_second =j+1;
            }

            // If better match found
            if( found.init == false || maxVal > found.maxVal ){
                found.init = true;
                found.maxVal = maxVal;
                found.maxLoc = maxLoc;
                found.scale = scale;
                
            }
            //写入每次maxVal
            // sprintf(buff_result,"maxval is :%.5lf\n",maxVal);
            // fwrite(buff_result, 19, 1, fp);

            cout <<"maxVal is :" <<maxVal<<"\n"
            <<"target is :" <<j+1<<"\n";
            // START VISUALIZATION CODE
            Mat target_clone;
            resize(target_img, target_clone, Size(0,0), scale, scale);// Resize
            rectangle(target_clone, Point(maxLoc.x, maxLoc.y), Point(maxLoc.x + tW, maxLoc.y + tH), Scalar(0, 255, 255), 3);
            imshow("DBG", target_edged);
            waitKey(350);
            // END VISUALIZATION CODE
            
        }

        int startX, startY, endX, endY;
        startX = found.maxLoc.x / found.scale;
        startY = found.maxLoc.y / found.scale;

        endX= (found.maxLoc.x + tW) / found.scale;
        endY= (found.maxLoc.y + tH) / found.scale;

        // draw a bounding box around the detected result and display the image
        rectangle(target_img, Point(startX, startY), Point(endX, endY), Scalar(0, 0, 255), 3);

        imshow("DBG", target_img);

        waitKey(0);

    
    }
       
    fclose(fp);
}