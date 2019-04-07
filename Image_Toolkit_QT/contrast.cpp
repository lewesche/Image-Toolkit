#include "contrast.h"
#include "math.h"


void contrastImg(QImage *img, double contrast)
{

    QRgb pixel;
    QColor transformedColor;

    for(int x = 0; x < img->width(); x++)
    {
        for(int y = 0; y < img->height(); y++)
        {
            pixel = img->pixel(x, y);

            transformedColor = contrastPixel(pixel, contrast);
            img->setPixelColor(x, y, transformedColor);
        }
    }
}

QColor contrastPixel(QRgb pixel, double contrast)
{

    double r = static_cast<double>(qRed(pixel));
    double g = static_cast<double>(qGreen(pixel));
    double b = static_cast<double>(qBlue(pixel));

    if(r < 128)
        r = 127.5 * pow(r/127.5, contrast);
    else
        r = 255.0 - 127.5 * pow((255-r)/127.5, contrast);

    if(g < 128)
        g = 127.5 * pow(g/127.5, contrast);
    else
        g = 255.0 - 127.5 * pow((255-g)/127.5, contrast);

    if(b < 128)
        b = 127.5 * pow(b/127.5, contrast);
    else
        b = 255.0 - 127.5 * pow((255-b)/127.5, contrast);

    QColor transformedPixel = QColor(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b));
    return transformedPixel;
}
