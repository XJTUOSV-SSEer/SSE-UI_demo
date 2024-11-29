#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QString>
#include <QMessageBox>
#include <QPointer>
#include <QJsonDocument>
#include <QJsonObject>

class socketServer : public QObject
{
    Q_OBJECT
public:
    explicit socketServer(QObject *parent = nullptr);

private:
    void* thread_func();


private:
    QPointer<QTcpServer> msgserver;
    QPointer<QTcpSocket> msgsocket;

};

#endif // SOCKETSERVER_H
