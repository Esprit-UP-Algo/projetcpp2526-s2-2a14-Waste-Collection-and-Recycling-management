#include <QApplication>
#include "gestionzones.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GestionZones window;
    window.show();

    return app.exec();
}
