#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QDebug>
#include <QPointer>

#include "scale.h"
#include "rotation.h"
#include "contrast.h"
#include "blur.h"
#include "edgeDetection.h"


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

    loadNewImg(0);


    ui->comboBox_scale_method->addItem("Linear");
    ui->comboBox_scale_method->addItem("Cubic");

    InterpFlag = static_cast<MainWindow::InterpolationFlags::Algo>(0);

    drawImg();
}

MainWindow::~MainWindow()
{
    delete img;
    delete ui;
}

//EVENT HANDLING

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
}

void MainWindow::on_comboBox_scale_method_currentIndexChanged()
{
    updateImg();
}

void MainWindow::on_lineEdit_rotate_returnPressed()
{
    QString scaleTxt = ui->lineEdit_rotate->text();
    int degrees = scaleTxt.toInt();

    if(degrees > 180)
        degrees = 180;
    else if(degrees < -180)
        degrees = -180;

    ui->lineEdit_rotate->setText(QString::number(degrees));
    ui->horizontalSlider_rotate->setValue(degrees);
}

void MainWindow::on_horizontalSlider_rotate_valueChanged(int degrees)
{
    ui->lineEdit_rotate->setText(QString::number(degrees));

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
}

void MainWindow::on_lineEdit_blur_returnPressed()
{
    QString scaleTxt = ui->lineEdit_blur->text();
    int iterations = scaleTxt.toInt();

    if(iterations > 10)
        iterations = 10;

    ui->lineEdit_blur->setText(QString::number(iterations));
    ui->horizontalSlider_blur->setValue(iterations);
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




void MainWindow::loadNewImg(int index)
{
    QImage imgTemp = QImage(ImgDirectories[static_cast<unsigned long>(index)]);
    imgTemp = imgTemp.convertToFormat(QImage::Format_ARGB32);

    if(img != nullptr)
        delete img;
    img = new QImage(imgTemp);
    return;
}

void MainWindow::drawImg()
{
    QPixmap pix = QPixmap::fromImage(*img);
    ui->label->setGeometry(350, 0, img->width(), img->height());
    ui->label->setPixmap(pix);
}

void MainWindow::updateImg()
{
    //Refresh Original Image
    loadNewImg(ui->comboBox_img->currentIndex());

    scaleHandler();
    rotateHandler();
    contrastHandler();
    blurHandler();
    edgeDetectionHandler();
    drawImg();
}

void MainWindow::scaleHandler()
{
    //Read scale + flags
    QString scaleTxt = ui->lineEdit_scale->text();
    double scale = scaleTxt.toDouble();

    if(scale == 1.0)
        return;

    InterpolationFlags::Algo flags = static_cast<InterpolationFlags::Algo>(ui->comboBox_scale_method->currentIndex());

    qDebug() << "in scaleImg - " << flags;

    scaleImg(img, scale, flags);

}

void MainWindow::rotateHandler()
{
    //Read scale + flags
    QString scaleTxt = ui->lineEdit_rotate->text();
    int degrees = scaleTxt.toInt();

    if(degrees == 0)
        return;

    qDebug() << "in rotateImg";

    rotateImg(img, degrees);
}

void MainWindow::contrastHandler()
{
    //Read Contrast
    QString contrastText = ui->lineEdit_contrast->text();
    double contrast = contrastText.toDouble();

    if(contrast == 1.0)
        return;

    qDebug() << "In ContrastImg";

    contrastImg(img, contrast);
}

void MainWindow::blurHandler()
{
    //Get Iteration Data
    QString iterationsTxt = ui->lineEdit_blur->text();
    int iterations = iterationsTxt.toInt();

    if(iterations ==0)
        return;

    qDebug() << "in blurImg";

    blurImg(img, iterations);

}

void MainWindow::edgeDetectionHandler()
{
    if(!(ui->radioButton_edge->isChecked()))
        return;

    qDebug() << "In edgeDetection";

    edgeDetection(img);
}




















