#include "convolution.h"

QColor convolution(QImage &transformedImg, int (&kernel)[3][3], int x, int y, int scl)
{

    QRgb pixel;// = img.pixel(x, y);
    int r = 0;
    int g = 0;
    int b = 0;

    for(int i=0; i<3; i++)
    {
        for(int j=0; j<3; j++)
        {
            pixel = transformedImg.pixel(x-1+i, y-1+j);
            r += qRed(pixel) * kernel[i][j];
            g += qGreen(pixel) * kernel[i][j];
            b += qBlue(pixel) * kernel[i][j];
        }
    }
    r=abs(r/scl);
    g=abs(g/scl);
    b=abs(b/scl);

    QColor convolutedPixel = QColor(r, g, b);
    return convolutedPixel;
}
