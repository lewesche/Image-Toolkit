#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QPixmap>
#include<QImage>
#include<QDir>
#include<QMessageBox>
#include<QRgb>
#include<QDebug>
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboBox_img->addItem("dude");
    ImgDirectories.push_back(":/new/resource/Default_Img/dude.png");
    ui->comboBox_img->addItem("lady");
    ImgDirectories.push_back(":/new/resource/Default_Img/lady.png");
    ui->comboBox_img->addItem("dog");
    ImgDirectories.push_back(":/new/resource/Default_Img/dog.png");
    ui->comboBox_img->addItem("cat");
    ImgDirectories.push_back(":/new/resource/Default_Img/cat.png");
    ui->comboBox_img->addItem("bird");
    ImgDirectories.push_back(":/new/resource/Default_Img/bird.png");

    QImage imgTemp = QImage(ImgDirectories[0]);
    imgTemp = imgTemp.convertToFormat(QImage::Format_RGB32);
    img = new QImage(imgTemp);



    ui->comboBox_scale_method->addItem("Linear");
    ui->comboBox_scale_method->addItem("Cubic");

    InterpFlag = static_cast<MainWindow::InterpolationFlags::Algo>(0);

    drawImg();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadNewImg(int index)
{
    delete img;
    QImage imgTemp = QImage(ImgDirectories[static_cast<unsigned long>(index)]);
    imgTemp = imgTemp.convertToFormat(QImage::Format_RGB32);
    img = new QImage(imgTemp);
}

void MainWindow::drawImg()
{
    QPixmap pix = QPixmap::fromImage(*img);
    ui->label->setGeometry(350, 0, img->width(), img->height());
    ui->label->setPixmap(pix);
}

void MainWindow::on_comboBox_img_activated(int index)
{
    loadNewImg(index);

    updateImg();
}

void MainWindow::on_horizontalSlider_scale_valueChanged(int value)
{
    double scale = value/10.0;
    ui->lineEdit_scale->setText(QString::number(scale));

    updateImg();
}

void MainWindow::on_lineEdit_scale_returnPressed()
{
    QString scaleTxt = ui->lineEdit_scale->text();
    double scale = scaleTxt.toDouble();

    if(scale > 4.0)
        scale = 4.0;

    ui->lineEdit_scale->setText(QString::number(scale));
    ui->horizontalSlider_scale->setValue(static_cast<int>(scale*10));

    updateImg();
}

void MainWindow::on_comboBox_scale_method_currentIndexChanged()
{
    updateImg();
}

void MainWindow::on_horizontalSlider_contrast_valueChanged(int contrast)
{
    ui->lineEdit_contrast->setText(QString::number(static_cast<double>(contrast)/10.0));

    updateImg();
}

void MainWindow::on_lineEdit_contrast_returnPressed()
{
    QString contrastTxt = ui->lineEdit_contrast->text();
    double contrast = contrastTxt.toDouble();

    ui->horizontalSlider_contrast->setValue(static_cast<int>(contrast*10));

    updateImg();
}

void MainWindow::on_lineEdit_blur_returnPressed()
{
    QString scaleTxt = ui->lineEdit_blur->text();
    int iterations = scaleTxt.toInt();

    if(iterations > 10)
        iterations = 10;

    ui->lineEdit_blur->setText(QString::number(iterations));
    ui->horizontalSlider_blur->setValue(iterations);

    updateImg();
}

void MainWindow::on_horizontalSlider_blur_valueChanged(int iterations)
{
    ui->lineEdit_blur->setText(QString::number(iterations));

    updateImg();
}

void MainWindow::on_radioButton_edge_clicked()
{
    updateImg();
}

void MainWindow::updateImg()
{
    scaleImg();
    contrastImg();
    gaussianBlur();
    edgeDetection();
    drawImg();
}





void MainWindow::scaleImg()
{
    //Refresh Original Image
    loadNewImg(ui->comboBox_img->currentIndex());

    //Read scale + flags
    QString scaleTxt = ui->lineEdit_scale->text();
    double scale = scaleTxt.toDouble();

    if(scale == 1.0)
        return;

    MainWindow::InterpolationFlags::Algo flags = static_cast<MainWindow::InterpolationFlags::Algo>(ui->comboBox_scale_method->currentIndex());

    qDebug() << "in scaleImg - " << flags;

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

    QImage transformedImg(w, h, QImage::Format_RGB32);

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
    delete img;
    img = new QImage(transformedImg);
}

void MainWindow::xInterpolateLinear(int backPosition, int forwardPosition, int y, QImage &transformedImg)
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

void MainWindow::yInterpolateLinear(int backPosition, int forwardPosition, int x, QImage &transformedImg)
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

QColor MainWindow::linearInterpolate(QRgb backPixel, QRgb frontPixel, int d, int p)
{
    int r = (qRed(backPixel)*(d-p) + qRed(frontPixel)*p)/d;
    int g = (qGreen(backPixel)*(d-p) + qGreen(frontPixel)*p)/d;
    int b = (qBlue(backPixel)*(d-p) + qBlue(frontPixel)*p)/d;
    return QColor(r, g, b);
}

void MainWindow::xInterpolateCubic(int back2Position, int backPosition, int forwardPosition, int forward2Position, int y, QImage &transformedImg)
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

void MainWindow::yInterpolateCubic(int back2Position, int backPosition, int forwardPosition, int forward2Position, int x, QImage &transformedImg)
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

QColor MainWindow::cubicInterpolate(int (&backSlope)[3], QRgb backPixel, QRgb frontPixel, int (&frontSlope)[3], int d, int p)
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

void MainWindow::contrastImg()
{
    //Read Contrast
    QString contrastText = ui->lineEdit_contrast->text();
    double contrast = contrastText.toDouble();

    if(contrast == 1.0)
        return;

    qDebug() << "In ContrastImg";

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

QColor MainWindow::contrastPixel(QRgb pixel, double contrast)
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

QColor MainWindow::convolution(QImage &transformedImg, int (&kernel)[3][3], int x, int y, int scl)
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

void MainWindow::gaussianBlur()
{
    //Get Iteration Data
    QString iterationsTxt = ui->lineEdit_blur->text();
    int iterations = iterationsTxt.toInt();

    if(iterations ==0)
        return;

    qDebug() << "in gaussianBlur";

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
        //Re-package Image
        delete img;
        img = new QImage(transformedImg);
    }
    return;
}

void MainWindow::edgeDetection()
{
    if(!(ui->radioButton_edge->isChecked()))
        return;

    qDebug() << "In edgeDetection";

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














