#include "myQMsgBox.h"

myQMsgBox::myQMsgBox(QWidget *parent,QMessageBox::Icon icon,int tim): QMessageBox(parent) {
    this->setIcon(icon);
    this->setAttribute(Qt::WA_DeleteOnClose); // 确保消息框关闭时被删除

    // 设置定时器，1秒后执行槽函数
    QTimer::singleShot(tim, this, &QMessageBox::close);
}
