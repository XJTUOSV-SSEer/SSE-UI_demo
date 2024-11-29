#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QWidget>
#include <QTcpSocket>
#include <QString>
#include <QMessageBox>
#include <QSettings>
#include <QPointer>

#define CONFIGPATH (QCoreApplication::applicationDirPath() + "/config/config.ini")

namespace Ui {
class socketClient;
}

class socketClient : public QWidget
{
    Q_OBJECT

public:
    explicit socketClient(QWidget *parent = nullptr);
    ~socketClient();

private slots:
    void on_pushButton_connect_clicked();

private:
    Ui::socketClient *ui;
    QPointer<QTcpSocket> c_socket;
};

#endif // SOCKETCLIENT_H
