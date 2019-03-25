#ifndef QTCPTHREAD_H
#define QTCPTHREAD_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include "mainwindow.h"

class QTcpThread : public QTcpSocket
{
    Q_OBJECT
public:
    explicit QTcpThread(qintptr socketDescriptor,QObject *parent = 0);
    ~QTcpThread();
    QThread *get_thread();
    void set_thread(QThread *qth);
    void set_path(QString qpath);

public slots:
    void rply_rqst();

    void disconnect_solt();

    void stop_connect();
signals:
    void disconnect_sign(int,QThread *);

    void message_ready(QString);

private:
    qintptr socketId;
    QThread *th;
    QString path;
};

#endif // QTCPTHREAD_H
