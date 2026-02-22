#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>

class Database
{
public:
    static bool connect();
    static void disconnect();
    static bool isConnected();
    static QSqlDatabase getDatabase();

private:
    static QSqlDatabase db;
};

#endif // DATABASE_H
