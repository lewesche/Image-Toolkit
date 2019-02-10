
#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>       
#include <cassert> 

using namespace std;
using namespace cv;
using namespace std::this_thread;
using namespace std::chrono_literals;

using image_t = Mat; // Define typedef for opencv image format

int convolution(image_t &image, int (&kernel)[3][3], int x, int y, int c)
{
	const int kSize = 3; 

	int value = 0;

	for (int i = 0; i < kSize; i++) {
		for (int j = 0; j < kSize; j++) {
			unsigned char *q; // Create pointer for pixels in output image
			q = image.ptr(y - 1 + j, x - 1 + i);			// Point towards pixel in original image

			value += kernel[i][j] * q[c];
		}
	}

	return value;
}



int gaussianBlur(image_t &image)
{
	unsigned char *p; // Create pointer for pixels in output image
	image_t imageOut(Size(image.cols, image.rows), CV_8UC3);

	//Using a kernel size of 3
	const int kSize = 3;

	//Fill in x and y kernels
	int k[kSize][kSize] =
	{
	{ 1, 2, 1}, // row 0
	{ 2, 4, 2}, // row 1
	{ 1, 2, 1} // row 2
	};

	for (int x = 1; x < image.cols - 1; x++) {
		for (int y = 1; y < image.rows - 1; y++) {
			p = imageOut.ptr(y, x);
			for (int c = 0; c < 3; c++) {
				int conv = convolution(image, k, x, y, c) / 16; // Divide by 16 to map between 0 and 255
				p[c] = conv;
			}
		}
	}

	imshow("Input Image", image);					// Show the original image
	image = imageOut;
	imshow("Output Image", image);					// Show the transformed image
	return 0;

}




int edgeDetection(image_t &image)
{
	unsigned char *p; // Create pointer for pixels in output image

	image_t imageOut(Size(image.cols, image.rows), CV_8UC3);
	image_t imageBW(Size(image.cols, image.rows), CV_8UC3);

	// Convert image to greyscale
	cvtColor(image, imageBW, COLOR_BGR2GRAY);

	//Using a kernel size of 3
	const int kSize = 3;

	//Fill in x and y kernels
	int k_x[kSize][kSize] =
	{
	{ -1, 0, 1}, // row 0
	{ -2, 0, 2}, // row 1
	{ -1, 0, 1} // row 2
	};

	int k_y[kSize][kSize] =
	{
	{ -1, -2, -1}, // row 0
	{ 0, 0, 0}, // row 1
	{ 1, 2, 1} // row 2
	};

	for (int x = 1; x < image.cols - 1; x++) {
		for (int y = 1; y < image.rows - 1; y++) {
			int conv_x = convolution(imageBW, k_x, x, y, 0) / 4; // Divide by 4 to map between 0 and 255
			int conv_y = convolution(imageBW, k_y, x, y, 0) / 4;

			// When in double sqrt(x^2 + y^2)
			double val = sqrt((conv_x * conv_x) + (conv_y * conv_y)) * 0.708; // 0.708 selected so that we dont assign color values over 255

			int c = static_cast<int>(val);
			assert(c <= 255 && "Color value exceeds 255" );

			p = imageOut.ptr(y, x);
			p[0] = static_cast<int>(val);	// B
			p[1] = static_cast<int>(val);	// G
			p[2] = static_cast<int>(val);	// R
		}
	}

	imshow("Input Image", image);					// Show the original image
	image = imageOut;
	imshow("Output Image", image);					// Show the transformed image
	return 0;

}