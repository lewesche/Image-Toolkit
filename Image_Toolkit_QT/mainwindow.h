#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLabel>

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

    struct InterpolationFlags{
        enum Algo{
            LINEAR    = 0,
            CUBIC     = 1
        };
    };

private slots:

    void on_comboBox_img_activated(int index);

    void on_horizontalSlider_scale_valueChanged(int value);

    void on_lineEdit_scale_returnPressed();

    void on_comboBox_scale_method_currentIndexChanged();

    void on_lineEdit_rotate_returnPressed();

    void on_horizontalSlider_rotate_valueChanged(int value);

    void on_lineEdit_blur_returnPressed();

    void on_horizontalSlider_blur_valueChanged(int value);

    void on_radioButton_edge_clicked();

    void on_horizontalSlider_contrast_valueChanged(int value);

    void on_lineEdit_contrast_returnPressed();


private:

    Ui::MainWindow *ui;

    QImage *img = nullptr;

    InterpolationFlags::Algo InterpFlag;

    void loadNewImg(int index);

    void drawImg();

    void updateImg();

    void scaleHandler();

    void rotateHandler();

    void contrastHandler();

    void blurHandler();

    void edgeDetectionHandler();

};

#endif // MAINWINDOW_H
