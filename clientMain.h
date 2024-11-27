#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include "msgSocket.h"
#include "myQMsgBox.h"

#include <QMainWindow>
#include <QtSql/QSqlQueryModel>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QProgressDialog>
#include <QStandardItemModel>
#include <QTableView>
#include <sstream>

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
    bool recvSqlResult(SOCKET socket);
    void setSqlRes2Table(std::vector<std::vector<std::string>> &sqlresult,QStringList &headers);

private:
    Ui::clientMain *ui;
    SOCKET msgsocket;
    SOCKET filesocket;
    qint64 totalBytes;
    QString fileName;
};

#endif // CLIENTMAIN_H
