#include "initWindow.h"
#include "./ui_initwindow.h"
#include "clientMain.h"

#include <QSettings>
#include <QString>
#include <QMessageBox>
#include <QWaitCondition>
#include <QPalette>

initWindow::initWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::initWindow)
{
    ui->setupUi(this);
    ischeck = false;
    csocket = INVALID_SOCKET;

    QString hostName;
    QString port;
    QString userName;
    QString password;
    QString database;

    QSettings writeIni(CONFIGPATH, QSettings::IniFormat);
    QSettings::Status status = writeIni.status();
    if (QSettings::NoError == status) {
        ischeck = writeIni.value("database/isRemember").toBool();
        ui->checkBox->setChecked(ischeck);
        hostName = writeIni.value("database/hostName").toString();
        port = writeIni.value("database/port").toString();
        userName = writeIni.value("database/userName").toString();
        password = writeIni.value("database/password").toString();
        database = writeIni.value("database/database").toString();
        if(ischeck){
            ui->lineEdit_host->setText(hostName);
            ui->lineEdit_port->setText(port);
            ui->lineEdit_user->setText(userName);
            ui->lineEdit_password->setText(password);
            ui->lineEdit_database->setText(database);
        }
    }else{
        QMessageBox::information(this,QString("read config file"),QString("error in config file reading"));
    }
}

initWindow::~initWindow()
{
    delete ui;
}

void initWindow::on_pushButton_socket_clicked()
{
    if(!init_Socket())return;

    SOCKET fd;
    if(createSocket(fd)){
        QMessageBox::information(this,QString("createSocket"),QString("代理连接成功"));
    }else{
        // QMessageBox::warning(nullptr,QString("createSocket"),QString("Error in on_pushButton_socket_clicked()"),QMessageBox::Cancel);
        return;
    }
    csocket = fd;
}


void initWindow::on_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    ischeck = ui->checkBox->isChecked();
}


void initWindow::on_pushButton_login_clicked()
{
    if(csocket == INVALID_SOCKET){
        QMessageBox::warning(this,QString("login"),QString("无效代理，请重新连接代理"));
        return;
    }
    QString hostName = ui->lineEdit_host->text();
    QString port = ui->lineEdit_port->text();
    QString user = ui->lineEdit_user->text();
    QString password = ui->lineEdit_password->text();
    QString database = ui->lineEdit_database->text();
    QString CONNINFO = QString("dbname=%1 user=%2 password=%3 hostaddr=%4 port=%5").arg(database,user,password,hostName,port);
    std::string PGSQL_CONNINFO = CONNINFO.toStdString();
    myMsg msg(msgType::conn,PGSQL_CONNINFO);
    if(sendMsg(csocket,msg) < 0){
        QMessageBox::warning(this,QString("login"),QString("error in sendMsg()"));
        return;
    }

    myMsg msgres(msgType::none);
    recvMsg(csocket,msgres);
    if(msgres.getmsgType() == msgType::connYES){
        QMessageBox::information(this,QString("login"),QString("数据库连接成功"));
    }else if(msgres.getmsgType() == msgType::connNO){
        QMessageBox::warning(this,QString("login"),QString("数据库连接失败"));
    }else{
        QMessageBox::critical(this,QString("login"),QString("error"));
    }

    clientMain* window = new clientMain(this->csocket);
    window->show();
    this->close();
}


void initWindow::on_pushButton_quit_clicked()
{
    delete this;
}
