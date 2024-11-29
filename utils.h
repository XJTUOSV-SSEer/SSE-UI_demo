#ifndef UTILS_H
#define UTILS_H

#include <QTcpSocket>
#include <QMessageBox>
#include <cstdio>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QThread>

static const int bufSize = 1024;
// static const char* serverAddr = "127.0.0.1";
static const unsigned short msgPort = 20000;
// static const unsigned short filePort = 20001;
// static const unsigned short sqlResPort = 20002;


enum class msgType : int
{
    NONE                    = 0,
    CONNECTION_REQUEST      = 1,
    CONNECTION_SUCCESS      = 2,
    CONNECTION_FAILED       = 3,
    FILE_REQUEST            = 4,
    FILE_RECEIVE_SUCCESS    = 5,
    FILE_RECEIVE_FAILED     = 6,
    SQL_REQUEST             = 7,
    SQL_SUCCESS             = 8,
    SQL_FAILED              = 9,
};

class myMsg
{
public:
    myMsg();
    myMsg(msgType mtype);
    myMsg(msgType mtype, QJsonObject mcontent);

    msgType getmType();
    QJsonObject getmContent();

    void setmType(msgType);
    void setmContent(QJsonObject);

private:
    msgType mtype;
    QJsonObject mContent;
};



bool recvMsg(QTcpSocket *socket,myMsg &msg);
bool sendMsg(QTcpSocket *socket,myMsg &msg);
bool recvFile(QTcpSocket *filesocket,QString fileName,qint64 fileSize);
bool sendSqlResult(QTcpSocket *sqlSocket,std::vector<std::vector<std::string>> &sqlResult);

#endif // UTILS_H
