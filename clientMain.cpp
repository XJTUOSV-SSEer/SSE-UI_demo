#include "clientMain.h"
#include "ui_clientMain.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlQuery>
#include <QStringList>
#include <QTextStream>
#include <QTimer>
#include <QProgressDialog>

clientMain::clientMain(SOCKET fd, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::clientMain)
{
    ui->setupUi(this);
    this->msgsocket = fd;
    this->filesocket = INVALID_SOCKET;
    this->totalBytes = 0;
    this->fileName = 0;
}

clientMain::~clientMain()
{
    delete ui;
}

void clientMain::closeEvent(QCloseEvent *event){
    QMessageBox::StandardButton result = QMessageBox::question(this,tr("退出程序"),tr("是否退出程序？"),QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if(result == QMessageBox::Yes){
        closesocket(msgsocket);
        close_Socket();
        event->accept();
    }else{
        event->ignore();
    }
}

void clientMain::on_pushButton_scanCsvFile_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(QStringLiteral("打开文件"));
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setNameFilter(tr("CSV(*.csv)"));
    QString openPath=QCoreApplication::applicationDirPath();
    fileDialog->setDirectory(openPath);
    if(fileDialog->exec() == QDialog::Accepted){
        QString srcFile = fileDialog->selectedFiles()[0];
        ui->lineEdit_csvFile->setText(srcFile);
    }
}

void clientMain::on_pushButton_uploadFile_clicked()
{
    // 文件大小
    QString filepath = ui->lineEdit_csvFile->text();
    QFile fp(filepath);
    fp.open(QIODevice::Text);
    this->totalBytes = fp.size();

    // 文件名
    QFileInfo fileinfo(filepath);
    this->fileName = fileinfo.fileName();
    std::string filename = this->fileName.toStdString();
    std::string info = filename +" "+ std::to_string(this->totalBytes);

    // 发送 msgType::file
    myMsg msg(msgType::file,info);
    sendMsg(msgsocket,msg);

    // 创建 文件专用socket
    if(createSocket(filesocket)){
        QMessageBox::information(this,QString("createSocket"),QString("文件代理连接成功，开始传输文件"));
    }else{
        QMessageBox::warning(nullptr,QString("createSocket"),QString("文件代理连接失败"),QMessageBox::Cancel);
        return;
    }

    sendFile(filesocket,filepath.toStdString());
    shutdown(filesocket,SD_SEND);
    filesocket = INVALID_SOCKET;

    myMsg msgres(msgType::none);
    recvMsg(msgsocket,msgres);

    if(msgres.getmsgType() == msgType::fileYES){
        QMessageBox::information(this,QString("sendfile"),QString("upload file successful"));
    }else{
        QMessageBox::warning(this,QString("sendfile"),QString("error in on_pushButton_uploadFile_clicked()"));
    }
}


void clientMain::on_pushButton_sqlQuery_clicked()
{
    QString sql = ui->plainTextEdit_sqlQuery->toPlainText();
    myMsg msg(msgType::sql,sql.toStdString());
    sendMsg(msgsocket,msg);
}


bool clientMain::sendFile(SOCKET s, std::string filepath)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);//设置鼠标为等待状态
    QProgressDialog progress;
    progress.setWindowTitle("sendFile");
    progress.setLabelText("正在发送中...");
    progress.setCancelButtonText("取消");
    progress.setRange(0, totalBytes/bufSize);//设置范围
    progress.setModal(true);//设置为模态对话框
    progress.show();

    const char* path = filepath.c_str();
    FILE* fp = fopen(path, "rb");
    if (!fp){
        QMessageBox::warning(nullptr,QString("fopen"),QString("Error in sendFile()"),QMessageBox::Cancel);
        return false;
    }

    //buffer
    char buffer[bufSize + 1];
    memset(buffer,'\0',bufSize + 1 );

    int readlen = 0, sendlen = 0, cnt = 0;
    qint64 sendcnt = 0;
    //循环读取文件进行传送
    while ((readlen = fread(buffer, 1, 16, fp)) > 0){
        sendlen= send(s, buffer, readlen, 0);
        memset(buffer,'\0',bufSize + 1 );
        if (sendlen == SOCKET_ERROR){
            QMessageBox::warning(nullptr,QString("send"),QString("Error in sendFile()"),QMessageBox::Cancel);
            return false;
        }
        cnt ++;
        sendcnt += sendlen;
        progress.setValue(cnt);
        //用户取消的话则中止
        if (progress.wasCanceled()){
            return false;
        }
        Sleep(500);
        QCoreApplication::processEvents();
    }

    fclose(fp);
    QApplication::restoreOverrideCursor();
    progress.close();
    QMessageBox::information(nullptr,QString("send"),QString("send file success!(sendsize: %1B/%2B)").arg(sendcnt).arg(this->totalBytes),QMessageBox::Ok);

    return true;
}
