#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include<QImage>

QColor convolution(QImage &transformedImg, int (&kernel)[3][3], int x, int y, int scl);

#endif // CONVOLUTION_H
