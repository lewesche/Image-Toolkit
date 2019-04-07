#ifndef ROTATION_H
#define ROTATION_H

#include <QImage>

void rotateImg(QImage *img, int degrees);

void minEnclosingRectangle(int w0, int h0, double rad, int (&newCoords)[2]);

void rotateCoordinates(int x, int y, double rad, int (&newCoords)[2]);

#endif // ROTATION_H
