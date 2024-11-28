#ifndef INITWINDOW_H
#define INITWINDOW_H

#include "msgSocket.h"
#include "clientMain.h"
#include "myQMsgBox.h"

#include <QSettings>
#include <QString>
#include <QMessageBox>
#include <QWaitCondition>
#include <QPalette>
#include <QMainWindow>

#define CONFIGPATH (QCoreApplication::applicationDirPath() + "/config/config.ini")


QT_BEGIN_NAMESPACE
namespace Ui {
class initWindow;
}
QT_END_NAMESPACE

class initWindow : public QMainWindow
{
    Q_OBJECT

public:
    initWindow(QWidget *parent = nullptr);
    ~initWindow();

private slots:
    void on_pushButton_socket_clicked();

    void on_checkBox_checkStateChanged(const Qt::CheckState &arg1);

    void on_pushButton_login_clicked();

    void on_pushButton_quit_clicked();

private:
    Ui::initWindow *ui;
    bool ischeck;
    SOCKET csocket;
};
#endif // INITWINDOW_H
