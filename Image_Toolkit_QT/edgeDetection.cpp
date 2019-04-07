#include "edgeDetection.h"
#include "convolution.h"
#include "math.h"

void edgeDetection(QImage *img)
{
    int w = img->width();
    int h = img->height();

    QImage greyImg = img->convertToFormat(QImage::Format_Grayscale8);
    QImage transformedImg = QImage(*img);

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

    int scl = 4;

    for(int x=1; x < w-1; x++)
    {
        for(int y=1; y < h-1; y++)
        {
            QColor xEdgeColor = convolution(greyImg, k_x, x, y, scl);
            QColor yEdgeColor = convolution(greyImg, k_y, x, y, scl);

            // 0.708 additional scaling selected so that we dont assign color values over 255
            double edgePixel = sqrt((xEdgeColor.red() * xEdgeColor.red()) + (yEdgeColor.red() * yEdgeColor.red())) * 0.708;
            QColor edgeColor = QColor(static_cast<int>(edgePixel), static_cast<int>(edgePixel), static_cast<int>(edgePixel));
            transformedImg.setPixelColor(x, y, edgeColor);
        }
    }
    //Re-package Image
    delete img;
    img = new QImage(transformedImg);
    return;
}

