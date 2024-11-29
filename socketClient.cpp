#include "socketClient.h"
#include "ui_socketClient.h"
#include "dbConnection.h"
#include "myQMsgBox.h"

socketClient::socketClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::socketClient)
{
    ui->setupUi(this);
    c_socket = new QTcpSocket(this);
    ui->lineEdit_addr->setText("127.0.0.1");
    ui->lineEdit_port->setText(QString::number(20000));

    QSettings writeIni(CONFIGPATH, QSettings::IniFormat);
    QSettings::Status status = writeIni.status();
    if (QSettings::NoError == status) {
        QString port = writeIni.value("serversocket/msgPort").toString();
        QString addr = writeIni.value("serversocket/addr").toString();
        ui->lineEdit_port->setText(port);
        ui->lineEdit_addr->setText(addr);
    }
}

socketClient::~socketClient()
{
    delete ui;
}

void socketClient::on_pushButton_connect_clicked()
{
    // 获取编辑框中的ip和端口
    unsigned short port = ui->lineEdit_port->text().toUShort();
    QString addr = ui->lineEdit_addr->text();
    c_socket->connectToHost(addr, port);

    if (c_socket->waitForConnected(3000)) { // 等待3000毫秒
        QMessageBox::information(nullptr,tr("连接代理"),tr("代理连接成功"));
        QSettings writeIni(CONFIGPATH, QSettings::IniFormat);
        QSettings::Status status = writeIni.status();
        if (QSettings::NoError == status) {
            writeIni.setValue("serversocket/msgPort",ui->lineEdit_port->text());
            writeIni.setValue("serversocket/addr",ui->lineEdit_addr->text());
        }
        // c_socket->disconnect();
        dbConnection *w = new dbConnection(this->c_socket);
        w->show();
        close();
    } else {
        myQMsgBox msgBox(this,3000,QMessageBox::Warning);
        msgBox.setWindowTitle(tr("连接代理"));
        msgBox.setText(tr("代理连接失败"));
        msgBox.exec();
        return;
    }
}

