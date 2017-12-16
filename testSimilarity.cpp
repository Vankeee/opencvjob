#include <opencv/cv.h>  
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>  
#include <iostream>
#include <stdio.h>  
#include <math.h>
#include <unistd.h>


using namespace cv;
using namespace std;


int count_num=0;
int count_all = 84;
// 记录匹配的信息
typedef struct Match_desc{
	bool init;
	double maxVal;
	Point maxLoc;
	double scale;
	Match_desc(): init(0){}
} Match_desc;


void checkRight(int test,int first_num );
int main(){
   
    char buff_result[40];
    FILE *fp;
    fp = fopen("result2.txt","w");

   
    int i;
    // 对测试集的每一张图片进行测试
    for(i =0;i<8; i++ ){
        Mat test_img,target_img;
        char buf_test[60];          //路径字符缓冲区
        sprintf(buf_test,"/media/vanke/DATA/opencvpro/my2/%d.png",i+1); //生成图片路径
        test_img = imread(buf_test);
        Mat canny;
        Canny(test_img,canny,50,50*4);

        //打印测试图片的号数
        char buf_test_num[3];
        if(i<9){
            sprintf(buf_test_num,"%d\n",i+1);
            fwrite(buf_test_num, 2, 1, fp);
        }else{
            sprintf(buf_test_num,"%d\n",i+1);
            fwrite(buf_test_num, 3, 1, fp);
        }
        cout << i+1 << "\n" ;
       

        //记录每张test img 的匹配情况
        double first =0.0; 
        double second =0.0;
        int  num_first, num_second;
        int j;
        //匹配样本库每一个目标
        for( j=0;j<36;j++){
            char buf_target[60];          //路径字符缓冲区
            sprintf(buf_target,"/media/vanke/DATA/opencvpro/my/%d.png",j+1); //生成图片路径
            target_img = imread(buf_target);

            //增加对比例的判断
            float ratio_target = (float)target_img.cols/target_img.rows;
            float ratio_test = (float)test_img.cols/test_img.rows;

            float difference = ratio_target-ratio_test;
            float diff_ratio = abs(difference/ratio_target);

            // cout <<"ratio_target : "<< ratio_target<<"\n";
            // cout <<"ratio_test : "<< ratio_test<<"\n";
            // cout <<"diff_ratio : "<< diff_ratio<<"\n";
       
            if(diff_ratio>0.1){
                // cout<< "skip" <<"\n";
                continue;
            }
             
            // 对于target 取
            // 判断测试图片的大小，不大于target
	        int tW, tH;
	        tW = test_img.cols;
	        tH = test_img.rows;

            // 用于尺寸变化的
            const float SCALE_START = 0.78;
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
                matchTemplate(target_resized,test_img,result,TM_CCORR_NORMED);

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

                // cout <<"maxVal is :" <<maxVal<<"\n";
                // START VISUALIZATION CODE
                Mat target_clone;
                resize(target_img, target_clone, Size(0,0), scale, scale);// Resize
                rectangle(target_clone, Point(maxLoc.x, maxLoc.y), Point(maxLoc.x + tW, maxLoc.y + tH), Scalar(0, 255, 255), 3);
                imshow("DBG", target_clone);
                // waitKey(10);
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

            // waitKey(300);
            

        }
        //打印排名前2个匹配图片
        char buf_first[50];
        char buf_second[50];
        if(num_first<9){
            sprintf(buf_first,"first num is%d, simi is:%.5lf\n",num_first,first);
            sprintf(buf_second,"second num is%d, simi is:%.5lf\n\n",num_second,second);
            cout << buf_first <<"\n"<<buf_second<<"\n";
            fwrite(buf_first, 31, 1, fp);
            fwrite(buf_second, 33, 1, fp);
        }else{
            sprintf(buf_first,"first num is%d, simi is:%.5lf\n",num_first,first);
            sprintf(buf_second,"second num is%d, simi is:%.5lf\n\n",num_second,second);
            cout << buf_first <<"\n"<<buf_second<<"\n";
            fwrite(buf_first, 32, 1, fp);
            fwrite(buf_second, 34, 1, fp);
        }
        //在搜索完全部之后进行准确率计算
            checkRight(i+1,num_first);
            cout<<"check"<< "\n";


    }

    fclose(fp);
    cout <<"count num"<< count_num << "\n";


}


void checkRight(int test,int first_num){
    if((test==7||test==6||test==55||test==56||test==8||test==82||test==83||test==84)&&first_num==1){
        count_num +=1;
        
    }
    if((test==1||test==4||test==5)&&first_num==2){
        count_num +=1;
        
    }
    if((test==2||test==9||test==10)&&first_num==3){
        count_num +=1;
        
    }
    if((test==11||test==12)&&first_num==4){
        count_num +=1;
        
    }
    if((test==13||test==14||test==15)&&first_num==5){
        count_num +=1;
        
    }
    if((test==17||test==16)&&first_num==6){
        count_num +=1;
        
    }
    if((test==19||test==18)&&first_num==7){
        count_num +=1;
        
    }
    if((test==21||test==20||test==22)&&first_num==8){
        count_num +=1;
        
    }
    if((test==23||test==24)&&first_num==9){
        count_num +=1;
        
    }
    if((test==25||test==26)&&first_num==10){
        count_num +=1;
        
    }
    if((test==27)&&first_num==11){
        count_num +=1;
        
    }
    if((test==28||test==29)&&first_num==12){
        count_num +=1;
        
    }
    if((test==30||test==31)&&first_num==13){
        count_num +=1;
        
    }
    if((test==32||test==33)&&first_num==14){
        count_num +=1;
        
    }
    if((test==34||test==35)&&first_num==15){
        count_num +=1;
        
    }

    if((test==36)&&first_num==16){
        count_num +=1;
        
    }
    if((test==37||test==38)&&first_num==17){
        count_num +=1;
        
    }
    if((test==40||test==39)&&first_num==18){
        count_num +=1;
        
    }
    if((test==42||test==41||test==43)&&first_num==19){
        count_num +=1;
        
    }

    if((test==44||test==45)&&first_num==20){
        count_num +=1;
        
    }
    if((test==46||test==47)&&first_num==21){
        count_num +=1;
        
    }
    if((test==49||test==48)&&first_num==22){
        count_num +=1;
        
    }
    if((test==51||test==50)&&first_num==23){
        count_num +=1;
        
    }

    if((test==52||test==53||test==54)&&first_num==24){
        count_num +=1;
        
    }
    if((test==57||test==58)&&first_num==25){
        count_num +=1;
        
    }
    if((test==60||test==59)&&first_num==26){
        count_num +=1;
        
    }
    if((test==61||test==62)&&first_num==27){
        count_num +=1;
        
    }
    if((test==63||test==64)&&first_num==28){
        count_num +=1;
        
    }
    if((test==66||test==65)&&first_num==29){
        count_num +=1;
        
    }

    if((test==67||test==68)&&first_num==30){
        count_num +=1;
        
    }
    if((test==70||test==69)&&first_num==31){
        count_num +=1;
        
    }
    if((test==72||test==71)&&first_num==32){
        count_num +=1;
        
    }
    if((test==73||test==74||test==75)&&first_num==33){
        count_num +=1;
        
    }
    if((test==76||test==77)&&first_num==34){
        count_num +=1;
        
    }
    if((test==79||test==78)&&first_num==35){
        count_num +=1;
        
    }
    if((test==81||test==80)&&first_num==36){
        count_num +=1;
        
    }



}