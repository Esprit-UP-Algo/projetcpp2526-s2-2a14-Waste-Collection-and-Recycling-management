#include "utilisateur.h"

Utilisateur::Utilisateur(int id, QString nom, QString prenom,
                         QString email, QString telephone,
                         QString motDePasse, QString role,
                         QString ville, QString codePostal,
                         QString sexe, QString photo)
{
    this->id         = id;
    this->nom        = nom;
    this->prenom     = prenom;
    this->email      = email;
    this->telephone  = telephone;
    this->motDePasse = motDePasse;
    this->role       = role;
    this->ville      = ville;
    this->codePostal = codePostal;
    this->sexe       = sexe;
    this->photo      = photo;
}

// ══════════════════════════════════════════════════════════════════════════════
//  ajouter() — INSERT INTO UTILISATEUR
// ══════════════════════════════════════════════════════════════════════════════
bool Utilisateur::ajouter()
{
    QSqlQuery query;

    QString res = QString::number(id);

    // prepare() prend la requête en paramètre pour la préparer à l'exécution.
    query.prepare("INSERT INTO UTILISATEUR "
                  "(ID_UTILISATEUR, NOM, PRENOM, EMAIL, NUM_TELEPHONE, MOT_DE_PASSE, ROLE, VILLE, CODE_POSTAL, SEXE, PHOTO) "
                  "VALUES (:id, :nom, :prenom, :email, :telephone, :motdepasse, :role, :ville, :codepostal, :sexe, :photo)");

    // Création des variables liées
    query.bindValue(":id",         res);
    query.bindValue(":nom",        nom);
    query.bindValue(":prenom",     prenom);
    query.bindValue(":email",      email);
    query.bindValue(":telephone",  telephone);
    query.bindValue(":motdepasse", motDePasse);
    query.bindValue(":role",       role);
    query.bindValue(":ville",      ville);
    query.bindValue(":codepostal", codePostal);
    query.bindValue(":sexe",       sexe);
    query.bindValue(":photo",      photo);

    return query.exec(); // exec() envoie la requête pour l'exécuter
}

// ══════════════════════════════════════════════════════════════════════════════
//  afficher() — SELECT pour le QTableView
// ══════════════════════════════════════════════════════════════════════════════
QSqlQueryModel* Utilisateur::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();

    model->setQuery("SELECT ID_UTILISATEUR, NOM, PRENOM, EMAIL, NUM_TELEPHONE, ROLE, VILLE, CODE_POSTAL, SEXE "
                    "FROM UTILISATEUR ORDER BY ID_UTILISATEUR");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Email"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Téléphone"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Rôle"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Ville"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Code Postal"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("Sexe"));

    return model;
}

// ══════════════════════════════════════════════════════════════════════════════
//  modifier() — UPDATE WHERE ID_UTILISATEUR = :id
// ══════════════════════════════════════════════════════════════════════════════
bool Utilisateur::modifier()
{
    QSqlQuery query;

    QString res = QString::number(id);

    // prepare() prend la requête en paramètre pour la préparer à l'exécution.
    query.prepare("UPDATE UTILISATEUR SET "
                  "NOM = :nom, PRENOM = :prenom, EMAIL = :email, NUM_TELEPHONE = :telephone, "
                  "MOT_DE_PASSE = :motdepasse, ROLE = :role, VILLE = :ville, "
                  "CODE_POSTAL = :codepostal, SEXE = :sexe, PHOTO = :photo "
                  "WHERE ID_UTILISATEUR = :id");

    // Création des variables liées
    query.bindValue(":nom",        nom);
    query.bindValue(":prenom",     prenom);
    query.bindValue(":email",      email);
    query.bindValue(":telephone",  telephone);
    query.bindValue(":motdepasse", motDePasse);
    query.bindValue(":role",       role);
    query.bindValue(":ville",      ville);
    query.bindValue(":codepostal", codePostal);
    query.bindValue(":sexe",       sexe);
    query.bindValue(":photo",      photo);
    query.bindValue(":id",         res);

    return query.exec(); // exec() envoie la requête pour l'exécuter
}

// ══════════════════════════════════════════════════════════════════════════════
//  supprimer(int id) — DELETE WHERE ID_UTILISATEUR = :id
// ══════════════════════════════════════════════════════════════════════════════
bool Utilisateur::supprimer(int id)
{
    QSqlQuery query;

    QString res = QString::number(id);

    query.prepare("DELETE FROM UTILISATEUR WHERE ID_UTILISATEUR = :id");

    query.bindValue(":id", res);

    return query.exec(); // exec() envoie la requête pour l'exécuter
}
