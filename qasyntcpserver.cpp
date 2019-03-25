#include "qasyntcpserver.h"
#include "qtcpthread.h"
#include <QTcpServer>
#include <QHash>
#include <QThread>


QAsynTcpServer::QAsynTcpServer(QObject *parent,int numConnections) :
    QTcpServer(parent)
{
     TcpThreadtList = new  QHash<int,QTcpThread *>;    //管理连接的哈希
     setMaxPendingConnections(numConnections);
}

QAsynTcpServer::~QAsynTcpServer()
{
    this->clear();
    QTcpServer::close();
    delete TcpThreadtList;
}

void QAsynTcpServer::setMaxPendingConnections(int numConnections)  //设置最大连接数
{
    this->QTcpServer::setMaxPendingConnections(numConnections);//调用Qtcpserver函数，使maxPendingConnections()依然有效
    this->maxConnections = numConnections;
}

void QAsynTcpServer::set_path(QString qpath)
{
    path = qpath;
}

void QAsynTcpServer::incomingConnection(qintptr socketDescriptor) //多线程必须在此函数里捕获新连接
{
    //继承重写此函数后，QQAsynTcpServer默认的判断最大连接数失效，自己实现
    if (TcpThreadtList->count() >= maxConnections)
    {
        return;
    }
    else
    {
        QThread *qth = new QThread();
        QTcpThread *tcp_thread = new QTcpThread(socketDescriptor);
        tcp_thread->set_path(path);
        tcp_thread->set_thread(qth);

        //NOTE:断开连接的处理，从列表移除，并释放断开的Tcpsocket，

        connect(tcp_thread,&QTcpThread::disconnect_sign,this,
                &QAsynTcpServer::sock_disconnect_slots);

        connect(tcp_thread,&QTcpThread::message_ready,this,
                &QAsynTcpServer::message_socket);

        connect(this,&QAsynTcpServer::stop_all,tcp_thread,
                &QTcpThread::stop_connect);
        //必须在QAsynTcpSocket的线程中执行

        tcp_thread->moveToThread(qth);//把tcp类移动到新的线程
        TcpThreadtList->insert(socketDescriptor,tcp_thread);//插入到连接信息中
        qth->start();
        QHostAddress addr=tcp_thread->peerAddress();
        quint16 port=tcp_thread->peerPort();
        emit message_ready(addr.toString()+":"+QString::number(port)+" 取得连接");
        emit connect_sign();
    }
}

//释放线程资源
void QAsynTcpServer::sock_disconnect_slots(int socketID,QThread * th)
{
    TcpThreadtList->remove(socketID);//连接管理中移除断开连接的socket
    th->quit();
    emit disconnect_sign();
}

void QAsynTcpServer::message_socket(QString msg)  //传递日志
{
    emit message_ready(msg);
}

void QAsynTcpServer::clear()   //服务器主动断开，清除socket
{

    //切断所有连接，并且清除线程
    emit stop_all();
    for (auto it = TcpThreadtList->begin(); it != TcpThreadtList->end(); ++it)
    {
        QThread *th=it.value()->get_thread();
        th->quit();
    }
    TcpThreadtList->clear();
}


