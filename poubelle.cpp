#include "poubelle.h"

// --- AJOUTER ---
bool Poubelle::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO POUBELLE (ID_POUBELLE, LOCALISATION, NIVEAU, ETAT, TYPE, ID_ZONE) "
                  "VALUES (SEQ_POUBELLE.NEXTVAL, :loc, :niv, :etat, :type, :zone)");
    
    query.bindValue(":loc", localisation);
    query.bindValue(":niv", niveau);
    query.bindValue(":etat", etat);
    query.bindValue(":type", type);
    query.bindValue(":zone", id_zone);

    return query.exec();
}

// --- AFFICHER ---
QSqlQueryModel* Poubelle::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT LOCALISATION, NIVEAU, ETAT, TYPE, ID_ZONE FROM POUBELLE");
    
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("LOCALISATION"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NIVEAU"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("ETAT"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("TYPE"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("ZONE"));
    
    return model;
}

// --- SUPPRIMER ---
bool Poubelle::supprimer(const QString &loc)
{
    QSqlQuery query;
    query.prepare("DELETE FROM POUBELLE WHERE LOCALISATION = :loc");
    query.bindValue(":loc", loc);
    return query.exec();
}

// --- MODIFIER ---
bool Poubelle::modifier(const QString &ancienneLocalisation)
{
    QSqlQuery query;
    query.prepare("UPDATE POUBELLE SET LOCALISATION=:loc, NIVEAU=:niv, ETAT=:et, TYPE=:ty, ID_ZONE=:zo "
                  "WHERE LOCALISATION=:anc");
    
    query.bindValue(":loc", localisation);
    query.bindValue(":niv", niveau);
    query.bindValue(":et", etat);
    query.bindValue(":ty", type);
    query.bindValue(":zo", id_zone);
    query.bindValue(":anc", ancienneLocalisation);
    
    return query.exec();
}
