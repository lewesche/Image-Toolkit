
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

image_t mapBasePixels(int scale, image_t &imageIn);
int linInterp(int scale, int position, int leftValue, int rightValue); 
int cubicInterp(int scale, int position, int farLeftValue, int leftValue, int rightValue, int farRightValue);

//Prompts user for a scale factor
int getScaleFactor() {
	int scale;

	cout << "Scale Factor? Enter an int >= 1." << endl;
	
	while (1) {
		cin >> scale;

		if (std::cin.fail()) // has a previous extraction failed?
		{
			// yep, so let's handle the failure
			std::cin.clear(); // put us back in 'normal' operation mode
			std::cin.ignore(32767, '\n'); // and remove the bad input
		}
		else if (scale >= 1) {
			std::cin.ignore(32767, '\n');
			return scale;
		}
		cout << "Please enter a valid selection..." << endl;
	}


	return scale;
}

//Scale image without interpolation, leaving blank space between pixels. 
int simpleScale(image_t &image) {
	int scale = getScaleFactor(); // Ask user for positive integer scale factor
	
	image_t scaledImage(Size(image.cols*scale, image.rows*scale), CV_8UC3); // Create container for scaled image. 

	scaledImage = mapBasePixels(scale, image);
	
	imshow("Input Image", image);					// Show the original image
	image = scaledImage;
	imshow("Output Image", image);					// Show the transformed image
	return 0;
}

//Scale image via linear interpolating between known pixels
int linearScale(image_t &image) {
	int scale = getScaleFactor(); // Ask user for scale factor
	
	image_t scaledImage(Size(image.cols*scale, image.rows*scale), CV_8UC3); // Create container for expanded image output

	unsigned char *q0 = image.ptr(0, 0);	// Create pointers to be used for interpolation. q0 is the known point "behind" the pixel position
	unsigned char *q1 = image.ptr(0, 0);	// q1 is the known point "in front" of the pixel position
	unsigned char *p;						// Create pointer for pixels in the new image. 

	scaledImage = mapBasePixels(scale, image); // Copy over base pixels to expanded image

	//Logic to construct vertical nodal lines via linear interpolation
	for (int x = 0; x < scaledImage.cols; x++) {			// Cycle through rows of NEW image
		for (int y = 0; y < scaledImage.rows - scale; y++) {  // Cycle through columns of NEW image
			if (x%scale == 0 and y%scale == 0) {		// If we hit a "node", aka a pixel that maps directly to new image from original
				q0 = scaledImage.ptr(y, x);	// "Back" pnt = current pnt
				q1 = scaledImage.ptr(y + scale, x);	// "Forward" pt = next pnt
			}
			if (x%scale == 0 and y%scale != 0) {	// If we land on a vertical "node line"
				p = scaledImage.ptr(y, x);				// Direct pointer to current position in output image
				for (int i = 0; i < 3; i++) {
					p[i] = linInterp(scale, y, q0[i], q1[i]);	//Linearly interpolate between points above and below
				}
			}
		}
	}

	// Reset back and forward pointers, just cause.
	q0 = image.ptr(0, 0); 
	q1 = image.ptr(0, 0);

	//Logic to fill in horizontal points between vertical nodal lines via linear interpolation
	for (int y = 0; y < scaledImage.rows; y++) {			// Cycle through vertical values of scaled image
		for (int x = 0; x < scaledImage.cols - scale; x++) {  // Cycle through horizontal values of scaled imag
			if (x%scale == 0) {						// If along a vertical node line
				q0 = scaledImage.ptr(y, x);			// Left point = current point
				q1 = scaledImage.ptr(y, x + scale);			// Right point = point on next nodal line
			}
			else if (x%scale != 0) {					// If not on a node line
				p = scaledImage.ptr(y, x);				// Pick out current points
				for (int color = 0; color < 3; color++) {	// Iterate through RGB colors
					p[color] = linInterp(scale, x, q0[color], q1[color]);	//Linearly interpolate between points above and below
				}
			}
		}
	}
	imshow("Input Image", image);					// Show the original image
	image = scaledImage;
	imshow("Output Image", image);					// Show the transformed image
	return 0; // Return the output image
}

//Scale image via cubic interpolating between known pixels
int cubicScale(image_t &image) {
	int scale = getScaleFactor();
	
	image_t scaledImage(Size(image.cols*scale, image.rows*scale), CV_8UC3); // Create scaled image to output

	unsigned char *q0 = image.ptr(0, 0);	// Create pointers to be used for interpolation. q0 is the known pnt 2 "behind" the pixel position
	unsigned char *q1 = image.ptr(0, 0);	// q1 is the known pnt "behind" the pixel position
	unsigned char *q2 = image.ptr(0, 0);	// q2 is the known point "in front" of the pixel position
	unsigned char *q3 = image.ptr(0, 0);	// q3 is the known pnt 2 "in front" of the pixel position
	unsigned char *p;						// Create pointer for pixels in the new image. 

	scaledImage = mapBasePixels(scale, image);

	//Logic to construct vertical nodal lines via bicubic interpolation
	for (int x = 0; x < scaledImage.cols; x++) {						// Cycle through rows of NEW image
		for (int y = scale; y < scaledImage.rows - 2 * scale; y++) {		// Cycle through columns of NEW image
			if (x%scale == 0 and y%scale == 0) {						// If we hit a "node", aka a pixel that maps directly to new image from original
				q0 = scaledImage.ptr(y - scale, x);						// "Back 2" pnt = behind current pnt
				q1 = scaledImage.ptr(y, x);				// "Back" pt = 1 ahead of current pnt
				q2 = scaledImage.ptr(y + scale, x);			// "Forward" pt = 2 ahead of current pnt
				q3 = scaledImage.ptr(y + 2 * scale, x);			// "Forward 2" pt = 3 ahead of current pnt
			}
			else if (x%scale == 0 and y%scale != 0) {	// If we land on a vertical "node line"
				p = scaledImage.ptr(y, x);				// Direct pointer to current position in output image
				for (int i = 0; i < 3; i++) {
					p[i] = cubicInterp(scale, y, q0[i], q1[i], q2[i], q3[i]);
				}
			}
		}
	}

	q0 = q1 = q2 = q3 = image.ptr(0, 0);	// Create pointers to be used for interpolation. q0 is the known pnt 2 "behind" the pixel position

	//Logic to fill in horizontal points between vertical nodal lines via linear interpolation
	for (int y = 0; y < scaledImage.rows; y++) {
		for (int x = scale; x < scaledImage.cols - 2 * scale; x++) {
			if (x%scale == 0) {								// If along a vertical node line
				q0 = scaledImage.ptr(y, x - scale);			// "Back 2" pnt = behind current pnt
				q1 = scaledImage.ptr(y, x);					// "Back" pt = 1 ahead of current pnt
				q2 = scaledImage.ptr(y, x + scale);			// "Forward" pt = 2 ahead of current pnt
				q3 = scaledImage.ptr(y, x + 2 * scale);		// "Forward 2" pt = 3 ahead of current pnt
			}
			else if (x%scale != 0) {					// If not on a node line
				p = scaledImage.ptr(y, x);			// Pick out current points
				for (int i = 0; i < 3; i++) {
					p[i] = cubicInterp(scale, x, q0[i], q1[i], q2[i], q3[i]);
				}
			}
		}
	}
	// Cubic interoplation cant be done around edge points because we dont have enough information to get the slope here. 
	// Instead, we can linearly interpolate these points. 

	q0 = q1 = q2 = q3 = image.ptr(0, 0); //Reset first two interpolation pointers.

	// Linear interpolate end points, sweep left to right
	for (int y = 0; y < scaledImage.rows; y++) {
		q0 = scaledImage.ptr(y, 0);
		q1 = scaledImage.ptr(y, scale);
		q2 = scaledImage.ptr(y, scaledImage.cols - 2 * scale);
		q3 = scaledImage.ptr(y, scaledImage.cols - scale);
		for (int x = 1; x < scale; x++) {
			p = scaledImage.ptr(y, x); // Left side of image
			for (int i = 0; i < 3; i++) {
				p[i] = linInterp(scale, x, q0[i], q1[i]);
			}
			p = scaledImage.ptr(y, (x + scaledImage.cols - 2 * scale)); // Right side of image
			for (int color = 0; color < 3; color++) {
				p[color] = linInterp(scale, x, q2[color], q3[color]);
			}
		}
	}

	for (int x = 0; x < scaledImage.cols; x++) {
		q0 = scaledImage.ptr(0, x);
		q1 = scaledImage.ptr(scale, x);
		q2 = scaledImage.ptr(scaledImage.rows - 2 * scale, x);
		q3 = scaledImage.ptr(scaledImage.rows - scale, x);
		for (int y = 1; y < scale; y++) {
			p = scaledImage.ptr(y, x);
			for (int color = 0; color < 3; color++) {
				p[color] = linInterp(scale, y, q0[color], q1[color]);
			}
			p = scaledImage.ptr((y + scaledImage.rows - 2 * scale), x);
			for (int color = 0; color < 3; color++) {
				p[color] = linInterp(scale, y, q2[color], q3[color]);
			}
		}
	}

	imshow("Input Image", image);					// Show the original image
	image = scaledImage;
	imshow("Output Image", image);					// Show the transformed image
	return 0; // Return the output image
}