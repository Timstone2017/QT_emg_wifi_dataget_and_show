#include "workthread.h"
#include <QtDebug>
#include "mainwindow.h"
#include "cglobal.h"

int olddata_num=0;//原始数据数量
int newdata_num=0;//处理后数据数量

int threadnum=0;//进入tcp接收中断的次数

//QString old_str;
//QString new_str;

quint8 jiaoyanhe(quint8 data[],int n);//校验一帧数据是否正确的函数
QString laststr;//上次接收到数据，但末尾剩下些数据不足以构成一帧，转到下一次接收处理

WorkThread::WorkThread()
{

}

void WorkThread::run()
{
    socket_emg = new QTcpSocket();

    //连接信号槽
    QObject::connect(socket_emg, &QTcpSocket::readyRead, this, &WorkThread::socket_Read_Data,Qt::BlockingQueuedConnection);
    QObject::connect(socket_emg, &QTcpSocket::disconnected, this, &WorkThread::socket_Disconnected,Qt::BlockingQueuedConnection);

    qDebug()<<"I am in thread run!";

    //取消已有的连接
    socket_emg->abort();
    //连接服务器
    socket_emg->connectToHost(IP, port);
    //等待连接成功
    if(!socket_emg->waitForConnected(30000))
    {
        qDebug() << "Connection failed!";
        return;
    }
    qDebug() << "Connect successfully!";
    exec();
}

void WorkThread::socket_Read_Data()
{
    threadnum++;
    qDebug()<<"I am in thread!"<<threadnum;
    QByteArray buffer;
    //读取缓冲区数据
    buffer = socket_emg->readAll();

    if((!buffer.isEmpty()))
    {
        //接收并解析原始数据
        QString str = laststr+tr(buffer.toHex().data());
        laststr.clear();//清楚之前剩余的字符串
        QString error_data="37 08 00 08 00 08 00 08 00 08 00 08 00 08 00 08 00 45";

        //简单方法
        int dataloc=0;
        int datanow=0;
        QString y="37";
        dataloc=str.indexOf(y);//查找字符串y，返回位置
        datanow=dataloc;

        while((datanow=str.indexOf(y,datanow+1))>=0)
        {
            if(datanow-dataloc==18*2)
            {
                QString hex_data=str.mid(dataloc,18*2);
                //old_str=old_str+hex_data+"\r\n";

                //取出数据，校验和
                quint8 data[18];
                for(int i=0;i<18;++i)
                {
                    data[i]=hex_data.mid(i*2,2).toLatin1().toUInt(Q_NULLPTR,16);
                }

                if(data[17]==jiaoyanhe(data,17))
                {
                    old_str=old_str+hex_data+"\r\n";
                    olddata_num++;
                    //ui->data16h->append(hex_data);//获得数据

                    quint16 showdata[8]={0x0000};
                    float showdata2[8]={0.0};
                    QString data_float_string;
                    for(int i=0;i<8;++i)
                    {
                        showdata[i]=data[i*2+1];
                        showdata[i]=(showdata[i]<<8)+data[i*2+2];
                        showdata2[i]=showdata[i];
                        showdata2[i]=showdata2[i]/4096*3.3;
                        data_float_string = data_float_string+QString("%1,").arg(showdata2[i]);
                    }
                    new_str=new_str+data_float_string+"\r\n";
                    newdata_num++;
                    //ui->datafloat->append(data_float_string);

                }else
                {
                    old_str=old_str+error_data+"\r\n";
                    //ui->data16h->append(error_data);//抛弃错误数据
                    quint8 data[18];
                    for(int i=0;i<18;++i)
                    {
                        data[i]=error_data.mid(i*2,2).toLatin1().toUInt(Q_NULLPTR,16);
                    }

                    quint16 showdata[8]={0x0000};
                    float showdata2[8]={0.0};
                    QString data_float_string;
                    for(int i=0;i<8;++i)
                    {
                        showdata[i]=data[i*2+1];
                        showdata[i]=(showdata[i]<<8)+data[i*2+2];
                        showdata2[i]=showdata[i];
                        showdata2[i]=showdata2[i]/4096*3.3;
                        data_float_string = data_float_string+QString("%1,").arg(showdata2[i]);
                    }
                    new_str=new_str+data_float_string+"\r\n";
                    //ui->datafloat->append(data_float_string);
                }
                dataloc=datanow;
            }else
            {
                dataloc=datanow;
            }
        }

        qDebug()<<"thread_new_data_size-->"<<new_str.size();
        qDebug()<<"thread_num-->"<<threadnum;
        laststr=str.mid(dataloc);//获取剩余的字符串。从dataloc到末尾
        emit wifidataSignal_1("1");//显示真实的一帧数据
        //old_str.clear();
        emit wifidataSignal_2("2");//显示真实的一帧数据
       // new_str.clear();
    }


    //buffer1=buffer1+buffer;
}

void WorkThread::socket_Disconnected()
{
    qDebug()<<"I am in Disconnected!";
    //发送按键失能
    //emit wifidataSignal_2(hex_data);
    //ui->pushButton_Send->setEnabled(false);
    //修改按键文字
    //ui->pushButton_Connect->setText("连接");
    qDebug() << "Disconnected!";
}

void WorkThread::socket_IP(QString ip_now)
{
    IP=ip_now;
}

void WorkThread::socket_Port(int port_now)
{
    port=port_now;
}

quint8 jiaoyanhe(quint8 data[],int n)
{
    quint8 a1=0;
    for(int i=0;i<n;++i)
    {
        a1=a1+data[i];
    }
    return a1;
}

//复杂的处理接收到的程序的方法，但还没完成
//        int datalocnow=0;
//        int dataloc[8]={0};
//        QString y="37";
//        dataloc[0]=str.indexOf(y);//查找字符串y，返回位置
//        datalocnow=dataloc[0];
//        while((datalocnow=str.indexOf(y,datalocnow+1))>=0)
//        {
//            if(dataloc[1]==0)
//            {
//                if(datalocnow-dataloc[0]==18*2)
//                {
//                    //取出数据，校验和
//                    QString hex_data=str.mid(dataloc[0],18*2);
//                    old_str=old_str+hex_data;
//                    //emit wifidataSignal_2(hex_data);//显示真实的一帧数据

//                    //dataloc[0]=datalocnow;
//                    continue;
//                }
//                else if(datalocnow-dataloc[0]>18*2)
//                {
//                    //抛弃之前的数据
//                    //dataloc[0]=datalocnow;
//                    continue;
//                }
//                else if(datalocnow-dataloc[0]<18*2)
//                {
//                    //添加标记位置
//                    dataloc[1]=datalocnow;
//                    continue;
//                }

//            }

//            if(dataloc[1]>0)
//            {
//                if(datalocnow-dataloc[0]==18*2)
//                {
//                    //取出数据，校验和
//                    QString hex_data=str.mid(dataloc[0],18*2);
//                    old_str=old_str+hex_data;
//                    //emit wifidataSignal_2(hex_data);
//                    //ui->textEdit_Recv_data->append(hex_data);//显示真实的一帧数据

//                    dataloc[0]=datalocnow;
//                    for(int i=1;i<8;++i)
//                    {
//                        dataloc[i]=0;
//                    }
//                    continue;
//                }
//                else if(datalocnow-dataloc[0]>18*2)
//                {
//                    //抛弃之前的数据
//                    dataloc[0]=datalocnow;
//                    for(int i=1;i<8;++i)
//                    {
//                        dataloc[i]=0;
//                    }
//                    continue;
//                }
//                else if(datalocnow-dataloc[0]<18*2)
//                {
//                    //添加标记位置
//                    for(int i=1;i<8;++i)
//                    {
//                        if(dataloc[i]==0)
//                        {
//                            dataloc[i]=datalocnow;
//                            break;
//                        }
//                    }
//                    continue;
//                }
//            }
//        }
