#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qasyntcpserver.h"
#include <QHostAddress>

class QAsynTcpServer;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void snt_log(QString log);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void show_message(QString msg);

    void connect_slots();

    void disconnect_slots();

    void save_slots();

private:
    Ui::MainWindow *ui;
    QAsynTcpServer *tcpServer;
    QHostAddress *addr;
    QString logs;    //记录

};

#endif // MAINWINDOW_H
