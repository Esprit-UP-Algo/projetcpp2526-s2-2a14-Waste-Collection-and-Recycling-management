#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(1700, 1050);  // Augmenté pour voir tout le tableau
    window.show();

    return app.exec();
}
