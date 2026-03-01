#include "gestion_des_poubelles.h"
#include <QApplication>
#include <QMessageBox>
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 1. On prépare la connexion d'abord
    Connection* c = Connection::instance();
    bool test = c->createConnect();
    
    // 2. On ne crée la fenêtre qu'APRES l'ouverture de la connexion
    if(test)
    {
        MainWindow w; // Créée ici, elle pourra charger les données sans erreur
        w.show();
        QMessageBox::information(nullptr, QObject::tr("database is open"),
                    QObject::tr("connection successful.\n"
                                "Click Cancel to exit."), QMessageBox::Cancel);
        return a.exec();
    }
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("database is not open"),
                    QObject::tr("connection failed.\n"
                                "Click Cancel to exit."), QMessageBox::Cancel);
        return 0; // On quitte si la connexion échoue
    }
}
