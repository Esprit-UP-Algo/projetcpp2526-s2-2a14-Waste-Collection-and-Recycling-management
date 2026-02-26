#include "utilisateur.h"

Utilisateur::Utilisateur(int id, const QString& nom, const QString& prenom,
                         const QString& email, const QString& telephone,
                         const QString& motDePasse, const QString& role,
                         const QString& ville, const QString& codePostal,
                         const QString& sexe, const QString& photo)
    : id(id), nom(nom), prenom(prenom), email(email), telephone(telephone),
    motDePasse(motDePasse), role(role), ville(ville), codePostal(codePostal),
    sexe(sexe), photo(photo)
{}

// ══════════════════════════════════════════════════════════════════════════════
//  ajouter() — INSERT INTO UTILISATEUR
// ══════════════════════════════════════════════════════════════════════════════
bool Utilisateur::ajouter()
{
    QSqlDatabase db = Database::getInstance().getDatabase();

    // Générer un nouvel ID
    QSqlQuery seqQuery(db);
    seqQuery.exec("SELECT NVL(MAX(ID_UTILISATEUR), 0) + 1 FROM UTILISATEUR");
    if (seqQuery.next())
        id = seqQuery.value(0).toInt();

    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO UTILISATEUR "
        "(ID_UTILISATEUR, NOM, PRENOM, EMAIL, NUM_TELEPHONE, MOT_DE_PASSE, ROLE, VILLE, CODE_POSTAL, SEXE, PHOTO) "
        "VALUES (:id, :nom, :prenom, :email, :telephone, :motdepasse, :role, :ville, :codepostal, :sexe, :photo)"
        );

    query.bindValue(":id",          id);
    query.bindValue(":nom",         nom);
    query.bindValue(":prenom",      prenom);
    query.bindValue(":email",       email);
    query.bindValue(":telephone",   telephone);
    query.bindValue(":motdepasse",  motDePasse);
    query.bindValue(":role",        role);
    query.bindValue(":ville",       ville);
    query.bindValue(":codepostal",  codePostal);
    query.bindValue(":sexe",        sexe);
    query.bindValue(":photo",       photo);

    if (!query.exec()) {
        qDebug() << "[Utilisateur] Erreur ajouter :" << query.lastError().text();
        return false;
    }
    qDebug() << "[Utilisateur] Ajouté — ID :" << id;
    return true;
}

// ══════════════════════════════════════════════════════════════════════════════
//  afficher() — SELECT pour le QTableView (appelé après chaque opération CRUD)
// ══════════════════════════════════════════════════════════════════════════════
QSqlQueryModel* Utilisateur::afficher()
{
    QSqlDatabase db = Database::getInstance().getDatabase();

    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(
        "SELECT ID_UTILISATEUR, NOM, PRENOM, EMAIL, NUM_TELEPHONE, ROLE, VILLE, CODE_POSTAL, SEXE "
        "FROM UTILISATEUR ORDER BY ID_UTILISATEUR",
        db
        );

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Prénom");
    model->setHeaderData(3, Qt::Horizontal, "Email");
    model->setHeaderData(4, Qt::Horizontal, "Téléphone");
    model->setHeaderData(5, Qt::Horizontal, "Rôle");
    model->setHeaderData(6, Qt::Horizontal, "Ville");
    model->setHeaderData(7, Qt::Horizontal, "Code Postal");
    model->setHeaderData(8, Qt::Horizontal, "Sexe");

    if (model->lastError().isValid())
        qDebug() << "[Utilisateur] Erreur afficher :" << model->lastError().text();

    return model;
}

// ══════════════════════════════════════════════════════════════════════════════
//  modifier() — UPDATE WHERE ID_UTILISATEUR = :id
// ══════════════════════════════════════════════════════════════════════════════
bool Utilisateur::modifier()
{
    QSqlDatabase db = Database::getInstance().getDatabase();

    QSqlQuery query(db);
    query.prepare(
        "UPDATE UTILISATEUR SET "
        "NOM = :nom, PRENOM = :prenom, EMAIL = :email, NUM_TELEPHONE = :telephone, "
        "MOT_DE_PASSE = :motdepasse, ROLE = :role, VILLE = :ville, "
        "CODE_POSTAL = :codepostal, SEXE = :sexe, PHOTO = :photo "
        "WHERE ID_UTILISATEUR = :id"
        );

    query.bindValue(":nom",         nom);
    query.bindValue(":prenom",      prenom);
    query.bindValue(":email",       email);
    query.bindValue(":telephone",   telephone);
    query.bindValue(":motdepasse",  motDePasse);
    query.bindValue(":role",        role);
    query.bindValue(":ville",       ville);
    query.bindValue(":codepostal",  codePostal);
    query.bindValue(":sexe",        sexe);
    query.bindValue(":photo",       photo);
    query.bindValue(":id",          id);

    if (!query.exec()) {
        qDebug() << "[Utilisateur] Erreur modifier :" << query.lastError().text();
        return false;
    }
    qDebug() << "[Utilisateur] Modifié — ID :" << id;
    return true;
}

// ══════════════════════════════════════════════════════════════════════════════
//  supprimer(int id) — DELETE WHERE ID_UTILISATEUR = :id
// ══════════════════════════════════════════════════════════════════════════════
bool Utilisateur::supprimer(int id)
{
    QSqlDatabase db = Database::getInstance().getDatabase();

    QSqlQuery query(db);
    query.prepare("DELETE FROM UTILISATEUR WHERE ID_UTILISATEUR = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "[Utilisateur] Erreur supprimer :" << query.lastError().text();
        return false;
    }
    qDebug() << "[Utilisateur] Supprimé — ID :" << id;
    return true;
}
