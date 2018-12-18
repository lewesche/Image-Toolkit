
#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>       

using namespace std;
using namespace cv;
using namespace std::this_thread;
using namespace std::chrono_literals;

// Map all pixels in an image to a new position, no interpolation
Mat mapBasePixels(int scale, Mat imageIn, Mat imageOut) {

	unsigned char * q;
	unsigned char * p;

	for (int x = 0; x < imageIn.cols; x++) {
		for (int y = 0; y < imageIn.rows; y++) {
			q = imageIn.ptr(y, x);
			p = imageOut.ptr(y*scale, x*scale);
			p[0] = q[0];   // B
			p[1] = q[1];   // G
			p[2] = q[2];   // R
		}
	}
	return imageOut;
}

// Linearly interpolate between two points
int linInterp(int scale, int position, int leftValue, int rightValue) {
	int pixelValue;

	position = static_cast<double>(position);	// Cast position variable as a double to prevent bad integer rounding 
	pixelValue = ((scale - position % scale)*leftValue + (position % scale) * rightValue) / scale;

	return pixelValue;
}

// Construct a cubic spline to interpolate between points
int cubicInterp(int scale, int position, int farLeftValue, int leftValue, int rightValue, int farRightValue) {
	int value;

	position = static_cast<double>(position);	// Cast position variable as a double to prevent bad integer rounding 
	value = leftValue + 0.5 * (position % scale) / scale * (rightValue - farLeftValue + (position % scale) / scale *
		(2.0*farLeftValue - 5.0*leftValue + 4.0*rightValue - farRightValue + (position % scale) / scale *
		(3.0*(leftValue - rightValue) + farRightValue - farLeftValue)));	//Linearly interpolate between points in front and behind

	if (value > 255) {
		value = 255;
	}
	else if (value < 0) {
		value = 0;
	}
	return value;
}
