#ifndef ImageToolkit_H
#define ImageToolkit_H

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

int simpleScale(image_t &imageIn);
int linearScale(image_t &imageIn);
int cubicScale(image_t &imageIn);
int rotate90CW(image_t &imageIn);
int rotate90CCW(image_t &imageIn);
int encodeTxt(image_t &imageIn);
string decodeTxt(image_t &imageIn);

#endif