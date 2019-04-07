#ifndef CONTRAST_H
#define CONTRAST_H

#include<QImage>

void contrastImg(QImage *img, double contrast);

QColor contrastPixel(QRgb pixel, double contrast);

#endif // CONTRAST_H
