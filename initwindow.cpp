#include "initwindow.h"
#include "./ui_initwindow.h"

initWindow::initWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::initWindow)
{
    ui->setupUi(this);
    // m_pMaskLayer = new LoadingForm(this);
    // m_pMaskLayer->setFixedSize(this->size());//设置窗口大小
    // m_pMaskLayer->setVisible(false);//初始状态下隐藏，待需要显示时使用
    // this->stackUnder(m_pMaskLayer);//其中pWrapper为当前窗口的QWidget

    ischeck = false;

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
        QMessageBox::information(this,QStringLiteral("read config file"),QStringLiteral("error in config file reading"));
    }
}

initWindow::~initWindow()
{
    delete ui;
}

void initWindow::on_pushButton_socket_clicked()
{
    if(!init_Socket()){
        std::cout << "Error Create" << std::endl;
        return;
    }
    SOCKET fd;
    fd = create_clientSocket();
    if(fd == INVALID_SOCKET){
        QMessageBox::warning(nullptr,QObject::tr("Create client socket"),QObject::tr("Error in socket creation"),QMessageBox::Cancel);
    }else{
        QMessageBox::information(nullptr,QObject::tr("Create client socket"),QObject::tr("connect success!"),QMessageBox::Ok);
    }
    csocket = fd;
}


void initWindow::on_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    ischeck = ui->checkBox->isChecked();
}


void initWindow::on_pushButton_login_clicked()
{
    QString hostName = ui->lineEdit_host->text();
    QString port = ui->lineEdit_port->text();
    QString user = ui->lineEdit_user->text();
    QString password = ui->lineEdit_password->text();
    QString database = ui->lineEdit_database->text();
    QString CONNINFO = QString("dbname=%1 user=%2 password=%3 hostaddr=%4 port=%5").arg(database,user,password,hostName,port);
    std::string PGSQL_CONNINFO = CONNINFO.toStdString();
    myMessage msg(msgType::conn,PGSQL_CONNINFO);
    if(sendMsg(csocket,msg) < 0){
        QMessageBox::warning(this,tr("connect"),tr("database connection error"));
        return;
    }

    msg = recvMsg(csocket);
    if((msg.getmsgType()==msgType::connresult) && (msg.getmsgContent() == "success")){
        QMessageBox::information(this,tr("connect"),tr("database connection successful"));
    }else{
        QMessageBox::warning(this,tr("connect"),tr("database connection error"));
    }
    //TODO
    MainWindow* window = new MainWindow(csocket);
    window->show();
    this->close();
    delete this;
}


void initWindow::on_pushButton_quit_clicked()
{
    delete this;
}

