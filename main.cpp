#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QDebug>
#include "mainwindow.h"
#include "database.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Vérifie QODBC (pas QOCI)
    if (!QSqlDatabase::isDriverAvailable("QODBC")) {
        QMessageBox::critical(nullptr,
                              "Driver ODBC manquant",
                              "Le driver QODBC n'est pas disponible.\n\n"
                              "Drivers disponibles : " + QSqlDatabase::drivers().join(", "));
        return -1;
    }

    // Connexion Oracle via ODBC
    if (!Database::connect()) {
        QMessageBox::critical(nullptr,
                              "Erreur de connexion Oracle",
                              "Impossible de se connecter à Oracle.\n\n"
                              "Vérifiez que :\n"
                              "1. Oracle XE est démarré\n"
                              "2. Le DSN TUNIWASTE existe dans ODBC\n"
                              "3. Le mot de passe est correct (tuni123)");
        return -1;
    }

    MainWindow window;
    window.resize(1700, 1050);
    window.show();

    int result = app.exec();
    Database::disconnect();
    return result;
}
