#ifndef MYQMSGBOX_H
#define MYQMSGBOX_H

#include <QMessageBox>
#include <QTimer>

class myQMsgBox : public QMessageBox
{
    Q_OBJECT
public:
    myQMsgBox(QWidget *parent = nullptr,int timeout = 1000,QMessageBox::Icon icon = QMessageBox::Information): QMessageBox(parent), m_timeout(timeout){
        setIcon(icon);
        // setAttribute(Qt::WA_DeleteOnClose); // 确保消息框关闭时被删除
        setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        setDefaultButton(QMessageBox::Ok);
        QTimer::singleShot(timeout, this, &myQMsgBox::close);// 设置定时器，1秒后执行槽函数
    }
private:
    int m_timeout;
};

#endif // MYQMSGBOX_H
