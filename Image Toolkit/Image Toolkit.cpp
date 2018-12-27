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

using image_t = Mat; // Define typedef for opencv image format


int getUserInputInt(int min, int max) {
	int input;

	while (1) {
		cin >> input;

		if (std::cin.fail()) // has a previous extraction failed?
		{
			// yep, so let's handle the failure
			std::cin.clear(); // put us back in 'normal' operation mode
			std::cin.ignore(32767, '\n'); // and remove the bad input
		}
		else if (input >= min && input <= max){
			std::cin.ignore(32767, '\n');
			return input;
		}
		cout << "Please enter a valid selection..." << endl;
	}
}

int selectImage() {
	int selection;

	static bool firstRun(true);

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

	selection = getUserInputInt(0, 6);

	firstRun = false;

	return selection;
}

image_t loadImage(int selection, image_t imageBank[6]) {
	image_t image;

	while (1) {
		if (selection == 6) {
			string imagePath;
			cout << "enter the path to the image..." << endl;
			cin.ignore();
			getline(cin, imagePath);
			image_t tempImage = imread(imagePath);
			imwrite("temp.png", tempImage);
			image = imread("temp.png");

			return image;
		}
		else {
			image = imageBank[selection];

			return image;
		}
	}
}

int selectTransform() {
	int selection;

	cout << "Select transform..." << endl;
	cout << "(1) Resize" << endl;
	cout << "(2) Rotate 90 deg" << endl;
	cout << "(3) Encode Steganographic message" << endl;
	cout << "(4) Read Steganographic message" << endl;
	cout << "(5) Save Previous Image" << endl;

	selection = getUserInputInt(1, 5);

	return selection;
}

image_t runTransform(image_t image, int selection) {
	image_t imageTransformed;
	
	while (1) {
		switch (selection) {
			case 1: 
			{
				cout << "Select interpolation method..." << endl;
				cout << "(1) No added pixels" << endl;
				cout << "(2) Bi-linear interpolation" << endl;
				cout << "(3) Bi-cubic interpolation" << endl;
				int subSelection = getUserInputInt(1, 3);

				switch (subSelection) {
					case 1: 
					{
						imageTransformed = simpleScale(image);
						imshow("Output Image", imageTransformed);					// Show the transformed image
						return imageTransformed;
					}
					case 2:
					{
						imageTransformed = linearScale(image);
						imshow("Output Image", imageTransformed);					// Show the transformed image
						return imageTransformed;
						
					}
					case 3:
					{
						imageTransformed = cubicScale(image);
						imshow("Output Image", imageTransformed);					// Show the transformed image
						return imageTransformed;
					}
				}
			}
			case 2: 
			{
				cout << "Rotate clockwise or counter-clockwise?" << endl;
				cout << "(1) Clockwise" << endl;
				cout << "(2) Counter-clockwise" << endl;
				int subSelection = getUserInputInt(1, 2);
				switch (subSelection) 
				{
					case 1: {
						imageTransformed = rotate90CW(image);
						imshow("Output Image", imageTransformed);					// Show the transformed image
						return imageTransformed;
					}
					case 2: {
						imageTransformed = rotate90CCW(image);
						imshow("Output Image", imageTransformed);					// Show the transformed image
						return imageTransformed;
					}
				}
			}
			case 3: 
			{
				imageTransformed = encodeTxt(image);
				imshow("Output Image", imageTransformed);					// Show the transformed image
				return imageTransformed;
			}
			case 4: 
			{
				string text = decodeTxt(image);
				cout << "Hidden text: " << endl << text << endl;
				imageTransformed = image;
				return imageTransformed;
			}
			case 5: 
			{
				imageTransformed = image;
				string imageName;
				cout << "Name of image file?" << endl;
				cin.ignore();
				getline(cin, imageName);
				imwrite((imageName + ".png"), image);
				return imageTransformed;
			}
		}
	}
}

int main()
{
	image_t image;										// Container for input images
	image_t imageTransformed;							// Container for transformed image
	image_t dude = imread("dude.png");					// Load simple black and white "dude" image
	image_t lady = imread("lady.png");					// Load a more complex black and white "lady" image
	image_t dog = imread("dog.png");					// Load a colorful pup
	image_t cat = imread("cat.png");					// Load cat photo
	image_t bird = imread("bird.png");					// Load bird photo
	image_t imageBank[] { imageTransformed, dude, lady, dog, cat, bird }; // Container for set of images

	image_t *imageBankPtr[] { &imageTransformed, &dude, &lady, &dog, &cat, &bird };
	cout << imageBankPtr[0] << endl;
	cout << imageBankPtr[1] << endl;

	image = loadImage(selectImage(), imageBank);						// Select and load image

	while(1) {
										
		imageTransformed = runTransform(image, selectTransform());		// Select and run action
		imshow("Input Image", image);									// Show the original image
		imshow("pointer pic", *imageBankPtr[1]);

		imageBank[0] = imageTransformed;								// Store the transformed image under previous image and 

		cout << "Press any key to close and continue..." << endl;		// Prompt user to close image windows
		waitKey(0);
		destroyAllWindows();											// Close Windows
		cout << "More transforms? (y/n)" << endl;						// Ask if user wants to continue
		char selection;
		cin >> selection;												// Get input from the user
		std::cin.ignore(32767, '\n');
		if (selection == 'n') {											// If user inputs 'n'...
			return 0;													// End the program
		}															
		else {															// If the user inputs anything else...
			image = loadImage(selectImage(), imageBank);					// Select a new image (or previous)
		}												
										
	}
}
