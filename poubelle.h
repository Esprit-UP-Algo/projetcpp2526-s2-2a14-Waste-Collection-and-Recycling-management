#ifndef POUBELLE_H
#define POUBELLE_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>

// ============================================
// CLASSE MODÈLE - Poubelle (pure C++)
// Toutes les requêtes SQL sont ici
// ============================================
class Poubelle
{
private:
    int     id_poubelle;
    QString localisation;
    int     niveau;
    QString etat;
    QString type;
    int     id_zone;

public:
    // ---- Constructeurs ----
    Poubelle() : id_poubelle(0), niveau(0), id_zone(0) {}

    Poubelle(int id_poubelle, const QString &localisation, int niveau,
             const QString &etat, const QString &type, int id_zone)
        : id_poubelle(id_poubelle), localisation(localisation),
        niveau(niveau), etat(etat), type(type), id_zone(id_zone) {}

    // ---- Getters ----
    int     getId()           const { return id_poubelle; }
    QString getLocalisation() const { return localisation; }
    int     getNiveau()       const { return niveau; }
    QString getEtat()         const { return etat; }
    QString getType()         const { return type; }
    int     getIdZone()       const { return id_zone; }

    // ---- Setters ----
    void setId(int id)                      { this->id_poubelle  = id; }
    void setLocalisation(const QString &l)  { this->localisation = l; }
    void setNiveau(int n)                   { this->niveau       = n; }
    void setEtat(const QString &e)          { this->etat         = e; }
    void setType(const QString &t)          { this->type         = t; }
    void setIdZone(int z)                   { this->id_zone      = z; }

    // ---- Fonctionnalités CRUD ----
    bool            ajouter();                      // CREATE
    QSqlQueryModel* afficher();                     // READ
    bool            modifier(const QString &ancienneLocalisation); // UPDATE
    bool            supprimer(const QString &localisation);        // DELETE

    // ---- Fonctionnalités avancées ----
    QSqlQueryModel* rechercherParLocalisation(const QString &keyword);
    QSqlQueryModel* filtrerParEtat(const QString &etat);
    double          calculerNiveauMoyen();
};

#endif // POUBELLE_H
