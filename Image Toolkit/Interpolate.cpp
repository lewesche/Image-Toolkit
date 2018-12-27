
#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>       

using namespace std;
using namespace cv;
using namespace std::this_thread;
using namespace std::chrono_literals;

using image_t = Mat; // Define typedef for opencv image format

// Map all pixels in an image to a new position, no interpolation. Leaves blank space between pixels. 
image_t mapBasePixels(int scale, image_t &imageIn) {

	unsigned char *q; // Create pointer for pixels in output image
	unsigned char *p; // Create pointer for pixels in input image

	image_t imageOut(Size(imageIn.cols*scale, imageIn.rows*scale), CV_8UC3);

	for (int x = 0; x < imageIn.cols; x++) {		// Cycle through x (left to right)
		for (int y = 0; y < imageIn.rows; y++) {	// Cycle through y (top to bottom)
			q = imageIn.ptr(y, x);					// Point towards pixel in original image
			p = imageOut.ptr(y*scale, x*scale);		// Point towards new pixel position in expanded image
			// Copy pixel values
			p[0] = q[0];	// B
			p[1] = q[1];	// G
			p[2] = q[2];	// R
		}
	}
	return imageOut;
}

// Linearly interpolate between points.
int linInterp(int scale, int position, int leftValue, int rightValue) {
	int pixelValue;

	position = static_cast<double>(position);	// Cast position variable as a double to prevent bad integer rounding 
	pixelValue = ((scale - position % scale)*leftValue + (position % scale) * rightValue) / scale;

	return pixelValue;
}

// Construct a cubic spline to interpolate between points.
int cubicInterp(int scale, int position, int farLeftValue, int leftValue, int rightValue, int farRightValue) {
	int value;

	position = static_cast<double>(position);	// Cast position variable as a double to prevent bad integer rounding 
	value = leftValue + 0.5 * (position % scale) / scale * (rightValue - farLeftValue + (position % scale) / scale *
		(2.0*farLeftValue - 5.0*leftValue + 4.0*rightValue - farRightValue + (position % scale) / scale *
		(3.0*(leftValue - rightValue) + farRightValue - farLeftValue)));	
		// Linearly interpolate between points in front and behind
		// Equation constructs a cubic spline using the end point slopes and values

	// It is possible for interpolated pixel values to exceed the top and bottom ranges of 8 bit color at points of shart color changes. 
	// This leads to overflow errors down the road, resulting in messed up images.  
	// To prevent overflow error, pixels above and below limit are rounded.  
	if (value > 255) {		// In case interpolated pixel values are greater than 255, 
		value = 255;		// Drop value to 255
	}
	else if (value < 0) {	// In case interpolated pixel values are below zero, 
		value = 0;			// Drop value to zero
	}
	return value;
}
