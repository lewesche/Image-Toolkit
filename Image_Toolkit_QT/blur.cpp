#include "blur.h"
#include "convolution.h"

void blurImg(QImage *img, int iterations)
{
    int w = img->width();
    int h = img->height();

    QImage transformedImg = QImage(*img);

    //Using a kernel size of 3
    const int kSize = 3;

    //Fill in kernal
    int kernel[kSize][kSize] =
    {
    { 1, 2, 1}, // row 0
    { 2, 4, 2}, // row 1
    { 1, 2, 1} // row 2
    };

    //Sum of kernel
    int scl = 16;

    for(int itCount = 0; itCount < iterations; itCount++)
    {
        for(int x=1; x < w-1; x++)
        {
            for(int y=1; y < h-1; y++)
            {
                QColor blurredColor = convolution(transformedImg, kernel, x, y, scl);
                transformedImg.setPixelColor(x, y, blurredColor);
            }
        }
    }

    //Re-package Image
    delete img;
    img = new QImage(transformedImg);
    return;
}
