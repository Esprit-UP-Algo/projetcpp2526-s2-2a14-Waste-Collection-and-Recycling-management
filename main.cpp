#include <QApplication>
#include <QMessageBox>
#include "gestion_des_poubelles.h"
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Tester la connexion à la base de données
    Connection c;
    bool test = c.createconnect();

    // Créer et afficher la fenêtre principale
    MainWindow w;

    if(test) {
        // Connexion réussie
        w.show();
        QMessageBox::information(nullptr, QObject::tr("Connexion Oracle"),
                                 QObject::tr("✅ Connexion réussie à la base de données Oracle !\n\n"
                                             "Username: eyamh\n"
                                             "Database: XE\n\n"
                                             "Click OK to continue."),
                                 QMessageBox::Ok);
    } else {
        // Connexion échouée - mais on lance quand même l'application
        w.show();
        QMessageBox::warning(nullptr, QObject::tr("Connexion Oracle"),
                             QObject::tr("⚠️ Impossible de se connecter à Oracle.\n\n"
                                         "L'application fonctionnera en mode hors-ligne.\n\n"
                                         "Vérifiez:\n"
                                         "- Oracle est démarré (services Windows)\n"
                                         "- Driver ODBC Oracle est installé\n"
                                         "- Username: eyamh / Password: 040705\n\n"
                                         "Click OK to continue."),
                             QMessageBox::Ok);
    }

    return a.exec();
}
