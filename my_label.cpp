#include "my_label.h"
#include <QPainter>
#include <QDebug>

static int clicktime =1;
static QPointF clickPoint1;
static QPointF clickPoint2;
static int grid_spacing =30;
static int checkBox1 =0;
static int checkBox2 =0;

my_Label::my_Label(QWidget *parent) : QLabel(parent)
{

}

void my_Label::on_horizontalSlider_Changed(int value)
{
    grid_spacing = value;
    update();
}

void my_Label::on_checkBox_Changed()
{
    checkBox1 =!checkBox1;
    update();
}

void my_Label::on_checkBox_2_Changed(bool checked)
{
    if(checked)
    {
        checkBox2 =1;
    }
    else
    {
        checkBox2 =0;
        clickPoint1.setX(qQNaN());
        clickPoint1.setY(qQNaN());
        clickPoint2.setX(qQNaN());
        clickPoint2.setY(qQNaN());
        clicktime =1;
    }
    update();
}

void my_Label::mousePressEvent(QMouseEvent *event)
{
    if(checkBox2)
    {
        if(clicktime)
        {
            clickPoint1 = event->pos();
            clicktime =0;
        }
        else
        {
            clickPoint2 = event->pos();
            clicktime =1;
        }
    }
}


void my_Label::paintEvent(QPaintEvent *event )
{
    QLabel::paintEvent(event);
    if(checkBox1)
    {
        QPainter painter(this);
        QPen pen(QColor(255,0,0));
        pen.setWidth(2);
        painter.setPen(pen);

        for (int y = 0; y < 2000; y += grid_spacing) {
            QPoint start_point(0, y); // 网格左边界的起点
            QPoint end_point(2000, y); // 网格右边界的终点
            painter.drawLine(start_point, end_point); // 绘制一条水平线
        }

        // 绘制列网格
        for (int x = 0; x < 2000; x += grid_spacing) {
            QPoint start_point(x, 0); // 网格上边界起点
            QPoint end_point(x, 2000); // 网格下边界的终点
            painter.drawLine(start_point, end_point); // 绘制一条垂直线
        }
    }
    if(clicktime && checkBox2)
    {
        QPainter painter(this);
        QPen pen(QColor(255,0,0));
        pen.setWidth(2);
        painter.setPen(pen);

        QLineF line(clickPoint1,clickPoint2);
        QPointF center((clickPoint1.x() + clickPoint2.x())/2, (clickPoint1.y() + clickPoint2.y())/2);
        painter.drawLine(line);
        painter.drawText(center,QString::number(line.length()));

    }
}

