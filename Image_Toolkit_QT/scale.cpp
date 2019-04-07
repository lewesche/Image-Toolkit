//#include<QPixmap>
#include<QImage>
//#include<QRgb>
//#include <math.h>

#include "scale.h"

void scaleImg(QImage *img, double scale, MainWindow::InterpolationFlags::Algo flags)
{


    int w0 = img->width();
    int h0 = img->height();

    int w = static_cast<int>(w0*scale);
    int h = static_cast<int>(h0*scale);

    //Reduce looping range. When scale is small this may cut off 1 or 2 pixels
    while(static_cast<int>((h0-1)*scale) >= h)
        h0--;
    while(static_cast<int>((w0-1)*scale) >= w)
        w0--;

    //qDebug() << w << " x " << h;

    //For some reason creating an image from scratch with the desired dimensions and format was causing strange bugs, so scaling and copying is a workaround.
    QImage transformedImg = img->scaled(w, h, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    transformedImg.fill(QColor(100, 0, 0));

    //Copy over original pixels to new positions
    for(int x = 0; x < w0; x++)
    {
        for(int y = 0; y < h0; y++)
        {
            QRgb originalPixel= img->pixel(x, y);
            transformedImg.setPixelColor(static_cast<int>(x*scale), static_cast<int>(y*scale), originalPixel);
        }
    }

    //Determine position of determinate pixels

    //Fill in new pixels
    if(scale > 1.0)
    {
        //Record position of determinate pixels
        std::vector<int> x_positions;
        std::vector<int> y_positions;
        for(int x = 0; x < w0; x++)
            x_positions.push_back(static_cast<int>(x*scale));

        for(int y = 0; y < h0; y++)
            y_positions.push_back(static_cast<int>(y*scale));

        if(flags == MainWindow::InterpolationFlags::CUBIC)
        {
            //First fill in pixels to form horizontal lines
            for(unsigned long j = 1; j < y_positions.size()-1; j++)
                for(unsigned long i = 2; i < x_positions.size()-1; i++){
                    xInterpolateCubic(x_positions[i-2], x_positions[i-1], x_positions[i], x_positions[i+1], y_positions[j], transformedImg);
                }

            //Fill in between horizontal lines
            for(int x = 1; x < w-1; x++)
                for(unsigned long i = 2; i < y_positions.size()-1; i++)
                    yInterpolateCubic(x_positions[i-2], x_positions[i-1], x_positions[i], x_positions[i+1], x, transformedImg);
        }

        if(flags == MainWindow::InterpolationFlags::LINEAR)
        {
            //First fill in pixels to form horizontal lines
            for(unsigned long j = 0; j < y_positions.size(); j++)
                for(unsigned long i = 1; i < x_positions.size(); i++)
                    xInterpolateLinear(x_positions[i-1], x_positions[i], y_positions[j], transformedImg);


            //Fill in between horizontal lines
            for(int x = 0; x < w; x++)
                for(unsigned long i = 1; i < y_positions.size(); i++)
                    yInterpolateLinear(y_positions[i-1], y_positions[i], x, transformedImg);
        }
    }

    //Re-package Image
    if(img != nullptr)
        delete img;
    img = new QImage(transformedImg);
    return;
}

void xInterpolateLinear(int backPosition, int forwardPosition, int y, QImage &transformedImg)
{
    int d = forwardPosition - backPosition;
    if(d == 1)
        return;
    else{
        QRgb backPixel = transformedImg.pixel(backPosition, y);
        QRgb frontPixel = transformedImg.pixel(forwardPosition, y);
        for(int p = 1; p < d; p++)
        {
            QColor color;
            color = linearInterpolate(backPixel, frontPixel, d, p);

            transformedImg.setPixelColor(backPosition+p, y, color);
        }
    }
}

void yInterpolateLinear(int backPosition, int forwardPosition, int x, QImage &transformedImg)
{
    int d = forwardPosition - backPosition;
    if(d == 1)
        return;
    else{
        QRgb backPixel = transformedImg.pixel(x, backPosition);
        QRgb frontPixel = transformedImg.pixel(x, forwardPosition);
        for(int p = 1; p < d; p++)
        {
            QColor color;
            color = linearInterpolate(backPixel, frontPixel, d, p);

            transformedImg.setPixelColor(x, backPosition+p, color);
        }
    }
}

QColor linearInterpolate(QRgb backPixel, QRgb frontPixel, int d, int p)
{
    int r = (qRed(backPixel)*(d-p) + qRed(frontPixel)*p)/d;
    int g = (qGreen(backPixel)*(d-p) + qGreen(frontPixel)*p)/d;
    int b = (qBlue(backPixel)*(d-p) + qBlue(frontPixel)*p)/d;
    return QColor(r, g, b);
}

void xInterpolateCubic(int back2Position, int backPosition, int forwardPosition, int forward2Position, int y, QImage &transformedImg)
{
    int d = forwardPosition - backPosition;
    if(d == 1)
        return;
    QRgb back2Pixel = transformedImg.pixel(back2Position, y);
    QRgb backPixel = transformedImg.pixel(backPosition, y);
    QRgb frontPixel = transformedImg.pixel(forwardPosition, y);
    QRgb front2Pixel = transformedImg.pixel(forward2Position, y);

    int d0 = backPosition - back2Position;
    int backSlope[3];
    backSlope[0] = (qRed(backPixel) - qRed(back2Pixel))/d0;
    backSlope[1] = (qGreen(backPixel) - qGreen(back2Pixel))/d0;
    backSlope[2] = (qBlue(backPixel) - qBlue(back2Pixel))/d0;

    int d1 = forward2Position - forwardPosition;
    int frontSlope[3];
    frontSlope[0] = (qRed(front2Pixel) - qRed(frontPixel))/d1;
    frontSlope[1] = (qGreen(front2Pixel) - qGreen(frontPixel))/d1;
    frontSlope[2] = (qBlue(front2Pixel) - qBlue(frontPixel))/d1;

    for(int p = 1; p < d; p++)
    {
        QColor color;
        color = cubicInterpolate(backSlope, backPixel, frontPixel, frontSlope, d, p);

        transformedImg.setPixelColor(backPosition+p, y, color);
    }
}

void yInterpolateCubic(int back2Position, int backPosition, int forwardPosition, int forward2Position, int x, QImage &transformedImg)
{
    int d = forwardPosition - backPosition;
    if(d == 1)
        return;
    QRgb back2Pixel = transformedImg.pixel(x, back2Position);
    QRgb backPixel = transformedImg.pixel(x, backPosition);
    QRgb frontPixel = transformedImg.pixel(x, forwardPosition);
    QRgb front2Pixel = transformedImg.pixel(x, forward2Position);

    int d0 = backPosition - back2Position;
    int backSlope[3];
    backSlope[0] = (qRed(backPixel) - qRed(back2Pixel))/d0;
    backSlope[1] = (qGreen(backPixel) - qGreen(back2Pixel))/d0;
    backSlope[2] = (qBlue(backPixel) - qBlue(back2Pixel))/d0;

    int d1 = forward2Position - forwardPosition;
    int frontSlope[3];
    frontSlope[0] = (qRed(front2Pixel) - qRed(frontPixel))/d1;
    frontSlope[1] = (qGreen(front2Pixel) - qGreen(frontPixel))/d1;
    frontSlope[2] = (qBlue(front2Pixel) - qBlue(frontPixel))/d1;

    for(int p = 1; p < d; p++)
    {
        QColor color;
        color = cubicInterpolate(backSlope, backPixel, frontPixel, frontSlope, d, p);

        transformedImg.setPixelColor(x, backPosition+p, color);
    }
}

QColor cubicInterpolate(int (&backSlope)[3], QRgb backPixel, QRgb frontPixel, int (&frontSlope)[3], int d, int p)
{
    double r;
    double g;
    double b;

    //https://en.wikipedia.org/wiki/Cubic_Hermite_spline

    double t = static_cast<double>(p)/static_cast<double>(d);

    r = static_cast<double>(qRed(backPixel))* (2.0*t*t*t- 3.0*t*t+ 1) + static_cast<double>(backSlope[0])* (t*t*t- 2.0*t*t+ t) + static_cast<double>(qRed(frontPixel))* (3.0*t*t- 2.0*t*t*t) + static_cast<double>(frontSlope[0])* (t*t*t-t*t);
    g = static_cast<double>(qGreen(backPixel))* (2.0*t*t*t- 3.0*t*t+ 1) + static_cast<double>(backSlope[1])* (t*t*t- 2.0*t*t+ t) + static_cast<double>(qGreen(frontPixel))* (3.0*t*t- 2.0*t*t*t) + static_cast<double>(frontSlope[1])* (t*t*t-t*t);
    b = static_cast<double>(qBlue(backPixel))* (2.0*t*t*t- 3.0*t*t+ 1) + static_cast<double>(backSlope[2])* (t*t*t- 2.0*t*t+ t) + static_cast<double>(qBlue(frontPixel))* (3.0*t*t- 2.0*t*t*t) + static_cast<double>(frontSlope[2])* (t*t*t-t*t);

    if(r > 255)
        r = 255;
    if(g > 255)
        g = 255;
    if(b > 255)
        b = 255;

    if(r < 0)
        r = 0;
    if(g < 0)
        g = 0;
    if(b < 0)
        b = 0;

    QColor result = QColor(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b));
    return result;
}
