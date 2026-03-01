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
    static Database& instance();   // Singleton

    bool connecter();
    bool estOuverte() const;
    void fermer();
    QSqlDatabase& bd();

    // CRUD Camions
    bool insertCamion(const QString &idCamion, const QString &typeCamion,
                      const QString &loc,
                      const QString &cap,      const QString &fill,
                      const QString &state,    const QString &typeDechet,
                      const QString &idZone,   const QString &driver,
                      const QString &phone);

    bool updateCamion(int id,
                      const QString &idCamion, const QString &typeCamion,
                      const QString &loc,
                      const QString &cap,      const QString &fill,
                      const QString &state,    const QString &typeDechet,
                      const QString &idZone,   const QString &driver,
                      const QString &phone);

    bool deleteCamion(int id);

    QSqlQuery tousLesCamions();

    // Retourne les zones : liste de paires (ID_ZONE, NOM_ZONE)
    QList<QPair<int,QString>> chargerZones();

private:
    Database() = default;
    QSqlDatabase m_bd;
};

#endif // DATABASE_H
