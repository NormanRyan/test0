#ifndef OPERATION_H
#define OPERATION_H

#include <QWidget>
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "setserial.h"
#include <opencv2/highgui/highgui_c.h>
#include <QPainter>
#include "qcustomplot.h"
#include "ui_uptab.h"


namespace Ui {
class Operation;
}

class Operation : public QWidget
{
    Q_OBJECT

public:
    explicit Operation(QWidget *parent = nullptr);
    ~Operation();
    Ui::MainWindow *mui;
    Ui::uptab *tui;
    QTimer *timer;

    void plotGrayDistribution(cv::Mat img, QCustomPlot *plot);

    quint8 calculateCrc(QByteArray data);

    void on_cameraOff_clicked();
    void on_cameraOn_clicked(int index,int count);    
    void on_btn_save_clicked_slot();
    void resize_scrollArea();

public slots:
    void ReadFrame();

    void on_btn_shotSave_clicked();

    void on_btn_backToShot_clicked();

    void on_btn_shot_clicked();

    void on_btn_savePicture_clicked();

    void on_btn_viewPicture_clicked();

    void on_btn_viewLast_clicked();

    void on_btn_viewNext_clicked();

    void on_radioButton_9_clicked();

    void on_rbtn_510_clicked();

    void on_rbtn_540_clicked();

    void on_rbtn_560_clicked();

    void on_rbtn_580_clicked();

    void on_rbtn_All_clicked();

    void on_radioButton_6_clicked();

    void on_radioButton_7_clicked();

    void on_radioButton_8_clicked();

signals:
    void backToShot_clicked();
    void viewPicture_clicked();

private:
    Ui::Operation *ui;
    cv::VideoCapture capture;


};

#endif // OPERATION_H
