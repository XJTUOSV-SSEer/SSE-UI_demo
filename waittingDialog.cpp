#include "waittingDialog.h"
#include "ui_waittingDialog.h"

waittingDialog::waittingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::waittingDialog)
    , m_Move(NULL)
{
    ui->setupUi(this);
    //设置透明度
    this->setWindowOpacity(0.8);
    //取消对话框标题
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);//设置为对话框风格，并且去掉边框
    setWindowModality(Qt::WindowModal);//设置为模式对话框，同时在构造该对话框时要设置父窗口
    ui->label->setStyleSheet("background-color: transparent;");
    m_Move = new QMovie("./icon/loading.gif");
    ui->label->setMovie(m_Move);
    ui->label->setScaledContents(true);
    m_Move->start();
}

waittingDialog::~waittingDialog()
{
    m_Move->stop();
}
