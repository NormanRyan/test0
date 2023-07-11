#ifndef UPTAB_H
#define UPTAB_H

#include <QWidget>
#include "ui_setserial.h"
#include "ui_mainwindow.h"
#include "ui_operation.h"
#include "setserial.h"
#include "operation.h"
#include "my_label.h"

namespace Ui {
class uptab;
}

class uptab : public QWidget
{
    Q_OBJECT

public:
    explicit uptab(QWidget *parent = nullptr);
    Ui::MainWindow *mui;
    ~uptab();

    void on_backToShot_clicked();
    void on_viewPicture_clicked();
    void resize_scrollArea();

signals:

    void valueChanged(int value);
    void checkBox_clicked();
    void checkBox_2_clicked(bool checked);
    void scrollArea_resized();

    void cameraOff_clicked();
    void cameraOn_clicked(int index,int count);
    void btn_save_clicked();

private slots:

    void on_setSerial_clicked();

    void on_btn_cameraOn_clicked();

    void on_btn_cameraOff_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_checkBox_clicked();

    void on_checkBox_2_clicked(bool checked);

    void on_btn_save_clicked();  

private:
    Ui::uptab *ui;
    SetSerial *setting;
    Operation *operation_widget_2;
};

#endif // UPTAB_H
