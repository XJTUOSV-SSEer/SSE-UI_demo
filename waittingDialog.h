#ifndef WAITTINGDIALOG_H
#define WAITTINGDIALOG_H

#include <QDialog>
#include <QMovie>

namespace Ui {
class waittingDialog;
}

class waittingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit waittingDialog(QWidget *parent = nullptr);
    ~waittingDialog();

private:
    Ui::waittingDialog* ui;
    QMovie* m_Move;
};

#endif // WAITTINGDIALOG_H
