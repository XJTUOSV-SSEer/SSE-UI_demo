#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QString>
#include <QMessageBox>
#include <QSettings>
#include <QPointer>

#define CONFIGPATH (QCoreApplication::applicationDirPath() + "/config/config.ini")


namespace Ui {
class socket_Client;
}

class socket_Client : public QWidget
{
    Q_OBJECT

public:
    explicit socket_Client(QWidget *parent = nullptr);
    ~socket_Client();

private slots:
    void on_pushButton_connect_clicked();

private:
    Ui::socket_Client *ui;
    QPointer<QTcpSocket> c_socket;
};

#endif // SOCKET_CLIENT_H
