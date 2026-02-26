#include "mainwindow.h"
#include "database.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Connexion à la base de données AVANT d'ouvrir la fenêtre
    if (!Database::instance().connecter()) {
        QMessageBox::critical(nullptr, "Erreur de connexion",
                              "Impossible de se connecter à Oracle XE.\n"
                              "Vérifiez que le serveur est démarré et que\n"
                              "les identifiants sont corrects (localhost:1521/XE).");
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
