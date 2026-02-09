#include <QApplication>
#include "recyclage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Recyclage w;
    w.showMaximized();
    return a.exec();
}
