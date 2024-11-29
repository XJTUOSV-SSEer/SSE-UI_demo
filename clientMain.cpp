#include "clientMain.h"
#include "ui_clientMain.h"
#include "socket_Client.h"
#include "utils.h"
#include "myQMsgBox.h"

using namespace std;

clientMain::clientMain(QPointer<QTcpSocket> socket, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::clientMain)
{
    ui->setupUi(this);
    this->msgsocket = socket;
    this->fileSize = 0;
    this->fileName = 0;

    // 判断是否断开连接
    connect(msgsocket, &QTcpSocket::disconnected, this, [=](){
        QMessageBox::warning(nullptr,tr("代理"),tr("代理已断开，点击确认返回连接代理"),QMessageBox::Ok);
        msgsocket->deleteLater();
        msgsocket->disconnect();
        socket_Client *w = new socket_Client();
        w->show();
        close();
        delete this;
    });
}

clientMain::~clientMain()
{
    delete ui;
}

void clientMain::closeEvent(QCloseEvent *event){
    QMessageBox::StandardButton result = QMessageBox::question(this,tr("退出程序"),tr("是否退出程序？"),QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if(result == QMessageBox::Yes){
        msgsocket->disconnectFromHost();
        // event->accept();
    }else{
        event->ignore();
    }
}

void clientMain::on_pushButton_scanCsvFile_clicked()
{
    QFileDialog fileDialog;
    // fileDialog.setWindowTitle(tr("打开文件"));
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    // fileDialog.setNameFilter(tr("CSV(*.csv)"));
    // 打开文件选择对话框
    QString selectedFile = fileDialog.getOpenFileName(nullptr,  // 父窗口
                                                    "Open File",  // 窗口标题
                                                    "",          // 起始目录
                                                    "CSV(*.csv)");
    if(!selectedFile.isEmpty()){
        ui->lineEdit_csvFile->setText(selectedFile);
    }
}

void clientMain::on_pushButton_uploadFile_clicked()
{
    QJsonObject mContent;
    // 文件大小
    QString filepath = ui->lineEdit_csvFile->text();
    QFile fp(filepath);
    fp.open(QIODevice::ReadOnly | QIODevice::Text);
    this->fileSize = fp.size();
    mContent["fileSize"] = fileSize;
    QTextStream in(&fp);  // 用文件构造流
    QString tableHeader = in.readLine();    // 读取一行放到字符串里
    QString tableMode = in.readLine();
    mContent["tableHeader"] = tableHeader;
    mContent["tableMode"] = tableMode;
    fp.close();

    // 文件名
    QFileInfo fileinfo(filepath);
    this->fileName = fileinfo.fileName();
    mContent["fileName"] = fileName;

    // 发送 msgType::file
    myMsg msg(msgType::FILE_REQUEST,mContent);
    sendMsg(msgsocket,msg);

    // 创建 文件专用socket
    QTcpSocket *filesocket;
    connect(filesocket, &QTcpSocket::disconnected, this, [=](){
        QMessageBox::warning(nullptr,tr("文件代理"),tr("文件用代理已断开，稍后自动释放"),QMessageBox::Ok);
        filesocket->deleteLater();
        filesocket->disconnect();
    });
    //// TODO
    filesocket->connectToHost(msgsocket->peerAddress(), msgsocket->peerPort() + 1);
    if (filesocket->waitForConnected(3000)) { // 等待3000毫秒
        myQMsgBox msgBox(this);
        msgBox.setWindowTitle(tr("文件代理"));
        msgBox.setText(tr("文件用代理连接成功，开始传输文件"));
        msgBox.exec();
    } else {
        myQMsgBox msgBox(this,3000000,QMessageBox::Warning);
        msgBox.setWindowTitle(tr("文件代理"));
        msgBox.setText(tr("文件用代理连接失败"));
        msgBox.exec();
        return;
    }

    // 发送文件
    if(sendFile(filesocket,filepath)){
        myQMsgBox msgBox(this);
        msgBox.setWindowTitle(tr("sendfile"));
        msgBox.setText(tr("文件发送成功"));
        msgBox.exec();
        filesocket->disconnectFromHost();
    }else{
        myQMsgBox msgBox(this,3000000,QMessageBox::Warning);
        msgBox.setWindowTitle(tr("sendfile"));
        msgBox.setText(tr("文件发送失败"));
        msgBox.exec();
        filesocket->disconnectFromHost();
        return;
    }

    // 反馈
    myMsg msgres(msgType::NONE);
    recvMsg(msgsocket,msgres);

    if(msgres.getmType() == msgType::FILE_RECEIVE_SUCCESS){
        myQMsgBox msgBox(this);
        msgBox.setWindowTitle(tr("sendfile"));
        msgBox.setText(tr("代理接收文件成功"));
        msgBox.exec();
    }else{
        myQMsgBox msgBox(this,3000000,QMessageBox::Warning);
        msgBox.setWindowTitle(tr("sendfile"));
        msgBox.setText(tr("代理接收文件失败"));
        msgBox.exec();
    }
}


void clientMain::on_pushButton_sqlQuery_clicked()
{
    QJsonObject mContent;
    QString sql = ui->plainTextEdit_sqlQuery->toPlainText();
    mContent["sql"] = sql;

    QRegularExpression regex("SELECT\\s+.*?(?=;|\\s|$)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = regex.match(sql);
    QStringList headerList;
    QString headers;
    if (match.hasMatch()) {
        // 返回匹配的SELECT子句
        headers = match.captured(0);
        headerList = headers.split(',');
    }
    else {
        // 如果没有匹配到，返回空字符串
        headers = "";
        headerList = headers.split(',');
    }

    myMsg msg(msgType::SQL_REQUEST,mContent);
    sendMsg(msgsocket,msg);

    // 反馈
    myMsg msgres(msgType::NONE);
    recvMsg(msgsocket,msgres);

    if(msgres.getmType() != msgType::SQL_SUCCESS){
        myQMsgBox msgBox(this,3000000,QMessageBox::Warning);
        msgBox.setWindowTitle(tr("sendfile"));
        msgBox.setText(tr("代理查询失败"));
        msgBox.exec();
        return;
    }

    // 创建 查询专用socket
    QTcpSocket * sqlsocket;
    connect(sqlsocket, &QTcpSocket::disconnected, this, [=](){
        QMessageBox::warning(nullptr,tr("查询代理"),tr("查询用代理已断开，稍后自动释放"),QMessageBox::Ok);
        sqlsocket->deleteLater();
        sqlsocket->disconnect();
    });
    //// TODO
    sqlsocket->connectToHost(msgsocket->peerAddress(), msgsocket->peerPort() + 2);
    if (sqlsocket->waitForConnected(3000)) { // 等待3000毫秒
        myQMsgBox msgBox(this);
        msgBox.setWindowTitle(tr("查询代理"));
        msgBox.setText(tr("查询用代理连接成功，开始查询"));
        msgBox.exec();
    } else {
        myQMsgBox msgBox(this,3000000,QMessageBox::Warning);
        msgBox.setWindowTitle(tr("查询代理"));
        msgBox.setText(tr("查询用代理连接失败"));
        msgBox.exec();
        return;
    }

    // 接收查询结果
    recvSqlResult(sqlsocket,headerList);
    sqlsocket->disconnectFromHost();
}


bool clientMain::sendFile(QTcpSocket *socket, QString filepath)
{
    bool ret = false;
    QApplication::setOverrideCursor(Qt::WaitCursor);//设置鼠标为等待状态
    // QProgressDialog progress;
    // progress.setWindowTitle("sendFile");
    // progress.setLabelText("正在发送中...");
    // progress.setCancelButtonText("取消");
    // progress.setRange(0, totalBytes/bufSize);//设置范围
    // progress.setModal(true);//设置为模态对话框
    // progress.show();

    QFile fp(filepath);

    if (!fp.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,tr("QFile::open"),tr("文件打开失败"),QMessageBox::Ok);
        return false;
    }

    qint64 perloadSize = bufSize; //每一帧发送1024*1个字节，控制每次读取文件的大小
    double progressByte = 0;
    qint64 bytesWritten = 0, bytesRemain = fileSize;
    int cnt = 0;

    do{
        //用户取消的话则中止
        // if (progress.wasCanceled()){
        //     return false;
        // }
        // QCoreApplication::processEvents();

        if(bytesWritten < fileSize){
            QByteArray readBlock = fp.read(qMin(bytesRemain, perloadSize));
            qint64 WriteBolockSize = socket->write(readBlock, readBlock.size());
            qDebug() << QString("第%1次发送: %2B").arg(cnt).arg(readBlock.size());
            QThread::usleep(3); //添加延时，防止发送文件帧过快，导致丢包
            //等待发送完成，才能继续下次发送，
            if(!socket->waitForBytesWritten(3*1000)) {
                QMessageBox::warning(this,tr("发送文件"),tr("网络请求超时"),QMessageBox::Ok);
                ret = false;
            }
            bytesWritten += WriteBolockSize;
            int  progress = static_cast<int>(bytesWritten * 100.0 / fileSize);
            ui->progressBar->setValue(progress);
        }else if(bytesWritten == fileSize){
            ui->progressBar->setValue(100);
            fp.close();
            QApplication::restoreOverrideCursor();
            // progress.close();
            myQMsgBox msgBox(this,1000000);
            msgBox.setWindowTitle(tr("send file"));
            msgBox.setText(QString("send file success!(sendsize: %1B/%2B)").arg(bytesWritten).arg(fileSize));
            msgBox.exec();
            ret = true;
        }else {
            // QMessageBox::warning(this,tr("发送文件"),tr("error: bytesWritten > fileSize"),QMessageBox::Ok);
            myQMsgBox msgBox(this,3000000,QMessageBox::Warning);
            msgBox.setWindowTitle(tr("发送文件"));
            msgBox.setText(tr("error: bytesWritten > fileSize"));
            msgBox.exec();
            ret = false;
        }
    }while (true);
    return ret;
}

bool clientMain::recvSqlResult(QTcpSocket * socket,QStringList &headerList){
    // 创建模型
    QStandardItemModel *model = new QStandardItemModel();

    QByteArray data;
    while (socket->bytesAvailable() > 0) {
        data.append(socket->readAll());
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull()) {
        QJsonArray jsonArray = doc.array();
        int r = 0;
        for (const QJsonValue &rowValue : jsonArray) {
            const QJsonArray &rowArray = rowValue.toArray();
            vector<string> row;
            int c = 0;
            for (const QJsonValue &itemValue : rowArray) {
                QVariant item = itemValue.toVariant();
                QStandardItem *it = new QStandardItem(item.toString());
                model->setItem(r, c++, it);
                row.push_back(itemValue.toString().toStdString());
            }
            r++;
        }
    }else{
        return false;
    }

    model->setHorizontalHeaderLabels(headerList);
    ui->tableView->setModel(model);
    return true;
}
