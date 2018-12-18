#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>       

using namespace std;
using namespace cv;
using namespace std::this_thread;
using namespace std::chrono_literals;

//Rotate Image 90 degrees clockwise
Mat rotate90CW(Mat imageIn) {
	Mat imageOut(Size(imageIn.rows, imageIn.cols), CV_8UC3);

	int w = imageOut.cols - 1;

	unsigned char * q;
	unsigned char * p;

	for (int y = 0; y < imageIn.rows; y++) {
		for (int x = 0; x < imageIn.cols; x++) {
			q = imageIn.ptr(y, x);
			p = imageOut.ptr(x, w - y);
			p[0] = q[0];   // B
			p[1] = q[1];   // G
			p[2] = q[2];   // R


		}
	}
	return imageOut;
}
//Rotat Image 90 degrees counter clockwise
Mat rotate90CCW(Mat imageIn) {
	Mat imageOut(Size(imageIn.rows, imageIn.cols), CV_8UC3);

	int w = imageOut.rows - 1;

	unsigned char * q;
	unsigned char * p;

	for (int y = 0; y < imageIn.rows; y++) {
		for (int x = 0; x < imageIn.cols; x++) {
			q = imageIn.ptr(y, x);
			p = imageOut.ptr(w - x, y);
			p[0] = q[0];   // B
			p[1] = q[1];   // G
			p[2] = q[2];   // R


		}
	}

	return imageOut;
}