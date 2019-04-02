#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::vector<QString> ImgDirectories;

private slots:

    void on_comboBox_img_activated(int index);

    void on_horizontalSlider_scale_valueChanged(int value);

    void on_lineEdit_scale_returnPressed();

    void on_comboBox_scale_method_currentIndexChanged();

    void on_lineEdit_blur_returnPressed();

    void on_horizontalSlider_blur_valueChanged(int value);

    void on_radioButton_edge_clicked();

    void on_horizontalSlider_contrast_valueChanged(int value);

    void on_lineEdit_contrast_returnPressed();

private:
    Ui::MainWindow *ui;

    QImage *img;

    struct InterpolationFlags{
        enum Algo{
            LINEAR    = 0,
            CUBIC     = 1
        };
    };

    InterpolationFlags::Algo InterpFlag;

    void loadNewImg(int index);

    void drawImg();

    void updateImg();

    void scaleImg();

    void xInterpolateLinear(int backPosition, int forwardPosition, int y, QImage &transformedImg);

    void yInterpolateLinear(int backPosition, int forwardPosition, int x, QImage &transformedImg);

    QColor linearInterpolate(QRgb backPixel, QRgb frontPixel, int d, int p);

    void xInterpolateCubic(int back2Position, int backPosition, int forwardPosition, int forward2Position, int y, QImage &transformedImg);

    void yInterpolateCubic(int back2Position, int backPosition, int forwardPosition, int forward2Position, int y, QImage &transformedImg);

    QColor cubicInterpolate(int (&backSlope)[3], QRgb backPixel, QRgb frontPixel, int (&frontSlope)[3], int d, int p);

    void contrastImg();

    QColor contrastPixel(QRgb pixel, double contrast);

    QColor convolution(QImage &transformedImg, int (&kernel)[3][3], int x, int y, int scl);

    void gaussianBlur();

    void edgeDetection();

};

#endif // MAINWINDOW_H
