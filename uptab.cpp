#include "opencv2/opencv.hpp"
#include "uptab.h"
#include "ui_uptab.h"

uptab::uptab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::uptab)
{
    ui->setupUi(this);

    //有控件需要被SetSerial调用
    {
        setting = new SetSerial;
        setting->tui = ui;
    }

    //有控件需要被operation调用
    {
        operation_widget_2 = new Operation;
        operation_widget_2->tui = ui;
    }

    //网格边长横杠horizontalSlider
    ui->horizontalSlider->setRange(30,150);

    //获取摄像头，并把自定义名称显示到combobox
    {
        int cameraCount =0;
        while (1)
        {
            cv::VideoCapture cap(cameraCount);
            if (!cap.isOpened()) break;
            std::string cameraName = "Camera " + std::to_string(cameraCount);
            ui->comboBox->addItem(QString::fromStdString(cameraName));
            cameraCount++;
        }
    }

    //将按钮隐藏操作，这个操作可以减少不必要的bug，后面不再注释（忽略影响不大）
    {
        ui->btn_cameraOff->setEnabled(false);
        ui->btn_save->setEnabled(false);
    }

    //旧版的按钮触发方式（不用管）
    connect(ui->btn_cameraOff,SIGNAL(clicked()),this,SLOT(on_btn_cameraOff_clicked()));

}

uptab::~uptab()
{
    delete ui;
}

//串口按钮
void uptab::on_setSerial_clicked()
{
    setting->show();
}

//关闭摄像头按钮
void uptab::on_btn_cameraOff_clicked()
{
    ui->btn_cameraOn->setEnabled(true);
    ui->btn_cameraOff->setEnabled(false);
    ui->btn_save->setEnabled(false);
    emit cameraOff_clicked();
}

//打开摄像头按钮
void uptab::on_btn_cameraOn_clicked()
{
    int index = ui->comboBox->currentIndex();//获取combobox上的shext
    int count = ui->comboBox->count();
    if(count)
    {
        ui->btn_cameraOn->setEnabled(false);
        ui->btn_cameraOff->setEnabled(true);
    }
    emit cameraOn_clicked(index,count);
}

//网格边长拖动条
void uptab::on_horizontalSlider_valueChanged(int value)
{
    emit valueChanged(value);
}

//网格勾选框
void uptab::on_checkBox_clicked()
{
    emit checkBox_clicked();
}

//测距勾选框
void uptab::on_checkBox_2_clicked(bool checked)
{
    emit checkBox_2_clicked(checked);
}

//另存为按钮
void uptab::on_btn_save_clicked()
{
    emit btn_save_clicked();
}

//返回拍摄按钮
void uptab::on_backToShot_clicked()
{
    ui->btn_save->setEnabled(false);
}

//浏览按钮
void uptab::on_viewPicture_clicked()
{
    ui->btn_save->setEnabled(true);
}

//scrollArea适应窗口大小
void uptab::resize_scrollArea()
{
     ui->tabWidget->setFixedSize(mui->scrollArea_2->width()-30,mui->scrollArea_2->height()-30);
}
