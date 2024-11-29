#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QSettings>
#include <QString>
#include <QMessageBox>
#include <QMainWindow>
#include <QPointer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

#define CONFIGPATH (QCoreApplication::applicationDirPath() + "/config/config.ini")


QT_BEGIN_NAMESPACE
namespace Ui {
class dbConnection;
}
QT_END_NAMESPACE

class dbConnection : public QMainWindow
{
    Q_OBJECT

public:
    dbConnection(QPointer<QTcpSocket> socket,QWidget *parent = nullptr);
    ~dbConnection();

private slots:
    void on_pushButton_change_clicked();

    void on_checkBox_checkStateChanged(const Qt::CheckState &arg1);

    void on_pushButton_login_clicked();

    void on_pushButton_quit_clicked();

private:
    Ui::dbConnection *ui;
    bool ischeck;
    QPointer<QTcpSocket> c_socket;
};
#endif // DBCONNECTION_H
