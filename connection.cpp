#include "connection.h"
#include <QDebug>

Connection::Connection()
{
}

bool Connection::createconnect()
{
    bool test = false;

    qDebug() << "Drivers SQL disponibles:" << QSqlDatabase::drivers();

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("Driver={Oracle in XE};DBQ=XE;");
    db.setUserName("tuniwaste");
    db.setPassword("tuni123");

    qDebug() << "Tentative de connexion a Oracle XE...";

    if (db.open()) {
        test = true;
        qDebug() << "=================================";
        qDebug() << "CONNEXION ORACLE XE REUSSIE !";
        qDebug() << "=================================";
    } else {
        qDebug() << "=================================";
        qDebug() << "ERREUR DE CONNEXION";
        qDebug() << "=================================";
        qDebug() << "Message:" << db.lastError().text();
    }

    return test;
}

void Connection::closeConnection()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen()) {
        db.close();
        qDebug() << "Connexion fermee.";
    }
}

QSqlDatabase Connection::getDatabase()
{
    return QSqlDatabase::database();
}
