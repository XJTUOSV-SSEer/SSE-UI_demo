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
