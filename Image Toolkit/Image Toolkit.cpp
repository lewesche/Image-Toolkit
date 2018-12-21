#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include "ImageToolkit.h"

using namespace std;
using namespace cv;
using namespace std::this_thread;
using namespace std::chrono_literals;

int selectImage(bool firstRun) {
	int selection;

	cout << "Select image..." << endl;
	if (firstRun == false) {
		cout << "(0) Previous image" << endl;
	}
	cout << "(1) Dude" << endl;
	cout << "(2) Lady" << endl;
	cout << "(3) Dog" << endl;
	cout << "(4) Cat" << endl;
	cout << "(5) Bird" << endl;
	cout << "(6) Another Image" << endl;
	cin >> selection;

	return selection;
}

Mat pullImage(int selection, Mat imageBank[6]) {
	Mat image;

	if (selection < 6) {
		image = imageBank[selection];
	} 
	else if (selection = 6) {
		string imagePath;
		cout << "enter the path to the image..." << endl; 
		cin.ignore();
		getline(cin, imagePath); 
		Mat tempImage = imread(imagePath); 
		imwrite("temp.png", tempImage);
		image = imread("temp.png");
	}

	return image;
}

int selectTransform() {
	int selection;

	cout << "Select transform..." << endl;
	cout << "(1) Resize" << endl;
	cout << "(2) Rotate 90 deg" << endl;
	cout << "(3) Encode Steganographic message" << endl;
	cout << "(4) Read Steganographic message" << endl;
	cout << "(5) Save Previous Image" << endl;
	cin >> selection;

	return selection;
}

Mat runTransform(Mat image, int selection) {
	Mat imageTransformed;
	
	if (selection == 1) {
		int subSelection;
		cout << "Select interpolation method..." << endl;
		cout << "(1) No added pixels" << endl;
		cout << "(2) Bi-linear interpolation" << endl;
		cout << "(3) Bi-cubic interpolation" << endl;
		cin >> subSelection;
		if (subSelection == 1) {
			imageTransformed = simpleScale(image);
		}
		else if (subSelection == 2) {
			imageTransformed = linearScale(image);
		}
		else if (subSelection == 3) {
			imageTransformed = cubicScale(image);
		}
	}
	else if (selection == 2) {
		int subSelection;
		cout << "Rotate clockwise or counter-clockwise?" << endl;
		cout << "(1) Clockwise" << endl;
		cout << "(2) Counter-clockwise" << endl;
		cin >> subSelection;
		if (subSelection == 1) {
			imageTransformed = rotate90CW(image);
		}
		else if (subSelection == 2) {
			imageTransformed = rotate90CCW(image);	
		}
	}
	else if (selection == 3) {
		imageTransformed = encodeTxt(image);

	}
	else if (selection == 4) {
		string text = decodeTxt(image);
		cout << text << endl;
		imageTransformed = image;
	}
	else if (selection == 5) {
		imageTransformed = image;
		string imageName;
		cout << "Name of image file?" << endl;
		cin.ignore();
		getline(cin, imageName);
		imwrite((imageName + ".png"), image);
	}

	return imageTransformed;
}

int main()
{
	char selection;
	bool keepGoin{ true };
	bool firstRun = true;

	Mat image;										// Container for input images
	Mat imageTransformed;							// Container for transformed images
	Mat dude = imread("dude.png");					// Load simple black and white "dude" image
	Mat lady = imread("lady.png");					// Load a more complex black and white "lady" image
	Mat dog = imread("dog.png");					// Load a colorful pup
	Mat cat = imread("cat.png");					// Load cat photo
	Mat bird = imread("bird.png");					// Load bird photo
	Mat imageBank[6] = { imageTransformed, dude, lady, dog, cat, bird }; // Container for set of images


	selection = selectImage(firstRun);								// Select an image
	image = pullImage(selection, imageBank);

	firstRun = false;												// Flip boolean to allow for previous image to be selected on next cycle

	while (keepGoin == true) {

		selection = selectTransform();								// Select a transform operation

		imageTransformed = runTransform(image, selection);			// Transform Image

		imageBank[0] = imageTransformed;							// Store the transformed image
		imshow("Input Image", image);								// Show the original image
		imshow("Output Image", imageTransformed);					// Show the transformed image

		cout << "Press any key to close and continue..." << endl;	// Prompt user to close image windows
		waitKey(0);
		destroyAllWindows();										// Close Windows
		cout << "More transforms? (y/n)" << endl;					// Ask if user wants to continue
		cin >> selection;												// Get input from the user
		if (selection == 'n') {											// If user inputs 'n'...
			keepGoin = false;										// Flip boolean to false to break while loop
		}															
		else {														// If the user inputs anything else...
			selection = selectImage(firstRun);	
			image = pullImage(selection, imageBank);				// Select a new image (or previous)
		}												
										
	}

	return 0;
}

