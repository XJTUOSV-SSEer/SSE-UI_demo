#include "clientMain.h"
#include "ui_clientMain.h"
using namespace std;

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
    QFileDialog fileDialog;
    fileDialog.setWindowTitle(tr("打开文件"));
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setNameFilter(tr("CSV/YAML(*.csv *.yaml)"));
    // 打开文件选择对话框
    QString selectedFile = fileDialog.getOpenFileName(nullptr,  // 父窗口
                                                    "Open File",  // 窗口标题
                                                    "",          // 起始目录
                                                    "CSV/YAML(*.csv *.yaml)");
    if(!selectedFile.isEmpty()){
        ui->lineEdit_csvFile->setText(selectedFile);
    }
}

void clientMain::on_pushButton_uploadFile_clicked()
{
    // 文件大小
    QString filepath = ui->lineEdit_csvFile->text();
    QFile fp(filepath);
    fp.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&fp);  // 用文件构造流
    QString headers = in.readLine();    // 读取一行放到字符串里
    QString tmode = in.readLine();
    this->totalBytes = fp.size();
    fp.close();

    // 文件名
    QFileInfo fileinfo(filepath);
    this->fileName = fileinfo.fileName();
    std::string filename = this->fileName.toStdString();
    std::string info = filename +" "+ std::to_string(this->totalBytes) + " " + headers.toStdString()+" "+tmode.toStdString();

    // 发送 msgType::file
    myMsg msg(msgType::FILE_REQUEST,info);
    sendMsg(msgsocket,msg);

    // 创建 文件专用socket
    if(createSocket(filesocket)){
        myQMsgBox msgBox(this);
        msgBox.setWindowTitle(tr("createSocket"));
        msgBox.setText(tr("文件用代理连接成功，开始传输文件"));
        msgBox.show();
    }else{
        myQMsgBox msgBox(this,3000,QMessageBox::Warning);
        msgBox.setWindowTitle(tr("createSocket"));
        msgBox.setText(tr("文件用代理连接失败"));
        msgBox.show();
        return;
    }

    // 发送文件
    sendFile(filesocket,filepath.toStdString());
    shutdown(filesocket,SD_SEND);
    filesocket = INVALID_SOCKET;

    // 反馈
    myMsg msgres(msgType::NONE);
    recvMsg(msgsocket,msgres);

    if(msgres.getmsgType() == msgType::FILE_RECEIVE_SUCCESS){
        myQMsgBox msgBox(this);
        msgBox.setWindowTitle(tr("sendfile"));
        msgBox.setText(tr("代理接收文件成功"));
        msgBox.show();
    }else{
        myQMsgBox msgBox(this,3000,QMessageBox::Warning);
        msgBox.setWindowTitle(tr("sendfile"));
        msgBox.setText(tr("代理接收文件失败"));
        msgBox.show();
    }
}


void clientMain::on_pushButton_sqlQuery_clicked()
{
    QString sql = ui->plainTextEdit_sqlQuery->toPlainText();
    myMsg msg(msgType::SQL_REQUEST,sql.toStdString());
    sendMsg(msgsocket,msg);

    // 反馈
    myMsg msgres(msgType::NONE);
    recvMsg(msgsocket,msgres);

    if(msgres.getmsgType() != msgType::SQL_SUCCESS){
        myQMsgBox msgBox(this,3000,QMessageBox::Warning);
        msgBox.setWindowTitle(tr("sendfile"));
        msgBox.setText(tr("代理查询失败"));
        msgBox.show();
        return;
    }

    // 创建 查询专用socket
    SOCKET sqlsocket;
    if(createSocket(sqlsocket)){
        myQMsgBox msgBox(this);
        msgBox.setWindowTitle(tr("createSocket"));
        msgBox.setText(tr("查询用代理连接成功，开始查询"));
        msgBox.show();
    }else{
        myQMsgBox msgBox(this,3000,QMessageBox::Warning);
        msgBox.setWindowTitle(tr("createSocket"));
        msgBox.setText(tr("查询用代理连接失败"));
        msgBox.show();
        return;
    }

    // 接收查询结果
    recvSqlResult(sqlsocket);
    shutdown(sqlsocket,SD_SEND);
    sqlsocket = INVALID_SOCKET;
}


bool clientMain::sendFile(SOCKET s, string filepath)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);//设置鼠标为等待状态
    QProgressDialog progress;
    progress.setWindowTitle("sendFile");
    progress.setLabelText("正在发送中...");
    progress.setCancelButtonText("取消");
    progress.setRange(0, totalBytes/bufSize);//设置范围
    progress.setModal(true);//设置为模态对话框
    progress.show();

    FILE* fp = fopen(filepath.c_str(), "rb");
    if (!fp){
        QMessageBox::warning(nullptr,QString("fopen"),QString("Error in sendFile()"),QMessageBox::Cancel);
        return false;
    }

    //buffer
    int readlen = 0, sendlen = 0, cnt = 0;
    uint sendcnt = 0, readcnt = 0;
    char buffer[bufSize + 1] = {'\0'};
    //循环读取文件进行传送
    do{
        //用户取消的话则中止
        if (progress.wasCanceled()){
            return false;
        }
        QCoreApplication::processEvents();

        memset(buffer,'\0',bufSize + 1 );
        readlen = fread(buffer, 1, bufSize, fp);

        if(readlen > 0){
            readcnt += readlen;
            sendlen = send(s, buffer, readlen, 0);
            if (sendlen == SOCKET_ERROR){
                myQMsgBox msgBox(this,3000,QMessageBox::Warning);
                msgBox.setWindowTitle(tr("send"));
                msgBox.setText(tr("Error in sendFile()"));
                msgBox.show();
                return false;
            }
            cnt ++;
            sendcnt += sendlen;
            progress.setValue(cnt);
        }else if(readlen == 0){
            break;
        }else{
            myQMsgBox msgBox(this,3000,QMessageBox::Warning);
            msgBox.setWindowTitle(tr("fread"));
            msgBox.setText(tr("Error in sendFile()"));
            msgBox.show();
            return false;
        }
    }while (true);

    fclose(fp);
    QApplication::restoreOverrideCursor();
    progress.close();
    myQMsgBox msgBox(this);
    msgBox.setWindowTitle(tr("send file"));
    msgBox.setText(QString("send file success!(sendsize: %1B/%2B)").arg(sendcnt).arg(this->totalBytes));
    msgBox.show();
    return true;
}

bool clientMain::recvSqlResult(SOCKET socket){
    int recvlen = 0, recvcnt = 0, r = 0;
    char buffer[bufSize + 1] = {'\0'};
    // 创建模型
    QStandardItemModel *model = new QStandardItemModel();
    do{
        memset(buffer,'\0',bufSize + 1);
        recvlen = recv(socket, buffer, bufSize, 0);
        if(recvlen > 0){
            recvcnt += recvlen;
            string line(buffer),cell;
            istringstream lineStream(line);
            int c = 0;
            while (std::getline(lineStream, cell, ',')) {
                QStandardItem *item = new QStandardItem(cell.c_str());
                model->setItem(r, c++, item);
            }
            r++;
        }else if(recvlen == 0){
            printf("result reception completed\n");
            break;
        }else{
            printf("recv failed: %s\n", strerror(errno));
            return false;
        }
    } while (true);
    // model->setHorizontalHeaderLabels(headers);
    ui->tableView->setModel(model);
    return true;
}

void clientMain::setSqlRes2Table(vector<vector<string>> &sqlresult,QStringList &headers){
    // 创建模型
    QStandardItemModel *model = new QStandardItemModel();
    int rowCount = sqlresult.size(); // 行数
    int columnCount = sqlresult.empty() ? 0 : sqlresult[0].size(); // 列数
    model->setRowCount(rowCount);
    model->setColumnCount(columnCount);

    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < columnCount; ++j) {
            QStandardItem *item = new QStandardItem(sqlresult[i][j].c_str());
            model->setItem(i, j, item);
        }
    }

    model->setHorizontalHeaderLabels(headers);
    ui->tableView->setModel(model);
}
