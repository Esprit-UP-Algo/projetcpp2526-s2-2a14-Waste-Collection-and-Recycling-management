#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set application information
    QApplication::setApplicationName("TuniWaste");
    QApplication::setOrganizationName("TuniWaste");
    QApplication::setApplicationVersion("1.0.0");

    MainWindow w;
    w.show();

    return a.exec();
}
