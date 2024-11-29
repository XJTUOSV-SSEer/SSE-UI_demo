#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QStandardItemModel>
#include <QPointer>
#include <QFile>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QTcpSocket>

namespace Ui {
class clientMain;
}

class clientMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit clientMain(QPointer<QTcpSocket> socket,QWidget *parent = nullptr);
    ~clientMain();


private slots:
    void on_pushButton_scanCsvFile_clicked();

    void on_pushButton_uploadFile_clicked();

    void on_pushButton_sqlQuery_clicked();


protected:
    void closeEvent(QCloseEvent *event) override;

private:
    bool sendFile(QTcpSocket *socket, QString filepath);
    bool recvSqlResult(QTcpSocket * socket,QStringList &headerList);

private:
    Ui::clientMain *ui;
    QPointer<QTcpSocket> msgsocket;
    qint64 fileSize;
    QString fileName;
};

#endif // CLIENTMAIN_H
