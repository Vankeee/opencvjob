#include <opencv/cv.h>  
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp> 
#include <iostream>
#include <stdio.h>  

using namespace cv;
using namespace std;


// Contains the description of the match
typedef struct Match_desc{
	bool init;
	double maxVal;
	Point maxLoc;
	double scale;
	Match_desc(): init(0){}
} Match_desc;

int main(int argc,char** argv){
	// const char template_str[] = "cod_logo.png";
	// const char target_str[] = "cod_blackops.jpg";

	namedWindow("DBG", WINDOW_AUTOSIZE);


	Mat template_mat;
	template_mat = imread(argv[2], 1); // Read image
	cvtColor(template_mat, template_mat, COLOR_BGR2GRAY); // Convert to Gray
	Canny(template_mat, template_mat, 50, 50*4); // Find edges

	// Find size
	int tW, tH;
	tW = template_mat.cols;
	tH = template_mat.rows;

	namedWindow("Template Image", WINDOW_AUTOSIZE );
	imshow("Template Image", template_mat);


	Mat target_img, target_gray, target_resized, target_edged;
	target_img = imread(argv[1], 1); // Read image
	cvtColor(target_img, target_gray, COLOR_BGR2GRAY); // Convert to Gray

	const float SCALE_START = 1;
	const float SCALE_END = 0.2;
	const int SCALE_POINTS = 30;

	Match_desc found;
	// 尺寸变化:1 ,0.8,0.6,0.4,0.2
	for(float scale = SCALE_START; scale >= SCALE_END; scale -= (SCALE_START - SCALE_END)/SCALE_POINTS){
		resize(target_gray, target_resized, Size(0,0), scale, scale);// Resize

		// Break if target image becomes smaller than template
		if(tW > target_resized.cols || tH > target_resized.rows) break;

		Canny(target_resized, target_edged, 50, 50*4); // Find edges


		// Match template
		Mat result;
		matchTemplate(target_edged, template_mat, result, TM_CCORR_NORMED);

		double maxVal; Point maxLoc;
		minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc);
		                              
		// If better match found
		if( found.init == false || maxVal > found.maxVal ){
			found.init = true;
			found.maxVal = maxVal;
			found.maxLoc = maxLoc;
			found.scale = scale;
		}
		cout <<"maxVal is :" <<maxVal<<"\n";
		// START VISUALIZATION CODE
		Mat target_clone;
		resize(target_img, target_clone, Size(0,0), scale, scale);// Resize
		rectangle(target_edged, Point(maxLoc.x, maxLoc.y), Point(maxLoc.x + tW, maxLoc.y + tH), Scalar(0, 0, 255), 3);
		imshow("DBG", target_edged);
		waitKey(300);
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

	waitKey(3000);
}