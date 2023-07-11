#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencv2/opencv.hpp"
#include <QDebug>
#include "QTimer"
#include "QImage"
#include <opencv2/highgui/highgui_c.h>
#include <QLabel>
#include <string>
#include <QDateTime>
#include <QFileDialog>
#include <QPainter>
#include <QMouseEvent>

using namespace cv;

//这个变量的作用是什么?
static int adapt =0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //有控件需要被调用 右侧操控台ui类
    {
        operation_widget = new Operation(this);
        operation_widget->mui = ui;
        ui->scrollArea->setWidget(operation_widget);
    }

    //有控件需要被调用 上侧页面窗口类
    {
        uptab_widget = new uptab(this);
        uptab_widget->mui = ui;
        ui->scrollArea_2->setWidget(uptab_widget);
    }

    //有控件需要被调用 串口设置ui类
    {
        setting = new SetSerial;
        setting->mui = ui;
    }

    //连接uptab类和operation类的信号和槽
    {
        connect(uptab_widget,&uptab::cameraOff_clicked,operation_widget,&Operation::on_cameraOff_clicked);
        connect(uptab_widget,&uptab::cameraOn_clicked,operation_widget,&Operation::on_cameraOn_clicked);
        connect(uptab_widget,&uptab::btn_save_clicked,operation_widget,&Operation::on_btn_save_clicked_slot);
    }

    //连接uptab类和my_label类的信号和槽
    {
        connect(uptab_widget,&uptab::valueChanged,ui->label_Show,&my_Label::on_horizontalSlider_Changed);
        connect(uptab_widget,&uptab::checkBox_clicked,ui->label_Show,&my_Label::on_checkBox_Changed);
        connect(uptab_widget,&uptab::checkBox_2_clicked,ui->label_Show,&my_Label::on_checkBox_2_Changed);

        connect(uptab_widget,&uptab::valueChanged,ui->label_Show_2,&my_Label::on_horizontalSlider_Changed);
        connect(uptab_widget,&uptab::checkBox_clicked,ui->label_Show_2,&my_Label::on_checkBox_Changed);
        connect(uptab_widget,&uptab::checkBox_2_clicked,ui->label_Show_2,&my_Label::on_checkBox_2_Changed);

        connect(uptab_widget,&uptab::valueChanged,ui->label_Show_3,&my_Label::on_horizontalSlider_Changed);
        connect(uptab_widget,&uptab::checkBox_clicked,ui->label_Show_3,&my_Label::on_checkBox_Changed);
        connect(uptab_widget,&uptab::checkBox_2_clicked,ui->label_Show_3,&my_Label::on_checkBox_2_Changed);
    }

    //连接operation类和uptab类的信号和槽
    {
        connect(operation_widget,&Operation::backToShot_clicked,uptab_widget,&uptab::on_backToShot_clicked);
        connect(operation_widget,&Operation::viewPicture_clicked,uptab_widget,&uptab::on_viewPicture_clicked);
    }

    //适应窗口大小的信号和槽
    {
        //使摄像头画面适应窗口的信号和槽
        connect(this, &MainWindow::resized, this, &MainWindow::resizeLabel);

        //使右侧ui适应窗口大小
        connect(this, &MainWindow::scrollArea_resized, operation_widget, &Operation::resize_scrollArea);

        //使上方tab适应窗口大小
        connect(this, &MainWindow::scrollArea_resized, uptab_widget, &uptab::resize_scrollArea);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 编写 resizeLabel 槽函数, 使画面适应窗口
void MainWindow::resizeLabel()
{
    //QSize labelSize(ui->label_Show->width(),ui->label_Show->height());//获取label的尺寸
    int width = ui->tabWidget_2->width() ;//获取窗口的宽度
    int height = ui->tabWidget_2->height() ;//获取窗口的高度

    ui->label_Show->setFixedSize(width, height);
    ui->label_Show_2->setFixedSize(width, height);
    ui->label_Show_3->setFixedSize(width, height);
}

//为触发上述函数重写resizeEvent使其产生resized信号
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if(adapt++)
    {
        emit resized();
        emit scrollArea_resized();
    }
}

