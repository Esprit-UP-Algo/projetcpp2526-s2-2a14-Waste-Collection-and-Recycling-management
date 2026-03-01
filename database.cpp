#include "database.h"

// Nom fixe pour la connexion Qt — évite les avertissements de remplacement
static const QString CONN_NAME = QStringLiteral("tuniwaste_conn");

Database& Database::instance()
{
    static Database inst;
    return inst;
}

bool Database::connecter()
{
    // Si la connexion existe déjà et est ouverte, on ne refait rien
    if (QSqlDatabase::contains(CONN_NAME)) {
        m_bd = QSqlDatabase::database(CONN_NAME, false);
        if (m_bd.isOpen()) {
            qDebug() << "[Database] Connexion déjà active.";
            return true;
        }
        // Connexion existante mais fermée : on la supprime pour la recréer
        QSqlDatabase::removeDatabase(CONN_NAME);
    }

    m_bd = QSqlDatabase::addDatabase("QODBC", CONN_NAME);
    // Utilise le DSN ODBC "tuniwaste" configuré dans l'Administrateur ODBC Windows
    m_bd.setDatabaseName("tuniwaste");
    m_bd.setUserName("tuniwaste");
    m_bd.setPassword("tuni123");

    if (!m_bd.open()) {
        qDebug() << "[Database] Erreur connexion:" << m_bd.lastError().text();
        return false;
    }
    qDebug() << "[Database] Connecté à Oracle XE via ODBC";
    return true;
}

bool Database::estOuverte() const { return m_bd.isValid() && m_bd.isOpen(); }

void Database::fermer()
{
    if (m_bd.isOpen())
        m_bd.close();
    // Libère la connexion nommée du registre Qt
    m_bd = QSqlDatabase(); // détacher la référence avant removeDatabase
    QSqlDatabase::removeDatabase(CONN_NAME);
}

QSqlDatabase& Database::bd()      { return m_bd; }

bool Database::insertCamion(const QString &idCamion, const QString &typeCamion,
                            const QString &loc,
                            const QString &cap,      const QString &fill,
                            const QString &state,    const QString &typeDechet,
                            const QString &idZone,   const QString &driver,
                            const QString &phone)
{
    QSqlQuery q(m_bd);
    q.prepare(R"(
        INSERT INTO CAMIONS
            (ID, ID_CAMION, TYPE_CAMION, LOCALISATION, CAPACITE, NIVEAU,
             ETAT, TYPE, ID_ZONE, CHAUFFEUR, TELEPHONE)
        VALUES
            (camions_seq.NEXTVAL, :idCamion, :typeCamion, :loc, :cap, :fill,
             :state, :type, :zone, :driver, :phone)
    )");
    q.bindValue(":idCamion",   idCamion);
    q.bindValue(":typeCamion", typeCamion);
    q.bindValue(":loc",        loc);
    q.bindValue(":cap",        cap);
    q.bindValue(":fill",       fill);
    q.bindValue(":state",      state);
    q.bindValue(":type",       typeDechet);
    q.bindValue(":zone",       idZone);
    q.bindValue(":driver",     driver);
    q.bindValue(":phone",      phone);
    if (!q.exec()) {
        qDebug() << "[Database] insertCamion erreur:" << q.lastError().text();
        return false;
    }
    return true;
}

bool Database::updateCamion(int id,
                            const QString &idCamion, const QString &typeCamion,
                            const QString &loc,
                            const QString &cap,      const QString &fill,
                            const QString &state,    const QString &typeDechet,
                            const QString &idZone,   const QString &driver,
                            const QString &phone)
{
    QSqlQuery q(m_bd);
    q.prepare(R"(
        UPDATE CAMIONS SET
            ID_CAMION    = :idCamion,
            TYPE_CAMION  = :typeCamion,
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
    q.bindValue(":id",         id);
    q.bindValue(":idCamion",   idCamion);
    q.bindValue(":typeCamion", typeCamion);
    q.bindValue(":loc",        loc);
    q.bindValue(":cap",        cap);
    q.bindValue(":fill",       fill);
    q.bindValue(":state",      state);
    q.bindValue(":type",       typeDechet);
    q.bindValue(":zone",       idZone);
    q.bindValue(":driver",     driver);
    q.bindValue(":phone",      phone);
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
    q.exec("SELECT ID, ID_CAMION, TYPE_CAMION, LOCALISATION, CAPACITE, NIVEAU, ETAT, TYPE, ID_ZONE, CHAUFFEUR, TELEPHONE FROM CAMIONS ORDER BY ID");
    return q;
}

QList<QPair<int,QString>> Database::chargerZones()
{
    QList<QPair<int,QString>> zones;
    QSqlQuery q(m_bd);

    // Essayer d'abord avec le nom simple, puis avec le schéma qualifié
    QStringList candidates = {
        "SELECT ID_ZONE, NOM_ZONE FROM ZONE ORDER BY NOM_ZONE",
        "SELECT ID_ZONE, NOM_ZONE FROM TUNIWASTE.ZONE ORDER BY NOM_ZONE"
    };
    bool success = false;
    for (const QString &sql : candidates) {
        if (q.exec(sql)) {
            success = true;
            break;
        }
        qDebug() << "[Database] chargerZones essai:" << sql
                 << "-> erreur:" << q.lastError().text();
    }
    if (!success) {
        qDebug() << "[Database] chargerZones: impossible de lire la table ZONE.";
        return zones;
    }
    while (q.next())
        zones.append(qMakePair(q.value(0).toInt(), q.value(1).toString()));

    qDebug() << "[Database] chargerZones:" << zones.size() << "zones chargées.";
    return zones;
}
