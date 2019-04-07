#ifndef SCALE_H
#define SCALE_H

#include <QImage>
#include "mainwindow.h"

void scaleImg(QImage *img, double scale, MainWindow::InterpolationFlags::Algo flags);

void xInterpolateLinear(int backPosition, int forwardPosition, int y, QImage &transformedImg);

void yInterpolateLinear(int backPosition, int forwardPosition, int x, QImage &transformedImg);

QColor linearInterpolate(QRgb backPixel, QRgb frontPixel, int d, int p);

void xInterpolateCubic(int back2Position, int backPosition, int forwardPosition, int forward2Position, int y, QImage &transformedImg);

void yInterpolateCubic(int back2Position, int backPosition, int forwardPosition, int forward2Position, int y, QImage &transformedImg);

QColor cubicInterpolate(int (&backSlope)[3], QRgb backPixel, QRgb frontPixel, int (&frontSlope)[3], int d, int p);

#endif // SCALE_H
