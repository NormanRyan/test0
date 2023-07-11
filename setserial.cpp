#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setserial.h"
#include "ui_setserial.h"
#include "QDebug"

QSerialPort m_serial;
static uint8_t a = 0;

SetSerial::SetSerial(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetSerial)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);

    on_btnrefresh_clicked();
    //on_btnopen_clicked();

    //为啥这步没用, 标签没有刷新文字
//    if(m_serial.isOpen()){
//        ui->le->setText("串口打开成功");++a;
//        connect(&m_serial,&QSerialPort::readyRead,this,&SetSerial::messlot);
//    }else{
//        ui->le->setText("串口打开失败!!!");
//    }

    ui->le->clear();
}

SetSerial::~SetSerial()
{
    delete ui;
}

void SetSerial::on_btnrefresh_clicked()
{
    //清空串口名
    ui->cmbportname->clear();
    //遍历串口信息
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){
        ui->cmbportname->addItem(info.portName());
    }
    ui->le->setText("串口已刷新");
}

void SetSerial::on_btnopen_clicked()
{
    m_serial.setPortName(ui->cmbportname->currentText());
    m_serial.setBaudRate(ui->cmbportbaud->currentText().toInt());
    m_serial.open(QIODevice::ReadWrite);
    if(a){
        ui->le->setText("已经开了");
    }else if(m_serial.isOpen()){
        ui->le->setText("串口打开成功");++a;
        connect(&m_serial,&QSerialPort::readyRead,this,&SetSerial::messlot);
    }else{
        ui->le->setText("串口打开失败!!!");
    }
}

void SetSerial::on_btnclose_clicked()
{
    m_serial.close();
    if(m_serial.isOpen()){
        ui->le->setText("串口关闭失败");
        connect(&m_serial,&QSerialPort::readyRead,this,&SetSerial::messlot);
    }else{
        ui->le->setText("串口已关闭");a=0;
    }
}

//串口接收信息的处理函数
void SetSerial::messlot(){
    QByteArray data = m_serial.readAll();
    tui->label_3->setText(data);
}
