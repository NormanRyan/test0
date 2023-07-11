#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "setserial.h"
#include <opencv2/highgui/highgui_c.h>
#include <QPainter>

#include "my_label.h"
#include "qcustomplot.h"
#include "operation.h"
#include "uptab.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();
    Ui::MainWindow *ui;

    void myDraw();
    void myClick();
    void resizeWindow(QResizeEvent *event);

signals:

    void valueChanged(int value);
    void checkBox_clicked();
    void checkBox_2_clicked(bool checked);
    void resized();
    void scrollArea_resized();

    void cameraOff_clicked();
    void cameraOn_clicked();
    void btn_save_clicked();

private slots:
    void resizeLabel();

private:
    SetSerial *setting;
    Operation *operation_widget;
    uptab *uptab_widget;
    QPainter painter;

protected:
    void resizeEvent(QResizeEvent *event) override;

};

#endif // MAINWINDOW_H
