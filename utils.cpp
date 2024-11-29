#include "utils.h"

myMsg::myMsg() {
    this->mtype = msgType::NONE;
    QJsonObject emptyObject;
    this->mContent = emptyObject;
}

myMsg::myMsg(enum msgType mtype){
    this->mtype = mtype;
    QJsonObject emptyObject;
    this->mContent = emptyObject;
}

myMsg::myMsg(enum msgType mtype, QJsonObject mcontent){
    this->mtype = mtype;
    this->mContent = mcontent;
}

msgType myMsg::getmType(){
    return this->mtype;
}

QJsonObject myMsg::getmContent(){
    return this->mContent;
}

void myMsg::setmType(enum msgType mtype){
    this->mtype = mtype;
}

void myMsg::setmContent(QJsonObject mcontent){
    this->mContent = mcontent;
}


// 接收消息
bool recvMsg(QTcpSocket * socket,myMsg &msg){
    msg.setmType(msgType::NONE);
    QByteArray buffer;
    buffer = socket->readAll();
    if(buffer.size() <= 0){
        QMessageBox::warning(nullptr,QString("read"),QString("error in recvMsg()"));
        return false;
    }
    // 转换为QJsonDocument
    QJsonDocument recvJsonDocument = QJsonDocument::fromJson(buffer);
    if (recvJsonDocument.isObject()) {
        QJsonObject recvJson = recvJsonDocument.object();
        int mType = recvJson["msgType"].toInt();
        msg.setmType(static_cast<msgType>(mType));
        recvJson.remove("msgType");
        msg.setmContent(recvJson);
    } else {
        QMessageBox::warning(nullptr,QString("Json"),QString("error in QJsonDocument::fromJson()"));
        return false;
    }

    return true;
}

//发送消息
bool sendMsg(QTcpSocket * socket,myMsg &msg){
    QJsonObject sendJson, msgContent = msg.getmContent();
    sendJson["msgType"] = static_cast<int>(msg.getmType());
    for (auto it = msgContent.begin(); it != msgContent.end(); ++it) {
        sendJson.insert(it.key(),static_cast<QJsonValue>(it.value()));
    }

    // 转换为QJsonDocument
    QJsonDocument sendJsonDocument(sendJson);
    // 转换为JSON字符串
    QByteArray buffer = sendJsonDocument.toJson(QJsonDocument::Compact);
    int ret = -1;
    ret = socket->write(buffer,buffer.size());
    socket->flush();
    if(ret < 0){
        QMessageBox::warning(nullptr,QString("write"),QString("error in sendMsg()"));
        return false;
    }

    return true;
}

// 接收文件
bool recvFile(QTcpSocket *filesocket,QString fileName,qint64 fileSize)
{
    QFile fp(fileName);
    if (!fp.open(QIODevice::ReadWrite)){
        printf("Failed to open file\n");
        return false;
    }

    qint64 recvcnt = 0, writecnt = 0;
    while(recvcnt < fileSize){
        QByteArray readBlock = filesocket->readLine();
        if (readBlock.isEmpty()) continue; // 忽略空行
        int recvBlockSize = readBlock.size();
        recvcnt += recvBlockSize;
        int writelen = fp.write(readBlock);
        if(writelen <= 0 && recvBlockSize > 0){
            printf("write file with error\n");
            fp.close();
            return false;
        }
        writecnt += writelen;
    }
    fp.close();
    printf("File reception completed\n");
    if(recvcnt == fileSize && writecnt == fileSize){
        printf("receive and write file success!\n");
        return true;
    }else{
        printf("receive or write file with error!\n");
        return false;
    }
}

// 发送结果
bool sendSqlResult(QTcpSocket *socket, std::vector<std::vector<std::string>> &sqlResult){
    for (const auto& row : sqlResult) {
        QJsonArray jsonRow;
        for (const auto& field : row) {
            jsonRow.append(QString::fromStdString(field));
        }

        QJsonDocument doc(jsonRow);
        QByteArray jsonBytes = doc.toJson(QJsonDocument::Compact); // 将JSON数组转换为字节数组

        socket->write(jsonBytes); // 发送JSON数据
        socket->write("\n"); // 发送行结束符，表示一行结束
        socket->flush();
        QThread::usleep(3); //添加延时，防止发送文件帧过快，导致丢包
        //等待发送完成，才能继续下次发送，
        if(!socket->waitForBytesWritten(3*1000)) {
            printf("timeout");
            return false;
        }
    }
    return true;
}
