#include "initwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    initWindow w;
    w.show();
    return a.exec();
}
