#include <QImage>
#include "rotation.h"
#include "math.h"
#include <QDebug>

void rotateImg(QImage *img, int degrees)
{
    double rad = static_cast<double>(degrees)/180 * 3.14159;
    //Create a container for new coords to pass by reference to transform function...lazy
    int newCoords[2];

    int w0 = img->width();
    int h0 = img->height();

    //Determine the minimum enclosing rectangle to size the new image
    minEnclosingRectangle(w0, h0, rad, newCoords);
    int w = newCoords[0] + 1;
    int h = newCoords[1] + 1;

    //For some reason creating an image from scratch with the desired dimensions and format was causing strange bugs, so scaling and copying is a workaround.
    QImage transformedImg = img->scaled(w, h, Qt::IgnoreAspectRatio, Qt::FastTransformation);

    transformedImg.fill(qRgba(0, 0, 0, 0));

    QRgb pixel;

    for(int x0 = 0; x0 < w0; x0++)
    {
        for(int y0 = 0; y0 < h0; y0++)
        {
            rotateCoordinates(x0-w0/2, y0-h0/2, rad, newCoords);

            int x;
            int y;

            if(x0 == w0/2 && y0 == h0/2) //Skip the center pixel
                continue;
            else if(x0 >=w0/2)
            {
                x = newCoords[0] + w/2;
                y = newCoords[1] + h/2;
            }
            else
            {
                x = -newCoords[0] + w/2;
                y = -newCoords[1] + h/2;
            }
            pixel=img->pixel(x0, y0);

            transformedImg.setPixelColor(x, y, pixel);
        }
    }


    //Re-package Image
    if(img != nullptr)
        delete img;
    img = new QImage(transformedImg);
    return;
}

void minEnclosingRectangle(int w0, int h0, double rad, int (&newCoords)[2])
{
    int quad1[2];
    int quad2[2];

    int w; int h;

    rotateCoordinates(w0/2, h0/2, rad, quad1);
    rotateCoordinates(-w0/2, h0/2, rad, quad2);

    if(abs(quad1[0]) > abs(quad2[0]))
        w = 2*abs(quad1[0]);
    else
        w = 2*abs(quad2[0]);

    if(abs(quad1[1]) > abs(quad2[1]))
        h = 2*abs(quad1[1]);
    else
        h = 2*abs(quad2[1]);

    newCoords[0] = w;
    newCoords[1] = h;
}


void rotateCoordinates(int x, int y, double rad, int (&newCoords)[2])
{
    newCoords[0] = static_cast<int>(round(sqrt(x*x + y*y) * cos(atan(static_cast<double>(y)/x)+rad)));
    newCoords[1] = static_cast<int>(round(sqrt(x*x + y*y) * sin(atan(static_cast<double>(y)/x)+rad)));
}


