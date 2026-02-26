#include "camion.h"
#include "database.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// ══════════════════════════════════════════════════════════════
//  Singleton — retourne l'instance unique de Camion
// ══════════════════════════════════════════════════════════════
Camion& Camion::instance()
{
    static Camion inst;   // Créée une seule fois, détruite à la fin
    return inst;
}

// ══════════════════════════════════════════════════════════════
//  Méthode utilitaire privée
//  Construit un CamionData à partir de ses champs
// ══════════════════════════════════════════════════════════════
CamionData Camion::lireLigne(int id,
                             const QString &idCamion,
                             const QString &loc,
                             const QString &cap,
                             const QString &niv,
                             const QString &etat,
                             const QString &type,
                             const QString &zone,
                             const QString &chauffeur,
                             const QString &tel)
{
    CamionData c;
    c.id           = id;
    c.idCamion     = idCamion;
    c.localisation = loc;
    c.capacite     = cap;
    c.niveau       = niv;
    c.etat         = etat;
    c.type         = type;
    c.idZone       = zone;
    c.chauffeur    = chauffeur;
    c.telephone    = tel;
    return c;
}

// ══════════════════════════════════════════════════════════════
//  CREATE — Ajouter un nouveau camion dans la base
// ══════════════════════════════════════════════════════════════
bool Camion::ajouter(const QString &idCamion,
                     const QString &localisation,
                     const QString &capacite,
                     const QString &niveau,
                     const QString &etat,
                     const QString &type,
                     const QString &idZone,
                     const QString &chauffeur,
                     const QString &telephone)
{
    QSqlQuery q(Database::instance().bd());
    q.prepare(R"(
        INSERT INTO CAMIONS
            (ID, ID_CAMION, LOCALISATION, CAPACITE, NIVEAU,
             ETAT, TYPE, ID_ZONE, CHAUFFEUR, TELEPHONE)
        VALUES
            (camions_seq.NEXTVAL, :idCamion, :loc, :cap, :niv,
             :etat, :type, :zone, :chauffeur, :tel)
    )");
    q.bindValue(":idCamion",  idCamion);
    q.bindValue(":loc",       localisation);
    q.bindValue(":cap",       capacite);
    q.bindValue(":niv",       niveau);
    q.bindValue(":etat",      etat);
    q.bindValue(":type",      type);
    q.bindValue(":zone",      idZone);
    q.bindValue(":chauffeur", chauffeur);
    q.bindValue(":tel",       telephone);

    if (!q.exec()) {
        qDebug() << "[Camion] ajouter() erreur :" << q.lastError().text();
        return false;
    }
    qDebug() << "[Camion] Camion ajouté :" << idCamion;
    return true;
}

// ══════════════════════════════════════════════════════════════
//  READ — Récupérer tous les camions
// ══════════════════════════════════════════════════════════════
QList<CamionData> Camion::tousLesCamions()
{
    QList<CamionData> liste;
    QSqlQuery q(Database::instance().bd());

    if (!q.exec("SELECT ID, ID_CAMION, LOCALISATION, CAPACITE, NIVEAU, "
                "ETAT, TYPE, ID_ZONE, CHAUFFEUR, TELEPHONE "
                "FROM CAMIONS ORDER BY ID"))
    {
        qDebug() << "[Camion] tousLesCamions() erreur :" << q.lastError().text();
        return liste;
    }

    while (q.next()) {
        liste.append(lireLigne(
            q.value(0).toInt(),
            q.value(1).toString(),
            q.value(2).toString(),
            q.value(3).toString(),
            q.value(4).toString(),
            q.value(5).toString(),
            q.value(6).toString(),
            q.value(7).toString(),
            q.value(8).toString(),
            q.value(9).toString()
            ));
    }
    return liste;
}

// ══════════════════════════════════════════════════════════════
//  READ — Trouver un camion par son ID
// ══════════════════════════════════════════════════════════════
CamionData Camion::trouverParId(int id)
{
    QSqlQuery q(Database::instance().bd());
    q.prepare("SELECT ID, ID_CAMION, LOCALISATION, CAPACITE, NIVEAU, "
              "ETAT, TYPE, ID_ZONE, CHAUFFEUR, TELEPHONE "
              "FROM CAMIONS WHERE ID = :id");
    q.bindValue(":id", id);

    if (!q.exec() || !q.next()) {
        qDebug() << "[Camion] trouverParId() erreur :" << q.lastError().text();
        return CamionData{};
    }
    return lireLigne(
        q.value(0).toInt(),
        q.value(1).toString(),
        q.value(2).toString(),
        q.value(3).toString(),
        q.value(4).toString(),
        q.value(5).toString(),
        q.value(6).toString(),
        q.value(7).toString(),
        q.value(8).toString(),
        q.value(9).toString()
        );
}

// ══════════════════════════════════════════════════════════════
//  READ — Rechercher par état (ex: "Actif", "En panne")
// ══════════════════════════════════════════════════════════════
QList<CamionData> Camion::rechercherParEtat(const QString &etat)
{
    QList<CamionData> liste;
    QSqlQuery q(Database::instance().bd());
    q.prepare("SELECT ID, ID_CAMION, LOCALISATION, CAPACITE, NIVEAU, "
              "ETAT, TYPE, ID_ZONE, CHAUFFEUR, TELEPHONE "
              "FROM CAMIONS WHERE ETAT = :etat ORDER BY ID");
    q.bindValue(":etat", etat);

    if (!q.exec()) {
        qDebug() << "[Camion] rechercherParEtat() erreur :" << q.lastError().text();
        return liste;
    }
    while (q.next()) {
        liste.append(lireLigne(
            q.value(0).toInt(),
            q.value(1).toString(),
            q.value(2).toString(),
            q.value(3).toString(),
            q.value(4).toString(),
            q.value(5).toString(),
            q.value(6).toString(),
            q.value(7).toString(),
            q.value(8).toString(),
            q.value(9).toString()
            ));
    }
    return liste;
}

// ══════════════════════════════════════════════════════════════
//  READ — Rechercher par type (ex: "Benne", "Compacteur")
// ══════════════════════════════════════════════════════════════
QList<CamionData> Camion::rechercherParType(const QString &type)
{
    QList<CamionData> liste;
    QSqlQuery q(Database::instance().bd());
    q.prepare("SELECT ID, ID_CAMION, LOCALISATION, CAPACITE, NIVEAU, "
              "ETAT, TYPE, ID_ZONE, CHAUFFEUR, TELEPHONE "
              "FROM CAMIONS WHERE TYPE = :type ORDER BY ID");
    q.bindValue(":type", type);

    if (!q.exec()) {
        qDebug() << "[Camion] rechercherParType() erreur :" << q.lastError().text();
        return liste;
    }
    while (q.next()) {
        liste.append(lireLigne(
            q.value(0).toInt(),
            q.value(1).toString(),
            q.value(2).toString(),
            q.value(3).toString(),
            q.value(4).toString(),
            q.value(5).toString(),
            q.value(6).toString(),
            q.value(7).toString(),
            q.value(8).toString(),
            q.value(9).toString()
            ));
    }
    return liste;
}

// ══════════════════════════════════════════════════════════════
//  UPDATE — Modifier un camion existant
// ══════════════════════════════════════════════════════════════
bool Camion::modifier(int id,
                      const QString &idCamion,
                      const QString &localisation,
                      const QString &capacite,
                      const QString &niveau,
                      const QString &etat,
                      const QString &type,
                      const QString &idZone,
                      const QString &chauffeur,
                      const QString &telephone)
{
    QSqlQuery q(Database::instance().bd());
    q.prepare(R"(
        UPDATE CAMIONS SET
            ID_CAMION    = :idCamion,
            LOCALISATION = :loc,
            CAPACITE     = :cap,
            NIVEAU       = :niv,
            ETAT         = :etat,
            TYPE         = :type,
            ID_ZONE      = :zone,
            CHAUFFEUR    = :chauffeur,
            TELEPHONE    = :tel
        WHERE ID = :id
    )");
    q.bindValue(":id",        id);
    q.bindValue(":idCamion",  idCamion);
    q.bindValue(":loc",       localisation);
    q.bindValue(":cap",       capacite);
    q.bindValue(":niv",       niveau);
    q.bindValue(":etat",      etat);
    q.bindValue(":type",      type);
    q.bindValue(":zone",      idZone);
    q.bindValue(":chauffeur", chauffeur);
    q.bindValue(":tel",       telephone);

    if (!q.exec()) {
        qDebug() << "[Camion] modifier() erreur :" << q.lastError().text();
        return false;
    }
    qDebug() << "[Camion] Camion modifié ID :" << id;
    return true;
}

// ══════════════════════════════════════════════════════════════
//  DELETE — Supprimer un camion par son ID
// ══════════════════════════════════════════════════════════════
bool Camion::supprimer(int id)
{
    QSqlQuery q(Database::instance().bd());
    q.prepare("DELETE FROM CAMIONS WHERE ID = :id");
    q.bindValue(":id", id);

    if (!q.exec()) {
        qDebug() << "[Camion] supprimer() erreur :" << q.lastError().text();
        return false;
    }
    qDebug() << "[Camion] Camion supprimé ID :" << id;
    return true;
}
