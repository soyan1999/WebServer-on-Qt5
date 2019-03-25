#ifndef QASYNTCPSERVER_H
#define QASYNTCPSERVER_H


#include<QTcpServer>
#include<QHash>
#include "qtcpthread.h"
class QTcpThread;

class QAsynTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit QAsynTcpServer(QObject *parent = 0,int numConnections = 100);
    ~QAsynTcpServer();
    void setMaxPendingConnections(int numConnections);//重写设置最大连接数函数
    void set_path(QString qpath);
protected slots:
    void sock_disconnect_slots(int handle, QThread *th);//socket主动断开后的处理
    void message_socket(QString msg);                //传递socket记录
public:
    void clear(); //断开所有连接，线程计数器请0
protected:
    void incomingConnection(qintptr socketDescriptor);//覆盖已获取多线程
    QHash<int, QTcpThread*> * TcpThreadtList;//管理连接的哈希
private:
    int maxConnections;         //最大连接数
    QString path;               //服务路径
signals:
    void message_ready(QString);  //传递记录
    void stop_all();            //关闭服务器传给所有socket
    void connect_sign();        //连接信号传给mainwindow
    void disconnect_sign();     //断开信号传给mainwindow
};

#endif // QASYNTCPSERVER_H
