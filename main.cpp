#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.showMaximized();   // toujours afficher la fenetre en premier

    Connection* c = Connection::instance();
    bool test = c->createConnect();

    if (test) {
        w.refreshTable();
        QMessageBox::information(&w,
                                 QObject::tr("Connexion à Oracle"),
                                 QObject::tr("Connexion à Oracle réussie."),
                                 QMessageBox::Ok);
    } else {
        QMessageBox::critical(&w,
                              QObject::tr("Connexion à Oracle"),
                              QObject::tr("Connexion à Oracle échouée.\nVérifiez la base de données."),
                              QMessageBox::Ok);
    }

    return a.exec();
}
