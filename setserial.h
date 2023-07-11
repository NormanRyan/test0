#ifndef SETSERIAL_H
#define SETSERIAL_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "ui_mainwindow.h"
#include "ui_uptab.h"

extern QSerialPort m_serial;

namespace Ui {
class SetSerial;
}

class SetSerial : public QWidget
{
    Q_OBJECT

public:
    explicit SetSerial(QWidget *parent = nullptr);
    Ui::MainWindow *mui;
    Ui::uptab *tui;
    ~SetSerial();

private slots:
    void on_btnrefresh_clicked();

    void on_btnopen_clicked();

    void on_btnclose_clicked();

    void messlot();

private:
    Ui::SetSerial *ui;
};

#endif // SETSERIAL_H
