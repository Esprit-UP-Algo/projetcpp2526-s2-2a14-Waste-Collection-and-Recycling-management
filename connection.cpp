#include "connection.h"
#include <QDebug>

Connection::Connection()
{
    // Constructeur vide
}

bool Connection::createconnect()
{
    bool test = false;

    qDebug() << "=================================";
    qDebug() << "Drivers SQL disponibles:" << QSqlDatabase::drivers();
    qDebug() << "=================================";

    // Connexion Oracle XE via ODBC
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    // ESSAI 1 : Avec le driver Oracle in XE
    db.setDatabaseName("Driver={Oracle in XE};DBQ=XE;");
    db.setUserName("eyamh");
    db.setPassword("040705");

    // Si ça ne marche pas, essayez avec OraClient11g_home1 :
    // db.setDatabaseName("Driver={Oracle in OraClient11g_home1};DBQ=XE;");

    qDebug() << "Tentative de connexion à Oracle XE...";
    qDebug() << "Driver: QODBC";
    qDebug() << "Service: XE";
    qDebug() << "Username: eyamh";

    if (db.open()) {
        test = true;
        qDebug() << "=================================";
        qDebug() << "✅ CONNEXION ORACLE XE RÉUSSIE !";
        qDebug() << "=================================";
        qDebug() << "Driver utilisé:" << db.driverName();
        qDebug() << "Database:" << db.databaseName();
    } else {
        qDebug() << "=================================";
        qDebug() << "❌ ERREUR DE CONNEXION";
        qDebug() << "=================================";
        qDebug() << "Message:" << db.lastError().text();
        qDebug() << "\nVérifications:";
        qDebug() << "1. Service OracleServiceXE est démarré";
        qDebug() << "2. Service OracleXETNSListener est démarré";
        qDebug() << "3. PATH contient: C:\\oraclexe\\app\\oracle\\product\\11.2.0\\server\\bin";
        qDebug() << "4. Driver ODBC visible dans Administrateur ODBC";
    }

    return test;
}
