#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "QVBoxLayout"
#include "QThread"
#include "q03ude.h"

namespace Ui {
class Widget;
}

class COMU_thread :public QThread
{
    Q_OBJECT
public:
    COMU_thread()
    {
        ;
    }
};

class WidgtPart : public QWidget
{
    Q_OBJECT
public:
    explicit WidgtPart(QWidget *parent = 0);
    ~WidgtPart();

    void addWidget(QWidget *widget);

private:
    QVBoxLayout *vlayout;

};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

signals:
    void start_comu(void);

private:
    Ui::Widget *ui;
    QList<QWidget *> widgetlist;

    COMU_thread *comu_thread;
    Q03UDE *q03ude;

private slots:
    void button_clicked(void);
    void end_comu(void);
    void end_comu(QString reg, int value);
};

#endif // WIDGET_H
