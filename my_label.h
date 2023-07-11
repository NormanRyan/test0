#ifndef MY_LABEL_H
#define MY_LABEL_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

class my_Label : public QLabel
{
    Q_OBJECT
public:
    explicit my_Label(QWidget *parent = nullptr);


public slots:
    void on_horizontalSlider_Changed(int value);
    void on_checkBox_Changed();
    void on_checkBox_2_Changed(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:

};

#endif // MY_LABEL_H
