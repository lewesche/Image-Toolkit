
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

double getContrastValue()
{
	double contrastValue;

	cout << "Contrast value? Enter a float >= 0." << endl;

	while (1) {
		cin >> contrastValue;

		if (std::cin.fail()) // has a previous extraction failed?
		{
			// yep, so let's handle the failure
			std::cin.clear(); // put us back in 'normal' operation mode
			std::cin.ignore(32767, '\n'); // and remove the bad input
		}
		else if (contrastValue >= 0.0) {
			return contrastValue;
		}
		cout << "Please enter a valid selection..." << endl;
	}
	return contrastValue;
}


int applyContrast(double pixelValue, double contrastValue)
{
	assert(contrastValue >= 0 && "contrast value must be positive");
	
	if (pixelValue >= 128)
	{
		pixelValue = -127.5 * pow(abs((pixelValue - 255) / 127.5), contrastValue) + 255;
		
		return static_cast<int>(round(pixelValue));
	}
	else if (pixelValue < 128)
	{
		pixelValue = 127.5*pow((pixelValue / 127.5), contrastValue);

		return static_cast<int>(round(pixelValue));
	}
}

int setImageContrast(image_t &image)
{
	double contrastValue = getContrastValue();
	
	unsigned char *p; // Create pointer for pixels in image

	imshow("Input Image", image);					// Show the original image

	for (int x = 0; x < image.cols; x++)
	{
		for (int y = 0; y < image.rows; y++)
		{
			p = image.ptr(y, x);
			for (int c = 0; c < 3; c++)
			{
				p[c] = applyContrast(p[c], contrastValue);
			}
		}
	}

	

	imshow("Output Image", image);					// Show the transformed image
	return 0;
}