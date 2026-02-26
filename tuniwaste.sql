-- ============================================================
-- TUNIWASTE - Base de Données (VERSION FINALE CORRIGÉE)
-- ============================================================

-- ============================================================
-- 1. TABLE UTILISATEUR
-- ============================================================
CREATE TABLE UTILISATEUR (
    ID_utilisateur  NUMBER          PRIMARY KEY,
    nom             VARCHAR2(50)    NOT NULL,
    prenom          VARCHAR2(50)    NOT NULL,
    email           VARCHAR2(100)   UNIQUE NOT NULL,
    num_telephone   VARCHAR2(20),
    mot_de_passe    VARCHAR2(255)   NOT NULL,
    role            VARCHAR2(30)    NOT NULL,
    ville           VARCHAR2(50),
    code_postal     VARCHAR2(10),
    sexe            VARCHAR2(10),
    photo_url       VARCHAR2(500)   -- Photo PNG de l'utilisateur
);

-- ============================================================
-- 2. TABLE ZONE
--    gérer : un utilisateur gère plusieurs zones (1,N)
-- ============================================================
CREATE TABLE ZONE (
    id_zone         NUMBER          PRIMARY KEY,
    population      NUMBER,
    nom_zone        VARCHAR2(100)   NOT NULL,
    localisation    VARCHAR2(255),
    surface_zone    NUMBER(10,2),
    frequence       NUMBER,
    ID_utilisateur  NUMBER          NOT NULL,
    CONSTRAINT fk_zone_utilisateur
        FOREIGN KEY (ID_utilisateur) REFERENCES UTILISATEUR(ID_utilisateur)
);

-- ============================================================
-- 3. TABLE CAMION
--    gérer : un utilisateur gère plusieurs camions (1,N)
-- ============================================================
CREATE TABLE CAMION (
    id_camion       NUMBER          PRIMARY KEY,
    type_camion     VARCHAR2(50),
    capacite        NUMBER(10,2),
    statut          VARCHAR2(30),
    chauffeur       VARCHAR2(100),
    num_chauffeur   VARCHAR2(20),
    ID_utilisateur  NUMBER          NOT NULL,
    CONSTRAINT fk_camion_utilisateur
        FOREIGN KEY (ID_utilisateur) REFERENCES UTILISATEUR(ID_utilisateur)
);

-- ============================================================
-- 4. TABLE RECYCLAGE
--    recevoir : une poubelle reçoit 0 ou 1 centre recyclage
--    FK id_poubelle ajoutée après POUBELLE via ALTER TABLE
-- ============================================================
CREATE TABLE RECYCLAGE (
    id_centre       NUMBER          PRIMARY KEY,
    responsable     VARCHAR2(100),
    type_dechet     VARCHAR2(50),
    quantite        NUMBER(10,2),
    date_recyclage  DATE,
    id_poubelle     NUMBER          -- FK vers POUBELLE (ajoutée après)
);

-- ============================================================
-- 5. TABLE POUBELLE
--    affecter  : une zone affecte plusieurs poubelles  → id_zone FK
--    gérer     : un utilisateur gère les poubelles     → id_utilisateur FK
--    recevoir  : liée à un centre recyclage            → id_centre FK
--    collecter : collectée par au max 1 camion (0,1)   → id_camion FK ici directement
-- ============================================================
CREATE TABLE POUBELLE (
    id_poubelle     NUMBER          PRIMARY KEY,
    localisation    VARCHAR2(255),
    niveau          NUMBER(5,2),
    etat            VARCHAR2(30),
    type            VARCHAR2(50),
    id_zone         NUMBER          NOT NULL,
    id_centre       NUMBER,
    id_utilisateur  NUMBER,
    id_camion       NUMBER,                     -- collecter (0,1) : au max 1 camion
    CONSTRAINT fk_poubelle_zone
        FOREIGN KEY (id_zone)           REFERENCES ZONE(id_zone),
    CONSTRAINT fk_poubelle_centre
        FOREIGN KEY (id_centre)         REFERENCES RECYCLAGE(id_centre),
    CONSTRAINT fk_poubelle_utilisateur
        FOREIGN KEY (id_utilisateur)    REFERENCES UTILISATEUR(ID_utilisateur),
    CONSTRAINT fk_poubelle_camion
        FOREIGN KEY (id_camion)         REFERENCES CAMION(id_camion)
);

-- ============================================================
-- 6. Résolution dépendance circulaire RECYCLAGE <-> POUBELLE
-- ============================================================
ALTER TABLE RECYCLAGE
    ADD CONSTRAINT fk_recyclage_poubelle
        FOREIGN KEY (id_poubelle) REFERENCES POUBELLE(id_poubelle);

-- ============================================================
-- FIN DU SCRIPT
-- ============================================================