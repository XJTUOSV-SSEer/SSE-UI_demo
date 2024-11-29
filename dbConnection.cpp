#include "dbConnection.h"
#include "./ui_dbConnection.h"
#include "clientMain.h"
#include "myQMsgBox.h"
#include "socketClient.h"
#include "utils.h"

using namespace std;

dbConnection::dbConnection(QPointer<QTcpSocket> socket,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::dbConnection)
{
    ui->setupUi(this);
    ischeck = false;
    c_socket = socket;

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
        QMessageBox::information(this,tr("read config file"),tr("error in config file reading"));
    }

    // 判断是否断开连接
    connect(c_socket, &QTcpSocket::disconnected, this, [=](){
        QMessageBox::warning(nullptr,tr("代理"),tr("代理已断开，点击确认返回连接代理"),QMessageBox::Ok);
        c_socket->deleteLater();
        c_socket->disconnect();
        socketClient *w = new socketClient();
        w->show();
        close();
        delete this;
    });
}

dbConnection::~dbConnection()
{
    delete ui;
}

void dbConnection::on_pushButton_change_clicked()
{
    c_socket->disconnectFromHost();
}


void dbConnection::on_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    ischeck = ui->checkBox->isChecked();
}


void dbConnection::on_pushButton_login_clicked()
{
    QString hostName = ui->lineEdit_host->text();
    QString port = ui->lineEdit_port->text();
    QString user = ui->lineEdit_user->text();
    QString password = ui->lineEdit_password->text();
    QString database = ui->lineEdit_database->text();
    QString CONNINFO = QString("dbname=%1 user=%2 password=%3 hostaddr=%4 port=%5").arg(database,user,password,hostName,port);
    QJsonObject msgContent;
    msgContent["CONNINFO"] = CONNINFO;
    myMsg msg(msgType::CONNECTION_REQUEST,msgContent);
    // conn
    if(!sendMsg(c_socket,msg)) return;

    myMsg msgres(msgType::NONE);
    recvMsg(c_socket,msgres);
    if(msgres.getmType() == msgType::CONNECTION_SUCCESS){
        // QMessageBox::information(this,tr("login"),tr("数据库连接成功"));
        myQMsgBox msgBox(this);
        msgBox.setWindowTitle(tr("login"));
        msgBox.setText(tr("数据库连接成功"));
        msgBox.exec();
    }else if(msgres.getmType() == msgType::CONNECTION_FAILED){
        // QMessageBox::information(this,tr("login"),tr("数据库连接失败"));
        myQMsgBox msgBox(this,3000000,QMessageBox::Warning);
        msgBox.setWindowTitle(tr("login"));
        msgBox.setText(tr("数据库连接失败"));
        msgBox.exec();
    }else{
        myQMsgBox msgBox(this,10000000,QMessageBox::Critical);
        msgBox.setWindowTitle(tr("login"));
        msgBox.setText(tr("error"));
        msgBox.exec();
    }

    c_socket->disconnect();
    clientMain* window = new clientMain(c_socket);
    window->show();
    close();
    delete this;
}


void dbConnection::on_pushButton_quit_clicked()
{
    delete this;
}
