#include "qtcpthread.h"
#include <QTcpSocket>
#include <QThread>
#include <QFile>

QTcpThread::QTcpThread(qintptr socketDescriptor, QObject *parent):QTcpSocket(parent)
{
    this->setSocketDescriptor(socketDescriptor);
    socketId=socketDescriptor;
    connect(this, &QTcpThread::readyRead,this,&QTcpThread::rply_rqst);
    connect(this, &QTcpThread::disconnected, this, &QTcpThread::disconnect_solt);
}

QTcpThread::~QTcpThread()
{

}

QThread* QTcpThread::get_thread()
{
    return th;
}

void QTcpThread::set_thread(QThread *qth)
{
    th = qth;
}
void QTcpThread::set_path(QString qpath)
{
    path = qpath;
}

void QTcpThread::stop_connect()     //服务器主动断开
{
    QTcpSocket::close();
    deleteLater();
}

void QTcpThread::disconnect_solt()  //浏览器主动断开
{
    QTcpSocket::close();
    emit disconnect_sign(socketId,th);
    emit message_ready(peerAddress().toString() + ":" + QString::number(peerPort()) + " 断开连接");
    deleteLater();
}

void QTcpThread::rply_rqst()   //响应请求
{
    /*获取socket信息*/
    QHostAddress addr=QTcpSocket::peerAddress();
    quint16 port=QTcpSocket::peerPort();
    /*获取报文信息*/
    QByteArray req_byt=QTcpSocket::readAll();
    QString req=QString(req_byt);
    QString url=req.split(" ")[1];

    emit message_ready(addr.toString()+":"+QString::number(port)+" 请求"+url);  //发送日志

    /*查找文件*/
    QFile file(path+url.mid(1));
    if(!file.open(QIODevice::ReadOnly))
    {
        /*请求不存在，发送404报文*/
        QFile file_404(path+"404.html");
        file_404.open(QIODevice::ReadOnly);

        /*编写报文头*/
        QString http = "HTTP/1.1 404  Not Found\r\n";
        http += "Content-Type: ";
        http += "text/html;charset=utf-8\r\n";
        http += "Connection: keep-alive\r\n";
        http += QString("Content-Length: %1\r\n\r\n").arg(QString::number(file_404.size()));

        QByteArray head, data;
        head.append(http);
        QTcpSocket::write(head);  //发送报文头
        /*分段发送404.html*/
        while(!file_404.atEnd())
        {
            data = file_404.read(10240);
            write(data);
        }
        file_404.close();
        emit message_ready(addr.toString() + ":" + QString::number(port) + " 请求" + url + "失败");  //发送请求失败日志
    }
    else
    {
        /*请求存在，发送200报文*/

        /*编写报文头*/
        QString http = "HTTP/1.1 200 OK\r\n";
        http += "Content-Type: ";
        if(url.split(".")[1]=="html")http += "text/html";
        else http += "application/octet-stream";
        http += ";charset=utf-8\r\n";
        http += "Connection: keep-alive\r\n";
        http += QString("Content-Length: %1\r\n\r\n").arg(QString::number(file.size()));

        QByteArray head, data;
        head.append(http);
        QTcpSocket::write(head);  //发送报文头
        /*分段发送文件*/
        while(!file.atEnd())
        {
            data = file.read(10240);
            write(data);
        }
        file.close();
        emit message_ready(addr.toString()+":"+QString::number(port)+" 请求"+url+"成功");  //发送请求成功日志
    }
}
