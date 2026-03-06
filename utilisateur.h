#ifndef UTILISATEUR_H
#define UTILISATEUR_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>
#include "connection.h"

class Utilisateur
{
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
    int id;

public:
    // Constructeurs
    Utilisateur(){}
    Utilisateur(int id, QString nom, QString prenom,
                QString email, QString telephone,
                QString motDePasse, QString role,
                QString ville, QString codePostal,
                QString sexe, QString photo);

    // Getters
    int     getId()         { return id; }
    QString getNom()        { return nom; }
    QString getPrenom()     { return prenom; }
    QString getEmail()      { return email; }
    QString getTelephone()  { return telephone; }
    QString getMotDePasse() { return motDePasse; }
    QString getRole()       { return role; }
    QString getVille()      { return ville; }
    QString getCodePostal() { return codePostal; }
    QString getSexe()       { return sexe; }
    QString getPhoto()      { return photo; }

    // Setters
    void setId(int v)              { id = v; }
    void setNom(QString v)         { nom = v; }
    void setPrenom(QString v)      { prenom = v; }
    void setEmail(QString v)       { email = v; }
    void setTelephone(QString v)   { telephone = v; }
    void setMotDePasse(QString v)  { motDePasse = v; }
    void setRole(QString v)        { role = v; }
    void setVille(QString v)       { ville = v; }
    void setCodePostal(QString v)  { codePostal = v; }
    void setSexe(QString v)        { sexe = v; }
    void setPhoto(QString v)       { photo = v; }

    // Fonctionnalités de Base relatives à l'entité Utilisateur
    bool            ajouter();
    QSqlQueryModel* afficher();
    bool            modifier();
    bool            supprimer(int id);
};

#endif // UTILISATEUR_H
