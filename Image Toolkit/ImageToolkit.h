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

Mat simpleScale(Mat image);
Mat linearScale(Mat image);
Mat cubicScale(Mat image);
Mat rotate90CW(Mat imageIn);
Mat rotate90CCW(Mat imageIn);


#endif