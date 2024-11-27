#ifndef MYQMSGBOX_H
#define MYQMSGBOX_H

#include <QMessageBox>
#include <QTimer>

class myQMsgBox : public QMessageBox
{
    Q_OBJECT
public:
    explicit myQMsgBox(QWidget *parent = nullptr,int tim = 1000,QMessageBox::Icon icon = QMessageBox::Information);
};

#endif // MYQMSGBOX_H
