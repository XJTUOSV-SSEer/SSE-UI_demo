#include "socket_Client.h"
#include "ui_socket_Client.h"

socket_Client::socket_Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::socket_Client)
{
    ui->setupUi(this);
    c_socket = new QTcpSocket(this);
    ui->lineEdit_addr->setText(serverAddr);
    ui->lineEdit_port->setText(QString::number(msgPort));

    QSettings writeIni(CONFIGPATH, QSettings::IniFormat);
    QSettings::Status status = writeIni.status();
    if (QSettings::NoError == status) {
        QString port = writeIni.value("serversocket/port").toString();
        QString addr = writeIni.value("serversocket/addr").toString();
        ui->lineEdit_port->setText(port);
        ui->lineEdit_addr->setText(addr);
    }

    // 判断是否连接成功
    connect(c_socket,&QTcpSocket::connected,this, [=]() {
        QMessageBox::information(nullptr,tr("连接代理"),tr("代理连接成功"));
        QSettings writeIni(CONFIGPATH, QSettings::IniFormat);
        QSettings::Status status = writeIni.status();
        if (QSettings::NoError == status) {
            writeIni.setValue("serversocket/port",ui->lineEdit_port->text());
            writeIni.setValue("serversocket/addr",ui->lineEdit_addr->text());
        }
    });
    // 判断是否断开连接
    connect(c_socket, &QTcpSocket::disconnected, this, [=](){
        QMessageBox::warning(nullptr,tr("代理"),tr("代理已断开"));
    });
}

socket_Client::~socket_Client()
{
    delete ui;
}

void socket_Client::on_pushButton_connect_clicked()
{
    // 获取编辑框中的ip和端口
    unsigned short port = ui->lineEdit_port->text().toUShort();
    QString addr = ui->lineEdit_addr->text();
    c_socket->connectToHost(addr, port);
}

