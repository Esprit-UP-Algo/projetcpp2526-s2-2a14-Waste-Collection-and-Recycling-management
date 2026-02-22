#include "database.h"

QSqlDatabase Database::db;

bool Database::connect()
{
    db = QSqlDatabase::addDatabase("QODBC");

    // Driver exact : "Oracle in XE" (confirmé dans ODBC)
    db.setDatabaseName(
        "Driver={Oracle in XE};"
        "DBQ=localhost:1521/XE;"
        "Uid=TUNIWASTE;"
        "Pwd=tuni123;"
        );

    if (!db.open()) {
        qDebug() << "ERREUR connexion ODBC Oracle:" << db.lastError().text();
        return false;
    }

    qDebug() << "Connexion ODBC Oracle reussie - TUNIWASTE";
    return true;
}

void Database::disconnect()
{
    if (db.isOpen()) {
        db.close();
        qDebug() << "Deconnexion effectuee.";
    }
}

bool Database::isConnected()
{
    return db.isOpen();
}

QSqlDatabase Database::getDatabase()
{
    return db;
}
