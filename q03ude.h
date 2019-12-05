#ifndef Q03UDE_H
#define Q03UDE_H

#include <QObject>
#include "QDebug"
#include "QTcpSocket"


class Q03UDE : public QObject
{
    Q_OBJECT
public:
    explicit Q03UDE(QObject *parent = 0);

    void set_param(QMap<QString, int> param);

    void insert_code(QString reg, int value);

private:
    enum code_flags
    {
        READ,
        WRITE
    };

    typedef struct
    {
        QString reg_name;
        int reg_num;
        int value;
    }regs;

    void gen_code(char *buf, enum code_flags flags, char *reg, int reg_num, int reg_cnt, int value);
    QString get_code(void);

    QMap<QString, int> param;
    QVector<regs> read_regs;

    bool is_write;
    enum code_flags code_type;

    regs insert_code_param;

    QTcpSocket *tcpClient;

    int j;

signals:
    void end_comu(void);
    void end_comu(QString, int);

private slots:
    void readMessage(void);

public slots:
    //void test1(void){while(1);}
    //void test2(){qDebug() << "test2";}
    void run(void);

};

#endif // Q03UDE_H
