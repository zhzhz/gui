#include "q03ude.h"
#include <windows.h>
#include "QHostAddress"
#include "QThread"

Q03UDE::Q03UDE(QObject *parent) : QObject(parent)
{
    is_write = false;
    j = 0;

    //初始化网络通信
    tcpClient = new QTcpSocket(this);
    tcpClient->connectToHost(QHostAddress("192.168.2.110"), 5010);
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(readMessage()));
}

//tcp连接，接收到信号,如果是读，则解析
void Q03UDE::readMessage(void)
{
    QByteArray recv_buf = tcpClient->readAll();
    //qDebug() << recv_buf;

    //qDebug() << buf;
    if (code_type == WRITE)
        emit end_comu();//丢弃数据
    else if (code_type == READ)
    {
        //解析，并返回数据
        //read_regs.at(i).reg_name.toLatin1().data(), read_regs.at(i).reg_num
        //qDebug() << buf.size();
        QString reg_name = read_regs.at(j).reg_name.left(1);
        int reg_num = read_regs.at(j).reg_num;


        char *buf = recv_buf.data();
        /*
        unsigned int num = 0;
        for (int i = 0; i < 4; i++)
        {
            buf[22 + i] -= '0';
            //qDebug() << (int)buf[22 + i];
            num = (num << 4) | buf[22 + i];
        }
        num = num & (1 << reg_num);
        //qDebug() << num;

        emit end_comu(reg_name + QString::number(reg_num), num?1:0);
        */
        emit end_comu(reg_name + QString::number(reg_num), (buf[recv_buf.size() - 1] - '0') & 1);
    }
}


//根据传入的参数，生成plc的指令
void Q03UDE::gen_code(char *buf, enum code_flags flags, char *reg, int reg_num, int reg_cnt, int value)
{
    if (flags == READ)
    {
        char *code_part1 = "500000FF03FF000018001004010000";
        strcpy(buf, code_part1);

        strcat(buf, reg);

        char reg_num_buf[7];
        sprintf(reg_num_buf, "%06d", reg_num);
        strcat(buf, reg_num_buf);

        char reg_cnt_buf[5];
        sprintf(reg_cnt_buf, "%04d", reg_cnt);
        strcat(buf, reg_cnt_buf);
    }
    else if (flags == WRITE)
    {
        char *code_part1 = "500000FF03FF000019001014010001";
        strcpy(buf, code_part1);

        strcat(buf, reg);

        char reg_num_buf[7];
        sprintf(reg_num_buf, "%06d", reg_num);
        strcat(buf, reg_num_buf);

        char reg_cnt_buf[5];
        sprintf(reg_cnt_buf, "%04d", reg_cnt);
        strcat(buf, reg_cnt_buf);

        char reg_value_buf[2];
        sprintf(reg_value_buf, "%01d", value);
        strcat(buf, reg_value_buf);
    }
}

//根据参数来记录要生成的指令,生成Y的string list
void Q03UDE::set_param(QMap<QString,int> param)
{
    this->param = param;
    QMap<QString, int>::const_iterator i = param.constBegin();
    while (i != param.constEnd())
    {
        if (i.key() == "Y")
        {
            regs reg;
            reg.reg_name = i.key() + "*";//*是为了适配三菱PLC指令
            reg.reg_num = i.value();
            //qDebug() << reg.reg_name << reg.reg_num;
            read_regs << reg;
        }
        i++;
    }

}

//根据json参数，和用户的点击事件生成下一条要运行的指令
//当无点击事件的时候，顺序产生读Y的指令，当有写M的指令的时候，产生写M的指令
QString Q03UDE::get_code(void)
{
    static int i = 0;
    int i_max = read_regs.size();
    char buf[100];

    if (is_write == true)
    {
        //生成写M的指令
        gen_code(buf, WRITE, insert_code_param.reg_name.toLatin1().data(),insert_code_param.reg_num, 1, insert_code_param.value);
        is_write = false;
        code_type = WRITE;
    }
    else
    {
        //顺序生成读Y的指令
        if (i >= i_max)
            i = 0;
        j = i;

        gen_code(buf, READ, read_regs.at(i).reg_name.toLatin1().data(), read_regs.at(i).reg_num, 1, 0);
        i++;
        code_type = READ;
    }

    return QString(buf);
}

//和plc通讯，调用网络通信函数，完成数据收发
void Q03UDE::run(void)
{
    //qDebug() << get_code();
    //Sleep(10);
    qDebug() << "Q03UDE::run" << QThread::currentThread();
    QString code = get_code();
    //qDebug() << code;
    tcpClient->write(code.toLatin1());



    /*
    char buf[100];
    int n_recv = tcpClient->read(buf, 100);
    if (n_recv > 0)
    {
        buf[n_recv] = '\0';
    }
    qDebug() << buf;

    Sleep(1000);

    emit end_comu();
    */

}

void Q03UDE::insert_code(QString reg, int value)
{
    qDebug() << "insert_code" << QThread::currentThread();
    insert_code_param.reg_name = reg.left(1) + "*";
    insert_code_param.reg_num = reg.mid(1).toInt();
    insert_code_param.value = value;
    is_write = true;
}


