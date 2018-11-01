#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <QThread.h>
#include <QtNetwork/QTcpSocket>

class WorkThread : public QThread
{
    Q_OBJECT
private:
    QTcpSocket *socket_emg;
    QString IP="192.168.16.254";
    int port=8086;
public:
    WorkThread();
protected:
    void run();
signals:
    void wifidataSignal_1(QString);
    void wifidataSignal_2(QString);
    void olddatanum(int);
    void newdatanum(int);

private slots:
    void socket_Read_Data();
    void socket_Disconnected();
public slots:
    void socket_IP(QString);
    void socket_Port(int);

};

#endif // WORKTHREAD_H
