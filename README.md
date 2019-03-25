# WebServer-on-Qt5
QT5编写的简易web服务器
## WebServer.pro
QT5工程文件
## main.cpp
主函数（调用窗口类）
## maindows.ui
主窗口ui文件
## mainwindows.h mainwindows.cpp
主窗口类
## qasyntcpserver.h qasyntcpserver.cpp
支持多线程的tcpserver类，继承自qtcpserver类
## qtcpthread.h qtcpthread.cpp
继承自qtcpsocket处理单个http连接的请求
