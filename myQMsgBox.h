#pragma once
#ifndef MYQMSGBOX_H
#define MYQMSGBOX_H

#include <QMessageBox>
#include <QTimer>

class myQMsgBox : public QMessageBox
{
    Q_OBJECT
public:
    explicit myQMsgBox(QWidget *parent = nullptr,QMessageBox::Icon icon = QMessageBox::Information,int tim = 1000);
};

#endif // MYQMSGBOX_H
