#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QString>
#include <QMessageBox>
#include <QSettings>

#define CONFIGPATH (QCoreApplication::applicationDirPath() + "/config/config.ini")

static const char* serverAddr = "127.0.0.1";
static const unsigned short msgPort = 20000;

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
    QTcpSocket * c_socket;
};

#endif // SOCKET_CLIENT_H
