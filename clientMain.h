#pragma once
#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include "msgSocket.h"
#include "myQMsgBox.h"

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQueryModel>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlQuery>
#include <QStringList>
#include <QTextStream>
#include <QTimer>
#include <QProgressDialog>
#include <QStandardItemModel>
#include <QTableView>

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
    bool sendFile(SOCKET socket, std::string filepath);
    bool recvSqlResult(SOCKET socket,std::vector<std::vector<std::string>> &sqlresult);
    void setSqlRes2Table(std::vector<std::vector<std::string>> &sqlresult,QStringList &headers);

private:
    Ui::clientMain *ui;
    SOCKET msgsocket;
    SOCKET filesocket;
    SOCKET sqlsocket;
    qint64 totalBytes;
    QString fileName;
};

#endif // CLIENTMAIN_H
