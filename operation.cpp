#include "operation.h"
#include "ui_operation.h"
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QTimer>
#include <QImage>
#include <opencv2/highgui/highgui_c.h>
#include <QLabel>
#include <string>
#include <QDateTime>
#include <QFileDialog>
#include <QPainter>

using namespace cv;

static int i = 0;
static int store =0;
static Mat resized_frame;
static Mat resized_frame_2;
static Mat resized_frame_3;
static QStringList list;//初始化列表，存图片地址时用到
static QImage my_image;

Operation::Operation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Operation)
{
    ui->setupUi(this);

    Mat grayImage(512, 512, CV_8UC1);
    Mat colorImage,colorImage_resize;

    timer=new QTimer(this);

    //定时器和槽函数连接
    connect(timer,SIGNAL(timeout()),this,SLOT(ReadFrame()));

    ui->horizontalSlider_5->setRange(0,255);

    //这些是什么?
     {
         void(QSpinBox::*spSignal)(int)=&QSpinBox::valueChanged;
         ui->horizontalSlider->setRange(0,510);
         ui->spinBox_3->setRange(0,510);
         connect(ui->spinBox_3,spSignal,ui->horizontalSlider,&QSlider::setValue);
         connect(ui->horizontalSlider,&QSlider::valueChanged,ui->spinBox_3,&QSpinBox::setValue);
         ui->horizontalSlider->setValue(255);

         ui->horizontalSlider_2->setRange(1,100);
         ui->spinBox_4->setRange(1,100);
         connect(ui->spinBox_4,spSignal,ui->horizontalSlider_2,&QSlider::setValue);
         connect(ui->horizontalSlider_2,&QSlider::valueChanged,ui->spinBox_4,&QSpinBox::setValue);
         ui->horizontalSlider_2->setValue(51);

         ui->horizontalSlider_3->setRange(0,100);
         ui->spinBox_5->setRange(0,100);
         connect(ui->spinBox_5,spSignal,ui->horizontalSlider_3,&QSlider::setValue);
         connect(ui->horizontalSlider_3,&QSlider::valueChanged,ui->spinBox_5,&QSpinBox::setValue);
    }

    //封装明场和荧光的勾选按钮
     {
         connect(ui->check_Bright, &QCheckBox::stateChanged, [=](int state){
             if(state == Qt::Checked){
                 mui->tabWidget_2->insertTab(0, mui->tab2_Bright, "明场");
                 mui->tabWidget_2->setCurrentWidget(mui->tab2_Bright);
             } else {
                 mui->tabWidget_2->removeTab(mui->tabWidget_2->indexOf(mui->tab2_Bright));
             }
         });
         connect(ui->check_Flu, &QCheckBox::stateChanged, [=](int state){
             if(state == Qt::Checked){
                 mui->tabWidget_2->insertTab(1, mui->tab2_Flu, "荧光");
                 mui->tabWidget_2->setCurrentWidget(mui->tab2_Flu);
             } else {
                 mui->tabWidget_2->removeTab(mui->tabWidget_2->indexOf(mui->tab2_Flu));
             }
         });
     }

    //按钮禁用
    {
        ui->btn_shotSave->setEnabled(false);
        ui->btn_shot->setEnabled(false);
        ui->btn_savePicture->setEnabled(false);
        ui->btn_viewLast->setEnabled(false);
        ui->btn_viewPicture->setEnabled(false);
        ui->btn_backToShot->setEnabled(false);
        ui->btn_viewNext->setEnabled(false);
    }

    //设置每个像素的灰度值（使其灰度值自下而上逐渐递增，且均匀分布）
     for (int i = 0; i < grayImage.rows; i++)
     {
         for (int j = 0; j < grayImage.cols; j++)
         {
             grayImage.at<uchar>(i, j) = static_cast<uchar>(i * 255 / grayImage.rows);
         }
     }

    //设置色度条（将灰度图转换为荧光图）
     {
         applyColorMap(grayImage,colorImage,COLORMAP_JET);
         cv::resize(colorImage, colorImage_resize, Size(ui->colorbarLabel->width(), ui->colorbarLabel->height()));
         QImage image=QImage(static_cast<const uchar*>(colorImage_resize.data),
                             colorImage_resize.cols,
                             colorImage_resize.rows,
                             static_cast<int>(colorImage_resize.step),
                             QImage::Format_RGB888)
                 .rgbSwapped();
         ui->colorbarLabel->setPixmap(QPixmap::fromImage(image.mirrored(false,true)));
     }

}

Operation::~Operation()
{
    delete ui;
}

//定时器槽函数,刷新摄像头画面
void Operation::ReadFrame()
{
    double minValue, maxValue;
    double alpha=1;
    if(ui->horizontalSlider_2->value()>=51)
    {
        alpha =1.0+((ui->horizontalSlider_2->value()-51)/5.0);
    }
    else
    {
        alpha =1.0/(51 - ui->horizontalSlider_2->value());
    }

    int color =ui->horizontalSlider_5->value();
    Mat frame;
    capture>>frame;
    Mat result,gray,equalized_img,copy;
    frame.convertTo(frame,-1,alpha,(ui->horizontalSlider->value())-255);//调对比度和亮度（第三和第四个参数）
    cvtColor(frame, gray, cv::COLOR_BGR2GRAY);//转换为灰度图
    gray.setTo(cv::Scalar(0, 0, 0), gray <= color);//将灰度值小于某个值的像素点变为黑色
    equalizeHist(gray, equalized_img);//均衡化，使得图像的像素值在整个像素值范围内均匀分布（可做详细了解）
    applyColorMap(equalized_img,result,COLORMAP_JET);//染色
    result.setTo(cv::Scalar(0, 0, 0), gray <= color);

    //将黑色像素点用原图的像素点替换，达到合成效果
    copy =result.clone();
    for (int i = 0; i < copy.rows; i++) {
        for (int j = 0; j < copy.cols; j++) {
            cv::Vec3b pixel = copy.at<cv::Vec3b>(i, j); // 获取像素值
            if (pixel == cv::Vec3b(0, 0, 0)) { // 判断是否为黑色
                cv::Vec3b replacePixel = frame.at<cv::Vec3b>(i, j); // 获取替换像素
                copy.at<cv::Vec3b>(i, j) = replacePixel; // 替换像素
            }
        }
    }

    QCustomPlot *plot = ui->widget_3;
    plotGrayDistribution(gray, plot);

    minMaxLoc(gray, &minValue, &maxValue);
    ui->label->setNum(maxValue);
    if(minValue < color)
    {
        ui->label_2->setNum(color);
    }
    else
    {
        ui->label_2->setNum(minValue);
    }
    cv::resize(frame, resized_frame, Size(mui->label_Show->width(), mui->label_Show->height()));
    QImage image=QImage(static_cast<const uchar*>(resized_frame.data),
                        resized_frame.cols,resized_frame.rows,
                        static_cast<int>(resized_frame.step),
                        QImage::Format_RGB888)
            .rgbSwapped();
    mui->label_Show->setPixmap(QPixmap::fromImage(image.mirrored(true,false)));

    cv::resize(result, resized_frame_2, Size(mui->label_Show_2->width(), mui->label_Show_2->height()));
    QImage image_2=QImage(static_cast<const uchar*>(resized_frame_2.data),
                          resized_frame_2.cols,
                          resized_frame_2.rows,
                          static_cast<int>(resized_frame_2.step),
                          QImage::Format_RGB888).
            rgbSwapped();
    mui->label_Show_2->setPixmap(QPixmap::fromImage(image_2.mirrored(true,false)));

    cv::resize(copy, resized_frame_3, Size(mui->label_Show_3->width(), mui->label_Show_3->height()));
    QImage image_3=QImage(static_cast<const uchar*>(resized_frame_3.data),
                          resized_frame_3.cols,
                          resized_frame_3.rows,
                          static_cast<int>(resized_frame_3.step),
                          QImage::Format_RGB888)
            .rgbSwapped();
    mui->label_Show_3->setPixmap(QPixmap::fromImage(image_3.mirrored(true,false)));

}

//关闭摄像头按钮
void Operation::on_cameraOff_clicked()
{
    ui->btn_shotSave->setEnabled(false);
    ui->btn_shot->setEnabled(false);
    ui->btn_savePicture->setEnabled(false);
    ui->btn_viewLast->setEnabled(false);
    ui->btn_viewPicture->setEnabled(false);
    ui->btn_backToShot->setEnabled(false);
    ui->btn_viewNext->setEnabled(false);

    ui->label->setNum(255);
    ui->label_2->setNum(0);

    timer->stop();//停止定时器
    mui->label_Show->clear();//清除显示屏幕上的图像
    mui->label_Show_2->clear();
    mui->label_Show_3->clear();
    capture.release();
}

//打开摄像头按钮
void Operation::on_cameraOn_clicked(int index,int count)
{
    if(i)
    {
         ui->btn_viewPicture->setEnabled(true);
    }
    else
    {
        ui->btn_viewPicture->setEnabled(false);
    }
    ui->btn_shotSave->setEnabled(true);
    ui->btn_shot->setEnabled(true);
    ui->btn_savePicture->setEnabled(false);
    ui->btn_viewLast->setEnabled(false);
    //ui->btn_backToShot->setEnabled(true);
    ui->btn_viewNext->setEnabled(false);

    if(count!= 0)
    {
        capture.open(index);//打开对应摄像头
        timer->start(30);//启动定时器
    }
    else
    {
        ui->btn_shotSave->setEnabled(false);
        ui->btn_shot->setEnabled(false);
    }

}

//拍照并保存按钮
void Operation::on_btn_shotSave_clicked()
{
    ui->btn_shotSave->setEnabled(false);
    ui->btn_shot->setEnabled(false);
    ui->btn_savePicture->setEnabled(false);
    ui->btn_viewLast->setEnabled(false);
    ui->btn_viewPicture->setEnabled(true);
    ui->btn_backToShot->setEnabled(true);
    ui->btn_viewNext->setEnabled(false);

    timer->stop();

    //保存图片时要保证给的地址不能重复，重复的话只能保存第一张（这里用实时时间加变量i来定义地址名称，保证名称不会重复）
    QDateTime currentDateTime = QDateTime::currentDateTime();//获取当前时间
    QString Img_Name = currentDateTime.toString("yyyy_MM_dd_hh_mm_ss")+ QString::number(i++) + ".jpg";//图片地址名称
    list.append(Img_Name);//将图片地址名称加到列表
    String Image_Name =Img_Name.toStdString();//将qstring转string，这样imwrite函数才能识别
    //存图片
    if(mui->tabWidget_2->currentIndex()==0)
    {
        imwrite(Image_Name,resized_frame);
    }
    if(mui->tabWidget_2->currentIndex()==1)
    {
        imwrite(Image_Name,resized_frame_2);
    }
    if(mui->tabWidget_2->currentIndex()==2)
    {
        imwrite(Image_Name,resized_frame_3);
    }
}

//返回拍摄按钮
void Operation::on_btn_backToShot_clicked()
{
    ui->btn_shotSave->setEnabled(true);
    ui->btn_shot->setEnabled(true);
    ui->btn_savePicture->setEnabled(false);
    ui->btn_viewLast->setEnabled(false);
    ui->btn_backToShot->setEnabled(false);
    if(i)
    {
        ui->btn_viewPicture->setEnabled(true);
    }
    else
    {
        ui->btn_viewPicture->setEnabled(false);
    }
    ui->btn_backToShot->setEnabled(true);
    ui->btn_viewNext->setEnabled(false);
//    mui_t2->btn_save->setEnabled(false);

    store=0;
    timer->start(30);
    emit backToShot_clicked();
}

//拍照按钮
void Operation::on_btn_shot_clicked()
{
    ui->btn_shotSave->setEnabled(false);
    ui->btn_shot->setEnabled(false);
    ui->btn_savePicture->setEnabled(true);
    ui->btn_viewLast->setEnabled(false);
    ui->btn_viewPicture->setEnabled(false);
    ui->btn_backToShot->setEnabled(true);
    ui->btn_viewNext->setEnabled(false);

    timer->stop();
}

//保存按钮
void Operation::on_btn_savePicture_clicked()
{
    ui->btn_shotSave->setEnabled(false);
    ui->btn_shot->setEnabled(false);
    ui->btn_savePicture->setEnabled(false);
    ui->btn_viewLast->setEnabled(false);
    ui->btn_viewPicture->setEnabled(true);
    ui->btn_backToShot->setEnabled(true);
    ui->btn_viewNext->setEnabled(false);

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString Img_Name =currentDateTime.toString("yyyy_MM_dd_hh_mm_ss")+ QString::number(i++) + ".jpg";
    list.append(Img_Name);
    String Image_Name =Img_Name.toStdString();
    if(mui->tabWidget_2->currentIndex()==0)
    {
        imwrite(Image_Name,resized_frame);
    }
    if(mui->tabWidget_2->currentIndex()==1)
    {
        imwrite(Image_Name,resized_frame_2);
    }
    if(mui->tabWidget_2->currentIndex()==2)
    {
        imwrite(Image_Name,resized_frame_3);
    }
}

//浏览按钮
void Operation::on_btn_viewPicture_clicked()
{
    ui->btn_shotSave->setEnabled(false);
    ui->btn_shot->setEnabled(false);
    ui->btn_savePicture->setEnabled(false);
    ui->btn_viewLast->setEnabled(true);
    ui->btn_viewPicture->setEnabled(false);
    ui->btn_backToShot->setEnabled(true);
    ui->btn_viewNext->setEnabled(true);

    if(store<0)
    {
        store = i;
        store--;
    }

    if(i)
    {
        timer->stop();
        QString Img_Name =list[store % i];//从列表中获取地址名
        String Image_Name=Img_Name.toStdString();
        Mat img = imread(Image_Name);
        my_image = QImage(static_cast<const uchar*>(img.data),img.cols,img.rows,static_cast<int>(img.step),QImage::Format_RGB888).rgbSwapped();
        mui->label_Show->setPixmap(QPixmap::fromImage(my_image.mirrored(true,false)));
        mui->label_Show_2->setPixmap(QPixmap::fromImage(my_image.mirrored(true,false)));
        mui->label_Show_3->setPixmap(QPixmap::fromImage(my_image.mirrored(true,false)));
    }
    emit viewPicture_clicked();
}

//上一张按钮
void Operation::on_btn_viewLast_clicked()
{
    store--;
    on_btn_viewPicture_clicked();
}

//下一张按钮
void Operation::on_btn_viewNext_clicked()
{
    store++;
    on_btn_viewPicture_clicked();
}

//另存为
void Operation::on_btn_save_clicked_slot()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);//打开文件夹
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString Img_Name =currentDateTime.toString("yyyy_MM_dd_hh_mm_ss")+ QString::number(i++) + ".jpg";
    folderPath.append("/");
    folderPath.append(Img_Name);
    my_image.save(folderPath);

}

//画出图像灰度分布直方图，在右下方
void Operation::plotGrayDistribution(Mat img, QCustomPlot *plot)
{
    int histSize = 256;  // 直方图bin数
    float range[] = { 0, 256 };  // 像素值范围
    const float* histRange = { range };
    cv::Mat hist;
    const int o = 0;
    cv::calcHist(&img, 1, &o, cv::Mat(), hist, 1, &histSize, &histRange);
    QVector<double> x(histSize), y(histSize);
    for (int i = 0; i < histSize; ++i)
    {
        x[i] = i;
        y[i] = hist.at<double>(i);
    }
    QCPBars *bars = new QCPBars(plot->xAxis, plot->yAxis);
    bars->setData(x, y);
    plot->rescaleAxes();
    plot->replot();

}

//crc校验函数，可以先了解原理，再用chatgpt解释这段代码
quint8 Operation::calculateCrc(QByteArray data)
{
    quint8 crc = 0xFF; // 初始值为0xFF
    for (int i = 0; i < data.length(); i++) {
        crc ^= static_cast<quint8>(data.at(i));
        for (int j = 0; j < 8; j++) {
            if (crc & 0x01) {
                crc >>= 1;
                crc ^= 0x8c;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

//scrollArea适应窗口大小
void Operation::resize_scrollArea()
{
     ui->widget_4->setFixedSize(mui->scrollArea->width()-30,mui->scrollArea->height()-30);
}

//镜头波长510
void Operation::on_rbtn_510_clicked()
{
    QByteArray data;
    data.append(QByteArray::fromHex("F0")); // 帧头1
    data.append(QByteArray::fromHex("0F")); //帧头2
    data.append(QByteArray::fromHex("01")); // 帧类型
    data.append(QByteArray::fromHex("01"));
    quint8 crc = calculateCrc(data);
    data.append(QByteArray::fromHex(QString("%1").arg(crc, 0, 16).toUtf8()));
    data.append(QByteArray::fromHex("AA")); // 帧尾

    m_serial.write(data);
}

//镜头波长540
void Operation::on_rbtn_540_clicked()
{
    QByteArray data;
    data.append(QByteArray::fromHex("F0")); // 帧头1
    data.append(QByteArray::fromHex("0F")); //帧头2
    data.append(QByteArray::fromHex("01")); // 帧类型
    data.append(QByteArray::fromHex("02"));
    quint8 crc = calculateCrc(data);
    data.append(QByteArray::fromHex(QString("%1").arg(crc, 0, 16).toUtf8()));
    data.append(QByteArray::fromHex("AA")); // 帧尾

    m_serial.write(data);
}

//镜头波长560
void Operation::on_rbtn_560_clicked()
{
    QByteArray data;
    data.append(QByteArray::fromHex("F0")); // 帧头1
    data.append(QByteArray::fromHex("0F")); //帧头2
    data.append(QByteArray::fromHex("01")); // 帧类型
    data.append(QByteArray::fromHex("03"));
    quint8 crc = calculateCrc(data);
    data.append(QByteArray::fromHex(QString("%1").arg(crc, 0, 16).toUtf8()));
    data.append(QByteArray::fromHex("AA")); // 帧尾

    m_serial.write(data);
}

//镜头波长580
void Operation::on_rbtn_580_clicked()
{
    QByteArray data;
    data.append(QByteArray::fromHex("F0")); // 帧头1
    data.append(QByteArray::fromHex("0F")); //帧头2
    data.append(QByteArray::fromHex("01")); // 帧类型
    data.append(QByteArray::fromHex("04"));
    quint8 crc = calculateCrc(data);
    data.append(QByteArray::fromHex(QString("%1").arg(crc, 0, 16).toUtf8()));
    data.append(QByteArray::fromHex("AA")); // 帧尾

    m_serial.write(data);
}

//镜头波长全光谱
void Operation::on_rbtn_All_clicked()
{
    QByteArray data;
    data.append(QByteArray::fromHex("F0")); // 帧头1
    data.append(QByteArray::fromHex("0F")); //帧头2
    data.append(QByteArray::fromHex("01")); // 帧类型
    data.append(QByteArray::fromHex("05"));
    quint8 crc = calculateCrc(data);
    data.append(QByteArray::fromHex(QString("%1").arg(crc, 0, 16).toUtf8()));
    data.append(QByteArray::fromHex("AA")); // 帧尾

    m_serial.write(data);
}

//激发光波长425
void Operation::on_radioButton_6_clicked()
{
    QByteArray data;
    data.append(QByteArray::fromHex("F0")); // 帧头1
    data.append(QByteArray::fromHex("0F")); //帧头2
    data.append(QByteArray::fromHex("02")); // 帧类型
    data.append(QByteArray::fromHex("01"));
    quint8 crc = calculateCrc(data);
    data.append(QByteArray::fromHex(QString("%1").arg(crc, 0, 16).toUtf8()));
    data.append(QByteArray::fromHex("AA")); // 帧尾

    m_serial.write(data);
}

//激发光波长455
void Operation::on_radioButton_7_clicked()
{
    QByteArray data;
    data.append(QByteArray::fromHex("F0")); // 帧头1
    data.append(QByteArray::fromHex("0F")); //帧头2
    data.append(QByteArray::fromHex("02")); // 帧类型
    data.append(QByteArray::fromHex("02"));
    quint8 crc = calculateCrc(data);
    data.append(QByteArray::fromHex(QString("%1").arg(crc, 0, 16).toUtf8()));
    data.append(QByteArray::fromHex("AA")); // 帧尾

    m_serial.write(data);
}

//激发光波长480
void Operation::on_radioButton_8_clicked()
{
    QByteArray data;
    data.append(QByteArray::fromHex("F0")); // 帧头1
    data.append(QByteArray::fromHex("0F")); //帧头2
    data.append(QByteArray::fromHex("02")); // 帧类型
    data.append(QByteArray::fromHex("03"));
    quint8 crc = calculateCrc(data);
    data.append(QByteArray::fromHex(QString("%1").arg(crc, 0, 16).toUtf8()));
    data.append(QByteArray::fromHex("AA")); // 帧尾

    m_serial.write(data);
}

//激发光波长全光谱
void Operation::on_radioButton_9_clicked()
{
    QByteArray data;
    data.append(QByteArray::fromHex("F0")); // 帧头1
    data.append(QByteArray::fromHex("0F")); //帧头2
    data.append(QByteArray::fromHex("02")); // 帧类型
    data.append(QByteArray::fromHex("04"));//数据
    quint8 crc = calculateCrc(data);
    data.append(QByteArray::fromHex(QString("%1").arg(crc, 0, 16).toUtf8()));//将quint8类型转换并存入数组（Ai写的，稍微了解一下就行）
    data.append(QByteArray::fromHex("AA")); // 帧尾

    m_serial.write(data);
}
