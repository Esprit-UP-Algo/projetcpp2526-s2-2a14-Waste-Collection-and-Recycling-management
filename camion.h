#ifndef CAMION_H
#define CAMION_H

#include <QString>
#include <QList>

// ══════════════════════════════════════════════════════════════
//  Structure Camion — représente un enregistrement de la table
// ══════════════════════════════════════════════════════════════
struct CamionData {
    int     id;
    QString idCamion;
    QString localisation;
    QString capacite;
    QString niveau;
    QString etat;
    QString type;
    QString idZone;
    QString chauffeur;
    QString telephone;
};

// ══════════════════════════════════════════════════════════════
//  Classe Camion — Patron Singleton
//  Gère toutes les opérations CRUD sur la table CAMIONS
// ══════════════════════════════════════════════════════════════
class Camion
{
public:
    // ── Singleton ─────────────────────────────────────────────
    static Camion& instance();

    // ── CREATE ────────────────────────────────────────────────
    bool ajouter(const QString &idCamion,
                 const QString &localisation,
                 const QString &capacite,
                 const QString &niveau,
                 const QString &etat,
                 const QString &type,
                 const QString &idZone,
                 const QString &chauffeur,
                 const QString &telephone);

    // ── READ ──────────────────────────────────────────────────
    QList<CamionData> tousLesCamions();
    CamionData        trouverParId(int id);
    QList<CamionData> rechercherParEtat(const QString &etat);
    QList<CamionData> rechercherParType(const QString &type);

    // ── UPDATE ────────────────────────────────────────────────
    bool modifier(int id,
                  const QString &idCamion,
                  const QString &localisation,
                  const QString &capacite,
                  const QString &niveau,
                  const QString &etat,
                  const QString &type,
                  const QString &idZone,
                  const QString &chauffeur,
                  const QString &telephone);

    // ── DELETE ────────────────────────────────────────────────
    bool supprimer(int id);

private:
    // Constructeur privé → empêche l'instanciation directe
    Camion() = default;

    // Copie et affectation interdites (règles du Singleton)
    Camion(const Camion&)            = delete;
    Camion& operator=(const Camion&) = delete;

    // Méthode utilitaire interne
    CamionData lireLigne(int id,
                         const QString &idCamion,
                         const QString &loc,
                         const QString &cap,
                         const QString &niv,
                         const QString &etat,
                         const QString &type,
                         const QString &zone,
                         const QString &chauffeur,
                         const QString &tel);
};

#endif // CAMION_H
