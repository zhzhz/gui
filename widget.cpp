#include "widget.h"
#include "ui_widget.h"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QPushButton"
#include "QCheckBox"
#include "QDebug"
#include "cJSON.h"


QMap<QString,int> map;

struct User : QObjectUserData {
    //int nID;  // ID
    //QString strName;  // 用户名
    int state;
};

Q_DECLARE_METATYPE(User)

void printJson(cJSON * root)
{
    /*
    for(int i=0; i<cJSON_GetArraySize(root); i++)
    {
        cJSON * item = cJSON_GetArrayItem(root, i);//得到root的每一项
        int cJSON_GetArraySize(item)
    }
    */

    /*
    cJSON *cjsonArr = cJSON_GetObjectItem(root, "M");//得到M
    int i = cJSON_GetArraySize(cjsonArr);
    for (int j = 0; j < i; j++)
    {
        cJSON *cjsonTmp = cJSON_GetArrayItem(cjsonArr, 0);
    }
    */

    for(int i=0; i<cJSON_GetArraySize(root); i++)
    {
        cJSON * item = cJSON_GetArrayItem(root, i);//得到root的每一项,为了简单，当前每一项皆为M和Y的数组
        for (int j = 0; j < cJSON_GetArraySize(item); j++)//得到数组的大小
        {
            cJSON *cjsonTmp = cJSON_GetArrayItem(item, j);//得到数组的每一项，item->string为数组的名字
            int num = cjsonTmp->valueint;
            //qDebug() << item->string  << num;
            map.insertMulti(QString(item->string), num);
        }
    }

    /*
    QMap<QString, int>::const_iterator i = map.constBegin();
    while (i != map.constEnd()) {
          qDebug() << i.key() << ": " << i.value() << endl;
          i++;
    }
    */
}

void doit2(char *text)
{
    char *out;cJSON *json;

    json=cJSON_Parse(text);
    if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
    else
    {
        /*
        out=cJSON_Print(json);
        cJSON_Delete(json);
        printf("%s\n",out);
        free(out);
        */
        printJson(json);
        cJSON_Delete(json);
    }
}

void dofile2(char *filename)
{
    FILE *f;long len;char *data;

    f=fopen(filename,"rb");fseek(f,0,SEEK_END);len=ftell(f);fseek(f,0,SEEK_SET);
    data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
    doit2(data);
    free(data);
}



WidgtPart::WidgtPart(QWidget *parent) :
    QWidget(parent)
{
    vlayout = new QVBoxLayout;
    this->setLayout(vlayout);
}

void WidgtPart::addWidget(QWidget *widget)
{
    vlayout->addWidget(widget);
}

WidgtPart::~WidgtPart()
{
    //delete ui;
}




Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //基本布局
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    this->setLayout(horizontalLayout);

    WidgtPart *widgetleft = new WidgtPart(this);
    WidgtPart *widgetright = new WidgtPart(this);

    horizontalLayout->addWidget(widgetleft);
    horizontalLayout->addWidget(widgetright);


    //通讯相关
    comu_thread = new COMU_thread;
    comu_thread->start();//新建线程用于网络通讯
    q03ude = new Q03UDE;
    q03ude->moveToThread(comu_thread);//将plc移到新线程



    //读取配置文件，得到M和Y的配置值
    dofile2("../tests/a.txt");

    //根据得到的M和Y的配置值，生成对应的界面布局
    QMap<QString, int>::const_iterator i = map.constBegin();
    while (i != map.constEnd())
    {
        //qDebug() << i.key() << ": " << i.value() << endl;
        if (i.key() == "M")
        {
            QPushButton *pushButton = new QPushButton(widgetleft);
            widgetleft->addWidget(pushButton);
            pushButton->setObjectName(i.key() + QString::number(i.value()));
            pushButton->setText(i.key() + QString::number(i.value()));
            connect(pushButton, SIGNAL(clicked()), this, SLOT(button_clicked()));
            pushButton->setCheckable(true);

            User *pUser = new User();
            pUser->state = 1;
            pushButton->setUserData(Qt::UserRole, pUser);
        }
        else if (i.key() == "Y")
        {
            QCheckBox *checkBox = new QCheckBox(widgetright);
            widgetright->addWidget(checkBox);
            checkBox->setText(i.key() + QString::number(i.value()));
            qDebug() << QString::number(i.value(),16);
            //checkBox2->setCheckState(Qt::Checked);
            widgetlist << checkBox;
        }
        i++;
    }

    q03ude->set_param(map);

    connect(this, SIGNAL(start_comu()), q03ude, SLOT(run()));
    connect(q03ude, SIGNAL(end_comu()), this, SLOT(end_comu()));
    connect(q03ude, SIGNAL(end_comu(QString, int)), this, SLOT(end_comu(QString, int)));
    emit start_comu();//开始调用通讯线程，开始数据采集的工作
    /*
    //水平布局1
    QWidget *widget = new QWidget(this);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(60, 110, 211, 51));

    QHBoxLayout *horizontalLayout = new QHBoxLayout(widget);
    horizontalLayout->setSpacing(6);
    horizontalLayout->setContentsMargins(11, 11, 11, 11);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *pushButton = new QPushButton(widget);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));

    horizontalLayout->addWidget(pushButton);

    QCheckBox *checkBox = new QCheckBox(widget);
    checkBox->setObjectName(QString::fromUtf8("checkBox"));

    horizontalLayout->addWidget(checkBox);
    */

    /*
    //水平布局2
    QPushButton *pushButton = new QPushButton(this);
    QCheckBox *checkBox = new QCheckBox(this);

    QHBoxLayout *horizontalLayout = new QHBoxLayout;

    horizontalLayout->addWidget(pushButton);
    horizontalLayout->addWidget(checkBox);

    this->setLayout(horizontalLayout);
    */

/*综合布局
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    WidgtPart *widgetleft = new WidgtPart(this);

    QPushButton *pushButton = new QPushButton(widgetleft);
    QPushButton *pushButton1 = new QPushButton(widgetleft);
    widgetleft->addWidget(pushButton);
    widgetleft->addWidget(pushButton1);

    pushButton->setObjectName(QString::fromUtf8("pushButton"));
    pushButton1->setObjectName(QString::fromUtf8("pushButton1"));

    widgetlist.append(pushButton);
    widgetlist.append(pushButton1);


    connect(pushButton, SIGNAL(clicked()), this, SLOT(button_clicked()));
    connect(pushButton1, SIGNAL(clicked()), this, SLOT(button_clicked()));

    WidgtPart *widgetright = new WidgtPart(this);

    QCheckBox *checkBox = new QCheckBox(widgetright);
    QCheckBox *checkBox1 = new QCheckBox(widgetright);
    QCheckBox *checkBox2 = new QCheckBox(widgetright);
    widgetright->addWidget(checkBox);
    widgetright->addWidget(checkBox1);
    widgetright->addWidget(checkBox2);
    checkBox2->setCheckState(Qt::Checked);

    horizontalLayout->addWidget(widgetleft);
    horizontalLayout->addWidget(widgetright);
    this->setLayout(horizontalLayout);
    */


}

void Widget::button_clicked(void)
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    /*
    for (int i = 0; i < widgetlist.size(); i++)
    {
        if (widgetlist.at(i)->objectName() == btn->objectName())
        {
            qDebug() << btn->objectName() + " clicked";
            break;
        }
    }
    */
    qDebug() << "button_clicked" << QThread::currentThread();
    User *pUser = (User *)(btn->userData(Qt::UserRole));
    //qDebug() << btn->objectName() + " clicked" + QString::number(pUser->state);
    q03ude->insert_code(btn->objectName(), pUser->state);
    if (pUser->state == 1)
        pUser->state = 0;
    else
        pUser->state = 1;

}

void Widget::end_comu(void)
{
    emit start_comu();
}

void Widget::end_comu(QString reg, int value)
{
    //设置lcd状态
    //qDebug() << reg << value;
    for (int i = 0; i < widgetlist.size(); i++)
    {
        QCheckBox* cbox = qobject_cast<QCheckBox*>(widgetlist.at(i));
        qDebug() << cbox->text();
        if (cbox->text() == reg)
        {
            cbox->setCheckState(value ? Qt::Checked : Qt::Unchecked);
            break;
        }
    }
    emit start_comu();
}

Widget::~Widget()
{
    delete ui;
    comu_thread->quit();
    comu_thread->wait();

}


