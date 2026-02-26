#include "database.h"

// Variable locale statique = créée UNE SEULE FOIS, thread-safe (C++11)
Database& Database::getInstance()
{
    static Database instance;
    return instance;
}

bool Database::connect()
{
    if (db.isOpen()) {
        qDebug() << "[Database] Déjà connecté.";
        return true;
    }

    // Tentative 1 : driver XEClient
    if (QSqlDatabase::contains("TUNIWASTE_CONN"))
        QSqlDatabase::removeDatabase("TUNIWASTE_CONN");

    db = QSqlDatabase::addDatabase("QODBC", "TUNIWASTE_CONN");
    db.setDatabaseName(
        "Driver={Oracle in XE};"
        "DBQ=localhost:1521/XE;"
        "Uid=TUNIWASTE;"
        "Pwd=tuni123;"
        );

    if (db.open()) {
        qDebug() << "[Database] ✔ Connexion Oracle réussie (Oracle in XE)";
        return true;
    }
    qDebug() << "[Database] ✘ Oracle in XE échoué :" << db.lastError().text();

    // Tentative 2 : via DSN nommé TUNIWASTE (configuré dans ODBC)
    QSqlDatabase::removeDatabase("TUNIWASTE_CONN");
    db = QSqlDatabase::addDatabase("QODBC", "TUNIWASTE_CONN");
    db.setDatabaseName("TUNIWASTE");

    if (db.open()) {
        qDebug() << "[Database] ✔ Connexion Oracle réussie (DSN TUNIWASTE)";
        return true;
    }
    qDebug() << "[Database] ✘ DSN TUNIWASTE échoué :" << db.lastError().text();

    return false;
}

void Database::disconnect()
{
    if (db.isOpen()) {
        db.close();
        QSqlDatabase::removeDatabase("TUNIWASTE_CONN");
        qDebug() << "[Database] Déconnexion effectuée.";
    }
}

bool Database::isConnected() const
{
    return db.isOpen();
}

QSqlDatabase Database::getDatabase() const
{
    return db;
}
