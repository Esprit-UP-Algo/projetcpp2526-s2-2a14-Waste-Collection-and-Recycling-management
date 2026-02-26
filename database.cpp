#include "database.h"

Database& Database::instance()
{
    static Database inst;
    return inst;
}

bool Database::connecter()
{
    m_bd = QSqlDatabase::addDatabase("QODBC");
    m_bd.setDatabaseName(
        "Driver={Oracle in XE};"
        "DBQ=localhost:1521/XE;"
        "Uid=system;"
        "Pwd=tuni123;"
        );

    if (!m_bd.open()) {
        qDebug() << "[Database] Erreur connexion:" << m_bd.lastError().text();
        return false;
    }
    qDebug() << "[Database] Connecté à Oracle XE via ODBC";
    return true;
}

bool Database::estOuverte() const { return m_bd.isOpen(); }
void Database::fermer()           { m_bd.close(); }
QSqlDatabase& Database::bd()      { return m_bd; }

bool Database::insertCamion(const QString &idCamion, const QString &loc,
                            const QString &cap,      const QString &fill,
                            const QString &state,    const QString &type,
                            const QString &zone,     const QString &driver,
                            const QString &phone)
{
    QSqlQuery q(m_bd);
    q.prepare(R"(
        INSERT INTO CAMIONS
            (ID, ID_CAMION, LOCALISATION, CAPACITE, NIVEAU, ETAT, TYPE, ID_ZONE, CHAUFFEUR, TELEPHONE)
        VALUES
            (camions_seq.NEXTVAL, :idCamion, :loc, :cap, :fill, :state, :type, :zone, :driver, :phone)
    )");
    q.bindValue(":idCamion", idCamion);
    q.bindValue(":loc",      loc);
    q.bindValue(":cap",      cap);
    q.bindValue(":fill",     fill);
    q.bindValue(":state",    state);
    q.bindValue(":type",     type);
    q.bindValue(":zone",     zone);
    q.bindValue(":driver",   driver);
    q.bindValue(":phone",    phone);
    if (!q.exec()) {
        qDebug() << "[Database] insertCamion erreur:" << q.lastError().text();
        return false;
    }
    return true;
}

bool Database::updateCamion(int id,
                            const QString &idCamion, const QString &loc,
                            const QString &cap,      const QString &fill,
                            const QString &state,    const QString &type,
                            const QString &zone,     const QString &driver,
                            const QString &phone)
{
    QSqlQuery q(m_bd);
    q.prepare(R"(
        UPDATE CAMIONS SET
            ID_CAMION    = :idCamion,
            LOCALISATION = :loc,
            CAPACITE     = :cap,
            NIVEAU       = :fill,
            ETAT         = :state,
            TYPE         = :type,
            ID_ZONE      = :zone,
            CHAUFFEUR    = :driver,
            TELEPHONE    = :phone
        WHERE ID = :id
    )");
    q.bindValue(":id",       id);
    q.bindValue(":idCamion", idCamion);
    q.bindValue(":loc",      loc);
    q.bindValue(":cap",      cap);
    q.bindValue(":fill",     fill);
    q.bindValue(":state",    state);
    q.bindValue(":type",     type);
    q.bindValue(":zone",     zone);
    q.bindValue(":driver",   driver);
    q.bindValue(":phone",    phone);
    if (!q.exec()) {
        qDebug() << "[Database] updateCamion erreur:" << q.lastError().text();
        return false;
    }
    return true;
}

bool Database::deleteCamion(int id)
{
    QSqlQuery q(m_bd);
    q.prepare("DELETE FROM CAMIONS WHERE ID = :id");
    q.bindValue(":id", id);
    if (!q.exec()) {
        qDebug() << "[Database] deleteCamion erreur:" << q.lastError().text();
        return false;
    }
    return true;
}

QSqlQuery Database::tousLesCamions()
{
    QSqlQuery q(m_bd);
    q.exec("SELECT * FROM CAMIONS ORDER BY ID");
    return q;
}
