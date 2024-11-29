#include "socket_Client.h"

#include <QApplication>
#include <QStyleFactory>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("windows11"); // 设置Fusion风格:windows11 windowsvista Windows Fusion
    socket_Client w;
    w.show();
    return a.exec();
}
