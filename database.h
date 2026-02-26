#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>

// ══════════════════════════════════════════════════════════════════════════════
//  Patron Singleton — une seule instance, une seule connexion Oracle ODBC
//
//  Utilisation dans tout le projet :
//      Database::getInstance().connect();        // main.cpp
//      Database::getInstance().getDatabase();    // utilisateur.cpp
//      Database::getInstance().isConnected();    // vérification
// ══════════════════════════════════════════════════════════════════════════════
class Database
{
public:
    // Accès à l'unique instance
    static Database& getInstance();

    bool         connect();
    void         disconnect();
    bool         isConnected() const;
    QSqlDatabase getDatabase() const;

private:
    Database() {}                                // constructeur privé
    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;

    QSqlDatabase db;
};

#endif // DATABASE_H
