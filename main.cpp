#include "mainwindow.h"
#include <QApplication>


MainWindow *p;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w; p = &w;
    w.show();

    return a.exec();
}
