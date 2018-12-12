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


int main()
{
	int scale;
	int selection;
	char input;
	bool keepGoin = true;

	Mat image;										// Container for input images
	Mat imageTransformed;							// Container for transformed images
	Mat dude = imread("dude.png");					// Load simple black and white "dude" image
	Mat lady = imread("lady.png");					// Load a more complex black and white "lady" image
	Mat dog = imread("dog.png");					// Load a colorful pup
	Mat cat = imread("cat.png");					// Load cat photo
	Mat bird = imread("bird.png");					// Load bird photo
	Mat imageBank[6] = { imageTransformed, dude, lady, dog, cat, bird };


	cout << "Select image..." << endl;
	cout << "(1) Dude" << endl;
	cout << "(2) Lady" << endl;
	std::cout << "(3) Dog" << endl;
	std::cout << "(4) Cat" << endl;
	std::cout << "(5) Bird" << endl;
	cin >> selection;

	image = imageBank[selection];


	while (keepGoin == true) {
		cout << "Select transform..." << endl;
		cout << "(1) Sparse" << endl;
		cout << "(2) Linear" << endl;
		cout << "(3) Cubic" << endl;
		cout << "(4) Rotate Clockwise" << endl;
		cout << "(5) Rotate Counter-Clockwise" << endl;
		cin >> selection;

		if (selection == 1 || selection == 2 || selection == 3) {
			cout << "Scale Factor?" << endl;
			cin >> scale;
		}


		if (selection == 1) {
			imageTransformed = simpleScale(scale, image);
		}
		else if (selection == 2) {
			imageTransformed = linearScale(scale, image);
		}
		else if (selection == 3) {
			imageTransformed = cubicScale(scale, image);
		}
		else if (selection == 4) {
			imageTransformed = rotate90CW(image);
		}
		else if (selection == 5) {
			imageTransformed = rotate90CCW(image);
		}

		imageBank[0] = imageTransformed;
		imshow("Input Image", image);
		imshow("Output Image", imageTransformed);

		cout << "Press any key to close and continue..." << endl;
		waitKey(0);
		destroyAllWindows();
		cout << "More transforms? (y/n)" << endl;
		cin >> input;
		if (input == 'n') {
			keepGoin = false;
		}
		else {
			cout << "Select image..." << endl;
			cout << "(0) Previous image" << endl;
			cout << "(1) Dude" << endl;
			cout << "(2) Lady" << endl;
			cout << "(3) Dog" << endl;
			cout << "(4) Cat" << endl;
			cout << "(5) Bird" << endl;
			cin >> selection;
		}
		image = imageBank[selection];
	}
	waitKey(0);
	return 0;
}
