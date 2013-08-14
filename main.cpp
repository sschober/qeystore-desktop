#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("sschober");
    a.setOrganizationDomain("sssm.de");
    a.setApplicationName("qeystore");
    MainWindow w;
    w.show();
    
    return a.exec();
}
