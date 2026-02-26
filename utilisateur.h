#ifndef UTILISATEUR_H
#define UTILISATEUR_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>
#include "database.h"

// ══════════════════════════════════════════════════════════════════════════════
//  Classe Utilisateur — Modèle (Architecture Modèle-Vue)
//  Table Oracle : TUNIWASTE.UTILISATEUR
//  Colonnes : ID_UTILISATEUR, NOM, PRENOM, EMAIL, NUM_TELEPHONE,
//             MOT_DE_PASSE, ROLE, VILLE, CODE_POSTAL, SEXE, PHOTO
// ══════════════════════════════════════════════════════════════════════════════
class Utilisateur
{
public:
    Utilisateur() {}
    Utilisateur(int id, const QString& nom, const QString& prenom,
                const QString& email, const QString& telephone,
                const QString& motDePasse, const QString& role,
                const QString& ville, const QString& codePostal,
                const QString& sexe, const QString& photo);

    // CRUD — toutes les requêtes SQL sont dans utilisateur.cpp, jamais dans les slots UI
    bool            ajouter();
    QSqlQueryModel* afficher();
    bool            modifier();
    bool            supprimer(int id);

    // Getters
    int     getId()          const { return id; }
    QString getNom()         const { return nom; }
    QString getPrenom()      const { return prenom; }
    QString getEmail()       const { return email; }
    QString getTelephone()   const { return telephone; }
    QString getMotDePasse()  const { return motDePasse; }
    QString getRole()        const { return role; }
    QString getVille()       const { return ville; }
    QString getCodePostal()  const { return codePostal; }
    QString getSexe()        const { return sexe; }
    QString getPhoto()       const { return photo; }

    // Setters
    void setId(int v)                   { id = v; }
    void setNom(const QString& v)       { nom = v; }
    void setPrenom(const QString& v)    { prenom = v; }
    void setEmail(const QString& v)     { email = v; }
    void setTelephone(const QString& v) { telephone = v; }
    void setMotDePasse(const QString& v){ motDePasse = v; }
    void setRole(const QString& v)      { role = v; }
    void setVille(const QString& v)     { ville = v; }
    void setCodePostal(const QString& v){ codePostal = v; }
    void setSexe(const QString& v)      { sexe = v; }
    void setPhoto(const QString& v)     { photo = v; }

private:
    int     id         = 0;
    QString nom;
    QString prenom;
    QString email;
    QString telephone;
    QString motDePasse;
    QString role;
    QString ville;
    QString codePostal;
    QString sexe;
    QString photo;
};

#endif // UTILISATEUR_H
