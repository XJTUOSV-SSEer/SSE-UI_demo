#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include "msgSocket.h"

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQueryModel>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class clientMain;
}

class clientMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit clientMain(SOCKET fd,QWidget *parent = nullptr);
    ~clientMain();


private slots:
    void on_pushButton_scanCsvFile_clicked();

    void on_pushButton_uploadFile_clicked();

    void on_pushButton_sqlQuery_clicked();


protected:
    void closeEvent(QCloseEvent *event) override;

private:
    bool sendFile(SOCKET s, std::string filepath);

private:
    Ui::clientMain *ui;
    SOCKET msgsocket;
    SOCKET filesocket;
    qint64 totalBytes;
    QString fileName;
};

#endif // CLIENTMAIN_H
