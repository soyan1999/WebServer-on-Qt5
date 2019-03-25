#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qasyntcpserver.h"
#include <QHostAddress>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->action_save,SIGNAL(triggered()),this,SLOT(save_slots()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::snt_log(QString log)  //显示信息
{
    QDateTime dt=QDateTime::currentDateTime();
    QString txt=dt.toString("[yyyy.MM.dd hh:mm:ss] ");
    ui->textBrowser->append(txt+log);
    logs.append(txt+log+"\r\n");
}

void MainWindow::show_message(QString msg)
{
    snt_log(msg);
}

void MainWindow::on_pushButton_clicked()
{
    /*判断路径有效*/
    QFile file_404(ui->lineEdit_3->text()+"404.html");
    if(file_404.open(QIODevice::ReadOnly))file_404.close();
    else
    {
        QMessageBox::warning(this,"警告","文件夹无效",QMessageBox::Ok);
        return;
    }
    /*判断ip有效*/
    addr=new QHostAddress();
    if(!addr->setAddress(ui->lineEdit->text()))
    {
        QMessageBox::warning(this,"警告","IP无效",QMessageBox::Ok);
        delete addr;
        return;
    }
    /*判断端口有效*/
    quint16 port=(quint16)(ui->lineEdit_2->text().toUInt());
    if(port==0)
    {
        QMessageBox::warning(this,"警告","端口无效",QMessageBox::Ok);
        return;
    }

    /*初始化*/
    tcpServer=new QAsynTcpServer();
    tcpServer->set_path(ui->lineEdit_3->text());
    tcpServer->setMaxPendingConnections(ui->spinBox->value());
    /*尝试监听*/
    if(tcpServer->listen(*addr,port))
    {
        connect(tcpServer, &QAsynTcpServer::message_ready,this, &MainWindow::show_message);
        connect(tcpServer,&QAsynTcpServer::connect_sign,this,&MainWindow::connect_slots);
        connect(tcpServer,&QAsynTcpServer::disconnect_sign,this,&MainWindow::disconnect_slots);

        snt_log(ui->lineEdit->text()+":"+ui->lineEdit_2->text()+" 启动监听成功");

        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(false);
        ui->lineEdit->setReadOnly(true);
        ui->lineEdit_2->setReadOnly(true);
        ui->lineEdit_3->setReadOnly(true);
        ui->spinBox->setReadOnly(true);
    }
    else
    {
        delete tcpServer;
        delete addr;
        snt_log(ui->lineEdit->text()+":"+ui->lineEdit_2->text()+" 启动监听失败");
        QMessageBox::warning(this,"警告","监听失败",QMessageBox::Ok);
    }

}

void MainWindow::on_pushButton_2_clicked()
{
    delete addr;
    delete tcpServer;
    snt_log(ui->lineEdit->text()+":"+ui->lineEdit_2->text()+" 关闭监听");
    ui->label_6->setText("0");
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(true);
    ui->lineEdit->setReadOnly(false);
    ui->lineEdit_2->setReadOnly(false);
    ui->lineEdit_3->setReadOnly(false);
    ui->spinBox->setReadOnly(false);
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->lineEdit_3->setText(QFileDialog::getExistingDirectory()+"/");
}

void MainWindow::connect_slots()  //开启新连接，计数器加一
{
    ui->label_6->setText(QString::number(ui->label_6->text().toInt() + 1));
}

void MainWindow::disconnect_slots()  //关闭连接，计数器减一
{
    ui->label_6->setText(QString::number(ui->label_6->text().toInt() - 1));
}

void MainWindow::save_slots()
{
    QString file_name=QFileDialog::getSaveFileName(this,tr("保存文件"),"",tr("Logs(*.log,*.txt)"));
    QFile log_file(file_name);
    if(log_file.open(QIODevice::WriteOnly))
    {
        QByteArray data=logs.toLocal8Bit();
        log_file.write(data);
    }

    else QMessageBox::warning(this,"警告","文件错误",QMessageBox::Ok);
}

