#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>       
#include <string>

using namespace std;
using namespace cv;
using namespace std::this_thread;
using namespace std::chrono_literals;

Mat encodeTxt(Mat imageIn) {
	Mat transformedImage = imageIn;						// Container for new image
	unsigned char * p;									// Creat pointer for pixels

	const unsigned char bit0 = 0b0000'0001;	// modifier for bit 0, least signifigant bit
	const unsigned char bit1 = 0b0000'0010;	// modifier for bit 1
	const unsigned char bit2 = 0b0000'0100; // modifier for bit 2
	const unsigned char bit3 = 0b0000'1000; // etc
	const unsigned char bit4 = 0b0001'0000; 
	const unsigned char bit5 = 0b0010'0000; 
	const unsigned char bit6 = 0b0100'0000; 
	const unsigned char bit7 = 0b1000'0000; 
	
	unsigned char bits[8] = { bit0, bit1, bit2, bit3, bit4, bit5, bit6, bit7 };

	string text;	// Container for input text to encode
	cout << "Enter a string of text to enconde..." << endl;
	cin.ignore();
	getline(cin, text); // Grab text from the user
	char endReturn = 13;
	text += endReturn;
	cout << text << endl;

	int textLength = text.length();	// Determine the number of characters
	
	int imageWidth = imageIn.cols;		// Width of the image in pixels
	int imageHeight = imageIn.rows;		// Height of the image in pixels

	int x = 0;	// x pixel position
	int y = 0;	// y pixel position 

	int color = 0; // Start by modifiying the first color pixel. Modifies the other pixels if necessary. 

	for (int i = 0; i < textLength; i++) {
		for (int bitCount = 0; bitCount < 8; bitCount++) {
			p = transformedImage.ptr(y, x);			// Select a pixel
			cout << "Character: " << text[i];
			cout << ", bit: " << bitCount;
			cout << ", x position: " << x;
			cout << ", y position: " << y;
			if (bits[bitCount] & text[i]) {
				 cout << ", bit:" << 1 << endl;
				p[color] |= bit0;
			}
			if (!(bits[bitCount] & text[i])) {
				 cout << ", bit:" << 0 << endl;
				p[color] &= ~bit0;
			}
			x++;
			if (x >= imageWidth) {
				x = 0;
				y = y++;
			}
			if (y >= imageHeight) {
				color++;
			}
			if (color >= 3) {
				cout << "Out of image space!" << endl;
				return transformedImage;
			}
		}
	}

	return transformedImage;
}

String decodeTxt(Mat imageIn) {
	unsigned char * p;									// Creat pointer for pixels

	const unsigned char bit0 = 0b0000'0001;	// modifier for bit 0, least signifigant bit
	const unsigned char bit1 = 0b0000'0010;	// modifier for bit 1
	const unsigned char bit2 = 0b0000'0100; // modifier for bit 2
	const unsigned char bit3 = 0b0000'1000; // etc
	const unsigned char bit4 = 0b0001'0000;
	const unsigned char bit5 = 0b0010'0000;
	const unsigned char bit6 = 0b0100'0000;
	const unsigned char bit7 = 0b1000'0000;

	unsigned char bits[8] = { bit0, bit1, bit2, bit3, bit4, bit5, bit6, bit7 };

	string text;	// Container for input text to encode
	char letter;

	int imageWidth = imageIn.cols;		// Width of the image in pixels
	int imageHeight = imageIn.rows;		// Height of the image in pixels

	int x = 0;	// x pixel position
	int y = 0;	// y pixel position 

	int color = 0; // Start by modifiying the first color pixel. Modifies the other pixels if necessary. 


	for (int i = 0; i < (imageWidth * imageHeight * 3); i++) {
		for (int bitCount = 0; bitCount < 8; bitCount++) {
			p = imageIn.ptr(y, x);		// Select a pixel

			if (bit0 & p[color]) {
				letter |= bits[bitCount];
			}
			if (!(bit0 & p[color])) {
				letter &= ~bits[bitCount];
			}


			x++;
			if (x >= imageWidth) {
				x = 0;
				y = y++;
			}
			if (y >= imageHeight) {
				color++;
			}
			if (color >= 3) {
				cout << "Reached the end of the image!" << endl;
				return text;
			}
		}
		if (static_cast<int>(letter) == 13) {
			return text;
		}
		else {
			text += letter;
		}
		
	}
}