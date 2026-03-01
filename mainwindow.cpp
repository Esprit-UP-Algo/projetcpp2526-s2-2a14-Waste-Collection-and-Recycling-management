#include "mainwindow.h"
#include "database.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QFrame>
#include <QMessageBox>
#include <QFileDialog>
#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QFont>
#include <QMap>
#include <QtMath>
#include <QCoreApplication>
#include <QPixmap>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSizePolicy>
#include <QScrollBar>

// ════════════════════════════════════════════════
//  DonutChart
// ════════════════════════════════════════════════
DonutChart::DonutChart(QWidget *parent) : QWidget(parent) {}
void DonutChart::setSlices(const QVector<Slice> &s) { m_slices = s; update(); }
void DonutChart::setCenterText(const QString &t)    { m_center = t;  update(); }

void DonutChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    int w = width(), h = height();
    int sz = qMin(w, h) - 10;
    QRectF rect((w - sz) / 2.0, (h - sz) / 2.0, sz, sz);
    double total = 0;
    for (auto &s : m_slices) total += s.value;
    if (total == 0) total = 1;
    double angle = -90 * 16;
    int thick = sz / 5;
    for (auto &s : m_slices) {
        double span = (s.value / total) * 360 * 16;
        p.setPen(Qt::NoPen);
        p.setBrush(s.color);
        p.drawPie(rect, (int)angle, (int)span);
        angle += span;
    }
    QRectF inner = rect.adjusted(thick, thick, -thick, -thick);
    p.setBrush(Qt::white);
    p.setPen(Qt::NoPen);
    p.drawEllipse(inner);
    if (!m_center.isEmpty()) {
        QFont f = p.font(); f.setPointSize(sz / 6); f.setBold(true);
        p.setFont(f); p.setPen(QColor(51, 51, 51));
        p.drawText(inner, Qt::AlignCenter, m_center);
    }
}

// ════════════════════════════════════════════════
//  Constructeur
// ════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , nextId(1), editRow(-1), currentDbId(-1)
{
    if (!Database::instance().estOuverte()) {
        QMessageBox::critical(this, "Erreur", "Base de donnees non connectee.");
        return;
    }
    creerTables();
    buildUI();
    applyStyles();
    chargerZones();
    loadData();
}

MainWindow::~MainWindow()
{
    Database::instance().fermer();
}

// ════════════════════════════════════════════════
//  buildUI
// ════════════════════════════════════════════════
void MainWindow::buildUI()
{
    setWindowTitle("TuniWaste - Gestion des Camions");
    resize(1280, 780);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // ══ SIDEBAR ══════════════════════════════════
    QWidget *sidebar = new QWidget();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(220);
    QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    // Logo
    QWidget *logoArea = new QWidget();
    logoArea->setObjectName("logoArea");
    logoArea->setFixedHeight(90);
    QHBoxLayout *logoLayout = new QHBoxLayout(logoArea);
    logoLayout->setContentsMargins(16, 12, 16, 12);
    logoLayout->setSpacing(12);

    logoIcon = new QLabel();
    logoIcon->setFixedSize(80, 80);
    logoIcon->setObjectName("logoIcon");
    logoIcon->setAlignment(Qt::AlignCenter);

    // Logo depuis le fichier PNG du projet (supporte la transparence)
    QString logoPath = QCoreApplication::applicationDirPath() + "/../logo.png";
    if (!QFile::exists(logoPath))
        logoPath = "C:/Users/user/Desktop/tuniwaste2/logo.png";
    QPixmap px(logoPath);
    if (!px.isNull()) {
        logoIcon->setPixmap(px.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoIcon->setStyleSheet("background:transparent;");
    } else {
        logoIcon->setStyleSheet("background:#81C784; border-radius:24px;");
    }
    QLabel *logoText = new QLabel("TuniWaste");
    logoText->setObjectName("logoText");
    logoLayout->addWidget(logoIcon);
    logoLayout->addWidget(logoText);
    sideLayout->addWidget(logoArea);

    QFrame *sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("background:rgba(255,255,255,0.2); max-height:1px; border:none;");
    sideLayout->addWidget(sep);

    // Menu
    QStringList menuLabels = {
        "Tableau de bord",
        "Gestion des utilisateurs",
        "Gestion des camions",
        "Gestion des zones",
        "Gestion des poubelles",
        "Gestion de recyclage",
        "Rapports",
        "Parametres"
    };
    for (int i = 0; i < menuLabels.size(); ++i) {
        QPushButton *btn = new QPushButton(menuLabels[i]);
        btn->setObjectName(QString("menuBtn%1").arg(i));
        btn->setFixedHeight(48);
        btn->setCursor(Qt::PointingHandCursor);
        if (i == 2) btn->setProperty("active", "true");
        connect(btn, &QPushButton::clicked, [this, i](){ onMenuClicked(i); });
        menuBtns.append(btn);
        sideLayout->addWidget(btn);
    }
    sideLayout->addStretch();

    QFrame *sep2 = new QFrame();
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("background:rgba(255,255,255,0.2); max-height:1px; border:none;");
    sideLayout->addWidget(sep2);

    // User
    QWidget *userRow = new QWidget();
    userRow->setObjectName("userRow");
    userRow->setFixedHeight(68);
    QHBoxLayout *userLayout = new QHBoxLayout(userRow);
    userLayout->setContentsMargins(16, 12, 16, 12);
    userLayout->setSpacing(12);

    // Avatar avec initiale
    QLabel *avatar = new QLabel("A");
    avatar->setObjectName("avatarLabel");
    avatar->setFixedSize(40, 40);
    avatar->setAlignment(Qt::AlignCenter);

    // Colonne nom + role
    QWidget *userInfo = new QWidget();
    userInfo->setStyleSheet("background:transparent;");
    QVBoxLayout *userInfoL = new QVBoxLayout(userInfo);
    userInfoL->setContentsMargins(0, 0, 0, 0);
    userInfoL->setSpacing(2);
    QLabel *userLabel = new QLabel("Admin");
    userLabel->setObjectName("userLabel");
    QLabel *userRole = new QLabel("Administrateur");
    userRole->setObjectName("userRole");
    userInfoL->addWidget(userLabel);
    userInfoL->addWidget(userRole);

    userLayout->addWidget(avatar);
    userLayout->addWidget(userInfo, 1);
    sideLayout->addWidget(userRow);

    rootLayout->addWidget(sidebar);

    // ══ CONTENU ══════════════════════════════════
    QWidget *contentArea = new QWidget();
    contentArea->setObjectName("contentArea");
    QHBoxLayout *contentLayout = new QHBoxLayout(contentArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    rootLayout->addWidget(contentArea, 1);

    // Formulaire
    QWidget *formPanel = new QWidget();
    formPanel->setObjectName("formPanel");

    QScrollArea *formScroll = new QScrollArea();
    formScroll->setWidget(formPanel);
    formScroll->setWidgetResizable(true);
    formScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    formScroll->setFixedWidth(285);
    formScroll->setStyleSheet("QScrollArea{border:none;background:white;}");

    QVBoxLayout *formLayout = new QVBoxLayout(formPanel);
    formLayout->setContentsMargins(20, 24, 20, 24);
    formLayout->setSpacing(7);

    formTitleLbl = new QLabel("Ajouter un camion");
    formTitleLbl->setObjectName("formTitleLbl");
    formLayout->addWidget(formTitleLbl);
    formLayout->addSpacing(6);

    auto mkLbl = [](const QString &t) {
        QLabel *l = new QLabel(t);
        l->setObjectName("fieldLabel");
        return l;
    };

    auto mkInput = [](QLineEdit *&ptr, const QString &name, const QString &ph) {
        ptr = new QLineEdit();
        ptr->setObjectName(name);
        ptr->setPlaceholderText(ph);
        ptr->setFixedHeight(36);
    };

    mkLbl("ID Camion"); formLayout->addWidget(mkLbl("ID Camion"));
    mkInput(idCamionInput, "idCamionInput", "Ex: CAM-001");
    formLayout->addWidget(idCamionInput);

    formLayout->addWidget(mkLbl("Localisation"));
    mkInput(locInput, "locInput", "Ex: Tunis Centre");
    formLayout->addWidget(locInput);

    formLayout->addWidget(mkLbl("Type de dechet"));
    mkInput(typeDechetInput, "typeDechetInput", "");
    typeDechetInput->setText("Plastique");
    formLayout->addWidget(typeDechetInput);

    formLayout->addWidget(mkLbl("Capacite (L)"));
    capacityInput = new QSpinBox();
    capacityInput->setObjectName("capacityInput");
    capacityInput->setRange(0, 99999);
    capacityInput->setValue(240);
    capacityInput->setFixedHeight(36);
    formLayout->addWidget(capacityInput);

    formLayout->addWidget(mkLbl("Niveau de remplissage (%)"));
    fillInput = new QSpinBox();
    fillInput->setObjectName("fillInput");
    fillInput->setRange(0, 100);
    fillInput->setValue(0);
    fillInput->setFixedHeight(36);
    formLayout->addWidget(fillInput);

    formLayout->addWidget(mkLbl("Etat"));
    mkInput(stateInput, "stateInput", "");
    stateInput->setText("Operationnelle");
    formLayout->addWidget(stateInput);

    formLayout->addWidget(mkLbl("Type de camion"));
    typeCamionCombo = new QComboBox();
    typeCamionCombo->setObjectName("typeCamionCombo");
    typeCamionCombo->setFixedHeight(36);
    typeCamionCombo->addItems({"Benne", "Compacteur", "Plateau", "Citerne", "Remorque", "Autre"});
    formLayout->addWidget(typeCamionCombo);

    formLayout->addWidget(mkLbl("Zone"));
    zoneCombo = new QComboBox();
    zoneCombo->setObjectName("zoneCombo");
    zoneCombo->setFixedHeight(36);
    zoneCombo->addItem("-- Choisir une zone --", -1);
    formLayout->addWidget(zoneCombo);

    formLayout->addWidget(mkLbl("Chauffeur"));
    mkInput(driverInput, "driverInput", "Nom du chauffeur");
    formLayout->addWidget(driverInput);

    formLayout->addWidget(mkLbl("Telephone"));
    mkInput(phoneInput, "phoneInput", "Ex: +216 XX XXX XXX");
    formLayout->addWidget(phoneInput);

    formLayout->addSpacing(10);

    saveBtn = new QPushButton("Enregistrer");
    saveBtn->setObjectName("saveBtn");
    saveBtn->setFixedHeight(44);
    saveBtn->setCursor(Qt::PointingHandCursor);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSave);
    formLayout->addWidget(saveBtn);

    QPushButton *cancelBtn = new QPushButton("Annuler");
    cancelBtn->setObjectName("cancelBtn");
    cancelBtn->setFixedHeight(36);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    connect(cancelBtn, &QPushButton::clicked, this, &MainWindow::clearForm);
    formLayout->addWidget(cancelBtn);

    formLayout->addSpacing(10);
    QLabel *tipBox = new QLabel("Remplissez tous les champs obligatoires avant d'enregistrer.");
    tipBox->setObjectName("tipBox");
    tipBox->setWordWrap(true);
    formLayout->addWidget(tipBox);
    formLayout->addStretch();

    contentLayout->addWidget(formScroll);

    // Partie droite
    QWidget *rightArea = new QWidget();
    rightArea->setObjectName("rightArea");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightArea);
    rightLayout->setContentsMargins(24, 20, 24, 20);
    rightLayout->setSpacing(14);
    contentLayout->addWidget(rightArea, 1);

    QLabel *breadcrumb = new QLabel("Tableau de bord  /  Gestion des Camions");
    breadcrumb->setObjectName("breadcrumb");
    rightLayout->addWidget(breadcrumb);

    QHBoxLayout *titleRow = new QHBoxLayout();
    QLabel *listTitle = new QLabel("Liste des Camions");
    listTitle->setObjectName("listTitle");
    exportBtn = new QPushButton("Exporter HTML");
    exportBtn->setObjectName("exportBtn");
    exportBtn->setFixedHeight(38);
    exportBtn->setCursor(Qt::PointingHandCursor);
    connect(exportBtn, &QPushButton::clicked, this, &MainWindow::onExportPDF);
    titleRow->addWidget(listTitle);
    titleRow->addStretch();
    titleRow->addWidget(exportBtn);
    rightLayout->addLayout(titleRow);

    QHBoxLayout *searchRow = new QHBoxLayout();
    searchBar = new QLineEdit();
    searchBar->setObjectName("searchBar");
    searchBar->setPlaceholderText("Rechercher un camion...");
    searchBar->setFixedHeight(38);
    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::onSearchChanged);

    stateFilter = new QComboBox();
    stateFilter->setObjectName("stateFilter");
    stateFilter->setFixedHeight(38);
    stateFilter->addItems({"Tous les etats", "Operationnelle", "En panne", "Maintenance", "Pleine", "Vide"});
    connect(stateFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onStateFilterChanged);

    typeFilter = new QComboBox();
    typeFilter->setObjectName("typeFilter");
    typeFilter->setFixedHeight(38);
    typeFilter->addItems({"Tous les types", "Plastique", "Organique", "Verre", "Metal", "Papier"});
    connect(typeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTypeFilterChanged);

    searchRow->addWidget(searchBar, 2);
    searchRow->addWidget(stateFilter, 1);
    searchRow->addWidget(typeFilter, 1);
    rightLayout->addLayout(searchRow);

    tableWidget = new QTableWidget(0, 12);
    tableWidget->setObjectName("tableWidget");
    tableWidget->setHorizontalHeaderLabels({"#","ID Camion","Type Camion","Capacite","Niveau","Etat","Type Dechet","Zone","Chauffeur","Telephone","Localisation","Actions"});
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setShowGrid(false);
    tableWidget->setColumnWidth(0,35);  tableWidget->setColumnWidth(1,90);
    tableWidget->setColumnWidth(2,100); tableWidget->setColumnWidth(3,80);
    tableWidget->setColumnWidth(4,70);  tableWidget->setColumnWidth(5,110);
    tableWidget->setColumnWidth(6,95);  tableWidget->setColumnWidth(7,90);
    tableWidget->setColumnWidth(8,115); tableWidget->setColumnWidth(9,115);
    tableWidget->setColumnHidden(10, true); // Localisation cachée mais gardée pour fillForm
    rightLayout->addWidget(tableWidget, 1);

    QLabel *statsTitle = new QLabel("Statistiques en temps reel");
    statsTitle->setObjectName("statsTitle");
    rightLayout->addWidget(statsTitle);

    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(16);

    auto mkStatBox = [](const QString &objName, const QString &title) -> QWidget* {
        QWidget *box = new QWidget();
        box->setObjectName(objName);
        return box;
    };

    QWidget *statBox1 = mkStatBox("statBox1","");
    QVBoxLayout *sb1 = new QVBoxLayout(statBox1);
    sb1->setContentsMargins(14,14,14,14);
    QLabel *sb1T = new QLabel("Repartition par Etat"); sb1T->setObjectName("statBoxTitle");
    stateChart = new DonutChart(); stateChart->setFixedSize(140,140);
    stateLegend = new QLabel(); stateLegend->setObjectName("stateLegend"); stateLegend->setWordWrap(true);
    sb1->addWidget(sb1T); sb1->addWidget(stateChart,0,Qt::AlignCenter); sb1->addWidget(stateLegend);
    statsRow->addWidget(statBox1);

    QWidget *statBox2 = mkStatBox("statBox2","");
    QVBoxLayout *sb2 = new QVBoxLayout(statBox2);
    sb2->setContentsMargins(14,14,14,14);
    QLabel *sb2T = new QLabel("Repartition par Type"); sb2T->setObjectName("statBoxTitle");
    typeChart = new DonutChart(); typeChart->setFixedSize(140,140);
    typeLegend = new QLabel(); typeLegend->setObjectName("typeLegend"); typeLegend->setWordWrap(true);
    sb2->addWidget(sb2T); sb2->addWidget(typeChart,0,Qt::AlignCenter); sb2->addWidget(typeLegend);
    statsRow->addWidget(statBox2);

    QWidget *statBox3 = mkStatBox("statBox3","");
    QVBoxLayout *sb3 = new QVBoxLayout(statBox3);
    sb3->setContentsMargins(14,14,14,14);
    QLabel *sb3T = new QLabel("Niveau Moyen de Remplissage"); sb3T->setObjectName("statBoxTitle");
    levelChart = new DonutChart(); levelChart->setFixedSize(140,140);
    levelSubLabel = new QLabel("Taux de remplissage moyen"); levelSubLabel->setObjectName("levelSubLabel"); levelSubLabel->setWordWrap(true);
    sb3->addWidget(sb3T); sb3->addWidget(levelChart,0,Qt::AlignCenter); sb3->addWidget(levelSubLabel);
    statsRow->addWidget(statBox3);

    statsRow->addStretch();
    rightLayout->addLayout(statsRow);
}

// ════════════════════════════════════════════════
//  creerTables
// ════════════════════════════════════════════════
bool MainWindow::creerTables()
{
    QSqlQuery check(Database::instance().bd());
    check.exec("SELECT table_name FROM user_tables WHERE table_name = 'CAMIONS'");
    if (!check.next()) {
        QSqlQuery q(Database::instance().bd());
        bool ok = q.exec(R"(
            CREATE TABLE CAMIONS (
                ID           NUMBER        PRIMARY KEY,
                ID_CAMION    VARCHAR2(50)  NOT NULL,
                TYPE_CAMION  VARCHAR2(50),
                LOCALISATION VARCHAR2(200),
                CAPACITE     VARCHAR2(50),
                NIVEAU       VARCHAR2(50),
                ETAT         VARCHAR2(50),
                TYPE         VARCHAR2(50),
                ID_ZONE      VARCHAR2(50),
                CHAUFFEUR    VARCHAR2(100),
                TELEPHONE    VARCHAR2(50),
                DATE_CREATION DATE DEFAULT SYSDATE
            )
        )");
        if (!ok) { qDebug() << "[DB] Erreur creation table:" << q.lastError().text(); return false; }

        // Crée la séquence seulement si elle n'existe pas encore
        QSqlQuery seqCheck(Database::instance().bd());
        seqCheck.exec("SELECT sequence_name FROM user_sequences WHERE sequence_name = 'CAMIONS_SEQ'");
        if (!seqCheck.next()) {
            q.exec("CREATE SEQUENCE camions_seq START WITH 1 INCREMENT BY 1 NOCACHE NOCYCLE");
        }
    } else {
        // Table existe déjà — ajouter TYPE_CAMION si elle manque
        QSqlQuery colCheck(Database::instance().bd());
        colCheck.exec("SELECT column_name FROM user_tab_columns WHERE table_name='CAMIONS' AND column_name='TYPE_CAMION'");
        if (!colCheck.next()) {
            QSqlQuery alter(Database::instance().bd());
            alter.exec("ALTER TABLE CAMIONS ADD (TYPE_CAMION VARCHAR2(50))");
        }
    }
    return true;
}

// ════════════════════════════════════════════════
//  chargerZones — Popule le ComboBox Zone
// ════════════════════════════════════════════════
void MainWindow::chargerZones()
{
    zoneCombo->clear();
    zoneCombo->addItem("-- Choisir une zone --", -1);
    auto zones = Database::instance().chargerZones();
    if (zones.isEmpty()) {
        // Récupère l'erreur brute pour aider au diagnostic
        QSqlQuery diagQ(Database::instance().bd());
        diagQ.exec("SELECT COUNT(*) FROM ZONE");
        QString erreur = diagQ.lastError().text();
        if (erreur.isEmpty()) {
            diagQ.exec("SELECT COUNT(*) FROM TUNIWASTE.ZONE");
            erreur = diagQ.lastError().text();
        }
        QMessageBox::warning(this, "Zones non chargees",
            "Impossible de charger les zones depuis la base de donnees.\n\n"
            "Erreur Oracle : " + (erreur.isEmpty() ? "(aucune erreur SQL, table peut-etre vide)" : erreur) + "\n\n"
            "Verifiez que la table ZONE est accessible avec l'utilisateur connecte.");
    } else {
        for (auto &z : zones)
            zoneCombo->addItem(z.second, z.first); // affiche NOM_ZONE, stocke ID_ZONE
    }
}

void MainWindow::loadDataFromDB()
{
    QSqlQuery q = Database::instance().tousLesCamions();
    tableWidget->setRowCount(0);
    int maxId = 0;
    while (q.next()) {
        int id = q.value("ID").toInt();
        if (id > maxId) maxId = id;
        addRow(id,
               q.value("ID_CAMION").toString(),
               q.value("TYPE_CAMION").toString(),
               q.value("LOCALISATION").toString(),
               q.value("CAPACITE").toString(),
               q.value("NIVEAU").toString(),
               q.value("ETAT").toString(),
               q.value("TYPE").toString(),
               q.value("ID_ZONE").toString(),
               q.value("CHAUFFEUR").toString(),
               q.value("TELEPHONE").toString());
    }
    nextId = maxId + 1;
    updateCharts();
}

bool MainWindow::saveToDB(const QString &idCamion, const QString &typeCamion,
                          const QString &loc,
                          const QString &cap,     const QString &fill,
                          const QString &state,   const QString &typeDechet,
                          const QString &idZone,  const QString &driver,
                          const QString &phone)
{
    return Database::instance().insertCamion(idCamion, typeCamion, loc, cap, fill, state, typeDechet, idZone, driver, phone);
}

bool MainWindow::updateInDB(int id,
                            const QString &idCamion, const QString &typeCamion,
                            const QString &loc,
                            const QString &cap,     const QString &fill,
                            const QString &state,   const QString &typeDechet,
                            const QString &idZone,  const QString &driver,
                            const QString &phone)
{
    return Database::instance().updateCamion(id, idCamion, typeCamion, loc, cap, fill, state, typeDechet, idZone, driver, phone);
}

bool MainWindow::deleteFromDB(int id)
{
    return Database::instance().deleteCamion(id);
}

void MainWindow::loadData() { loadDataFromDB(); }

void MainWindow::addRow(int id,
                        const QString &idCamion, const QString &typeCamion,
                        const QString &loc,      const QString &cap,
                        const QString &fill,     const QString &state,
                        const QString &typeDechet, const QString &idZone,
                        const QString &driver,   const QString &phone)
{
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);
    auto mkItem = [](const QString &t) {
        auto *it = new QTableWidgetItem(t);
        it->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        return it;
    };
    // Col: 0=#, 1=ID_CAMION, 2=TYPE_CAMION, 3=CAPACITE, 4=NIVEAU,
    //      5=ETAT(badge), 6=TYPE_DECHET, 7=ZONE(NOM), 8=CHAUFFEUR,
    //      9=TELEPHONE, 10=LOCALISATION(caché), 11=ACTIONS
    tableWidget->setItem(row, 0,  mkItem(QString::number(id)));
    tableWidget->setItem(row, 1,  mkItem(idCamion));
    tableWidget->setItem(row, 2,  mkItem(typeCamion));
    tableWidget->setItem(row, 3,  mkItem(cap));
    tableWidget->setItem(row, 4,  mkItem(fill));
    // Zone : afficher le nom si ID_ZONE est numérique, sinon afficher tel quel
    QString zoneName = idZone;
    bool ok;
    int zId = idZone.toInt(&ok);
    if (ok) {
        for (int i = 1; i < zoneCombo->count(); ++i) {
            if (zoneCombo->itemData(i).toInt() == zId) {
                zoneName = zoneCombo->itemText(i);
                break;
            }
        }
    }
    tableWidget->setItem(row, 6,  mkItem(typeDechet));
    tableWidget->setItem(row, 7,  mkItem(zoneName));
    tableWidget->setItem(row, 8,  mkItem(driver));
    tableWidget->setItem(row, 9,  mkItem(phone));
    tableWidget->setItem(row, 10, mkItem(loc)); // localisation cachée

    QLabel *badge = new QLabel(state);
    badge->setAlignment(Qt::AlignCenter);
    badge->setStyleSheet(stateStyle(state));
    tableWidget->setCellWidget(row, 5, badge);

    QWidget *actW = new QWidget();
    actW->setStyleSheet("background:white;");
    QHBoxLayout *al = new QHBoxLayout(actW);
    al->setContentsMargins(6,4,6,4); al->setSpacing(6);
    QPushButton *modBtn = new QPushButton("Modifier");
    modBtn->setFixedHeight(28); modBtn->setCursor(Qt::PointingHandCursor);
    modBtn->setStyleSheet("QPushButton{background:#4CAF50;color:white;border:none;border-radius:4px;font-size:11px;font-weight:bold;padding:0 8px;}QPushButton:hover{background:#388E3C;}");
    connect(modBtn, &QPushButton::clicked, [this, row](){ onModify(row); });
    QPushButton *delBtn = new QPushButton("Supprimer");
    delBtn->setFixedHeight(28); delBtn->setCursor(Qt::PointingHandCursor);
    delBtn->setStyleSheet("QPushButton{background:#F44336;color:white;border:none;border-radius:4px;font-size:11px;font-weight:bold;padding:0 8px;}QPushButton:hover{background:#C62828;}");
    connect(delBtn, &QPushButton::clicked, [this, row](){ onDelete(row); });
    al->addWidget(modBtn); al->addWidget(delBtn); al->addStretch();
    tableWidget->setCellWidget(row, 11, actW);
    tableWidget->setRowHeight(row, 50);
}

void MainWindow::updateCharts()
{
    QMap<QString, int> stateCount, typeCount;
    double totalFill = 0;
    int rows = tableWidget->rowCount();
    for (int r = 0; r < rows; ++r) {
        QLabel *bl = qobject_cast<QLabel*>(tableWidget->cellWidget(r, 5));
        if (bl) stateCount[bl->text()]++;
        if (tableWidget->item(r, 6)) typeCount[tableWidget->item(r, 6)->text()]++;
        if (tableWidget->item(r, 4)) {
            QString f = tableWidget->item(r, 4)->text();
            f.remove("%"); totalFill += f.trimmed().toDouble();
        }
    }
    QStringList stColors = {"#FFA726","#EF5350","#FFEE58","#66BB6A","#42A5F5"};
    QVector<DonutChart::Slice> stSlices; int ci = 0; QString stLegend;
    for (auto it = stateCount.begin(); it != stateCount.end(); ++it, ++ci) {
        stSlices.append({it.key(), (double)it.value(), QColor(stColors[ci % stColors.size()])});
        stLegend += QString("- %1: %2\n").arg(it.key()).arg(it.value());
    }
    stateChart->setSlices(stSlices);
    stateChart->setCenterText(QString::number(rows));
    stateLegend->setText(stLegend.trimmed());

    QStringList tpColors = {"#FFA726","#66BB6A","#42A5F5","#AB47BC","#EF5350"};
    QVector<DonutChart::Slice> tpSlices; ci = 0; QString tpLegend;
    for (auto it = typeCount.begin(); it != typeCount.end(); ++it, ++ci) {
        tpSlices.append({it.key(), (double)it.value(), QColor(tpColors[ci % tpColors.size()])});
        tpLegend += QString("- %1: %2\n").arg(it.key()).arg(it.value());
    }
    typeChart->setSlices(tpSlices);
    typeChart->setCenterText(QString::number(rows));
    typeLegend->setText(tpLegend.trimmed());

    double avg = rows > 0 ? totalFill / rows : 0;
    QVector<DonutChart::Slice> lvSlices = {
        {"Rempli", avg, QColor("#66BB6A")},
        {"Vide", 100 - avg, QColor("#E0E0E0")}
    };
    levelChart->setSlices(lvSlices);
    levelChart->setCenterText(QString::number((int)avg) + "%");
}

QString MainWindow::stateStyle(const QString &s)
{
    if (s == "Operationnelle" || s == "Vide")
        return "color:#2E7D32;font-size:13px;font-weight:bold;";
    if (s == "Pleine" || s == "En panne")
        return "color:#C62828;font-size:13px;font-weight:bold;";
    if (s == "Moyen" || s == "Maintenance")
        return "color:#E65100;font-size:13px;font-weight:bold;";
    return "color:#424242;font-size:13px;";
}

void MainWindow::clearForm()
{
    editRow = -1; currentDbId = -1;
    idCamionInput->clear(); locInput->clear();
    typeDechetInput->setText("Plastique");
    capacityInput->setValue(240); fillInput->setValue(0);
    stateInput->setText("Operationnelle");
    typeCamionCombo->setCurrentIndex(0);
    zoneCombo->setCurrentIndex(0);
    driverInput->clear(); phoneInput->clear();
    formTitleLbl->setText("Ajouter un camion");
    saveBtn->setText("Enregistrer");
}

void MainWindow::fillForm(int row)
{
    editRow = row;
    currentDbId = tableWidget->item(row, 0)->text().toInt();
    idCamionInput->setText(tableWidget->item(row, 1) ? tableWidget->item(row, 1)->text() : "");
    // TYPE_CAMION (col 2)
    QString tc = tableWidget->item(row, 2) ? tableWidget->item(row, 2)->text() : "";
    int tci = typeCamionCombo->findText(tc);
    typeCamionCombo->setCurrentIndex(tci >= 0 ? tci : 0);
    // CAPACITE (col 3)
    QString c = tableWidget->item(row, 3) ? tableWidget->item(row, 3)->text() : "240";
    c.remove(" L"); capacityInput->setValue(c.trimmed().toInt());
    // NIVEAU (col 4)
    QString f = tableWidget->item(row, 4) ? tableWidget->item(row, 4)->text() : "0";
    f.remove("%"); fillInput->setValue(f.trimmed().toInt());
    // ETAT badge (col 5)
    QLabel *bl = qobject_cast<QLabel*>(tableWidget->cellWidget(row, 5));
    stateInput->setText(bl ? bl->text() : "");
    // TYPE_DECHET (col 6)
    typeDechetInput->setText(tableWidget->item(row, 6) ? tableWidget->item(row, 6)->text() : "");
    // ZONE (col 7) — retrouver l'ID_ZONE stocké pour sélectionner dans le combo
    // On stocke le nom dans col 7; on retrouve le bon index par rematch du nom
    QString zoneName = tableWidget->item(row, 7) ? tableWidget->item(row, 7)->text() : "";
    int zi = zoneCombo->findText(zoneName);
    zoneCombo->setCurrentIndex(zi >= 0 ? zi : 0);
    // CHAUFFEUR (col 8)
    driverInput->setText(tableWidget->item(row, 8) ? tableWidget->item(row, 8)->text() : "");
    // TELEPHONE (col 9)
    phoneInput->setText(tableWidget->item(row, 9) ? tableWidget->item(row, 9)->text() : "");
    // LOCALISATION (col 10 caché)
    locInput->setText(tableWidget->item(row, 10) ? tableWidget->item(row, 10)->text() : "");
    formTitleLbl->setText("Modifier camion #" + tableWidget->item(row, 0)->text());
    saveBtn->setText("Mettre a jour");
}

// ════════════════════════════════════════════════
//  applyStyles — CSS UNIQUE SANS DOUBLONS
// ════════════════════════════════════════════════
void MainWindow::applyStyles()
{
    setStyleSheet(R"(

QMainWindow, QWidget {
    font-family: 'Segoe UI', Arial, sans-serif;
    background: #F5F5F5;
    color: #212121;
    font-size: 13px;
    font-weight: normal;
}

#sidebar {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #43A047, stop:1 #2E7D32);
}

#logoArea { background: transparent; }

#logoText {
    color: white;
    font-size: 20px;
    font-weight: bold;
    background: transparent;
}
QLabel { color:#212121; font-size:13px; font-weight:bold; }

#menuBtn0,#menuBtn1,#menuBtn2,#menuBtn3,
#menuBtn4,#menuBtn5,#menuBtn6,#menuBtn7 {
    background: transparent;
    color: rgba(255,255,255,0.90);
    border: none;
    text-align: left;
    padding-left: 20px;
    font-size: 13px;
    border-left: 4px solid transparent;
    font-weight: normal;
}
#menuBtn0:hover,#menuBtn1:hover,#menuBtn2:hover,#menuBtn3:hover,
#menuBtn4:hover,#menuBtn5:hover,#menuBtn6:hover,#menuBtn7:hover {
    background: rgba(255,255,255,0.15);
    color: white;
    border-left: 4px solid rgba(255,255,255,0.4);
}
#menuBtn0[active="true"],#menuBtn1[active="true"],#menuBtn2[active="true"],
#menuBtn3[active="true"],#menuBtn4[active="true"],#menuBtn5[active="true"],
#menuBtn6[active="true"],#menuBtn7[active="true"] {
    background: rgba(255,255,255,0.18);
    color: white;
    border-left: 4px solid white;
    font-weight: bold;
}

#userRow { background: rgba(0,0,0,0.22); }
#userLabel { color: white; font-size: 14px; font-weight: bold; background: transparent; }
#userRole  { color: rgba(255,255,255,0.65); font-size: 11px; font-weight: normal; background: transparent; }
#avatarLabel {
    background: #66BB6A;
    border-radius: 20px;
    color: white;
    font-weight: bold;
    font-size: 16px;
}

#formPanel { background: white; }

#formTitleLbl {
    color: #1B5E20;
    font-size: 16px;
    font-weight: bold;
}

#fieldLabel {
    color: #424242;
    font-size: 12px;
    font-weight: bold;
}

#idCamionInput,#locInput,#typeDechetInput,
#stateInput,#zoneInput,#driverInput,#phoneInput {
    background: white;
    border: 1px solid #BDBDBD;
    border-radius: 6px;
    padding: 5px 10px;
    font-size: 13px;
    color: #212121;
}
#idCamionInput:focus,#locInput:focus,#typeDechetInput:focus,
#stateInput:focus,#zoneInput:focus,#driverInput:focus,#phoneInput:focus {
    border-color: #4CAF50;
}

QSpinBox {
    background: white;
    border: 1px solid #BDBDBD;
    border-radius: 6px;
    padding: 4px 8px;
    font-size: 13px;
    color: #212121;
}
QSpinBox:focus { border-color: #4CAF50; }

#saveBtn {
    background: #4CAF50;
    color: white;
    border: none;
    border-radius: 8px;
    font-size: 14px;
    font-weight: bold;
}
#saveBtn:hover { background: #388E3C; }

#cancelBtn {
    background: #EEEEEE;
    color: #424242;
    border: none;
    border-radius: 6px;
    font-size: 13px;
}
#cancelBtn:hover { background: #BDBDBD; }

#tipBox {
    background: #E3F2FD;
    color: #1565C0;
    border-left: 4px solid #2196F3;
    border-radius: 6px;
    padding: 10px 12px;
    font-size: 12px;
}

#contentArea, #rightArea { background: #F5F5F5; }

#breadcrumb { color: #9E9E9E; font-size: 12px; }

#listTitle {
    color: #212121;
    font-size: 20px;
    font-weight: bold;
}

#exportBtn {
    background: #FF9800;
    color: white;
    border: none;
    border-radius: 8px;
    font-size: 13px;
    font-weight: bold;
    padding: 0 16px;
}
#exportBtn:hover { background: #F57C00; }

#searchBar {
    background: white;
    border: 1px solid #BDBDBD;
    border-radius: 8px;
    padding: 0 14px;
    font-size: 13px;
    color: #212121;
}
#searchBar:focus { border-color: #4CAF50; }

QComboBox {
    background: white;
    color: #212121;
    border: 1px solid #BDBDBD;
    border-radius: 8px;
    padding: 0 10px;
    font-size: 13px;
}
QComboBox:focus { border-color: #4CAF50; }
QComboBox::drop-down { border: none; width: 24px; }
QComboBox QAbstractItemView {
    background: white;
    color: #212121;
    selection-background-color: #E8F5E9;
    selection-color: #1B5E20;
}

#tableWidget {
    background: white;
    border: none;
    font-size: 13px;
    color: #212121;
}
#tableWidget::item {
    padding: 6px 8px;
    border-bottom: 1px solid #F5F5F5;
    background: white;
}
#tableWidget::item:selected { background: #E8F5E9; color: #1B5E20; }
#tableWidget::item:hover    { background: #F9FBE7; }

QHeaderView::section {
    background: white;
    color: #757575;
    font-weight: 700;
    font-size: 11px;
    letter-spacing: 0.8px;
    padding: 10px;
    border: none;
    border-bottom: 2px solid #EEEEEE;
    text-transform: uppercase;
}

#statsTitle {
    color: #212121;
    font-size: 17px;
    font-weight: bold;
}

#statBox1,#statBox2,#statBox3 {
    background: white;
    border-radius: 12px;
    border: 1px solid #E0E0E0;
    min-width: 200px;
}

#statBoxTitle {
    color: #212121;
    font-size: 13px;
    font-weight: bold;
}

#stateLegend, #typeLegend, #levelSubLabel {
    color: #616161;
    font-size: 12px;
}

QScrollBar:vertical {
    background: #F5F5F5;
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: #BDBDBD;
    border-radius: 4px;
    min-height: 24px;
}
QScrollBar::handle:vertical:hover { background: #4CAF50; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }

)");
}

// ════════════════════════════════════════════════
//  SLOTS
// ════════════════════════════════════════════════
void MainWindow::onMenuClicked(int index)
{
    for (int i = 0; i < menuBtns.size(); ++i) {
        menuBtns[i]->setProperty("active", i == index ? "true" : "false");
        menuBtns[i]->style()->unpolish(menuBtns[i]);
        menuBtns[i]->style()->polish(menuBtns[i]);
    }
}

void MainWindow::onSave()
{
    QString idCam      = idCamionInput->text().trimmed();
    QString loc        = locInput->text().trimmed();
    QString typeDechet = typeDechetInput->text().trimmed();
    QString state      = stateInput->text().trimmed();
    QString typeCam    = typeCamionCombo->currentText();
    QString idZone     = zoneCombo->currentData().toString();  // valeur = ID_ZONE
    QString driver     = driverInput->text().trimmed();
    QString phone      = phoneInput->text().trimmed();

    if (idCam.isEmpty() || loc.isEmpty() || typeDechet.isEmpty() || driver.isEmpty() || phone.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants",
                             "Remplissez : ID Camion, Localisation, Type dechet, Chauffeur et Telephone.");
        return;
    }
    if (zoneCombo->currentData().toInt() == -1) {
        QMessageBox::warning(this, "Zone manquante", "Veuillez choisir une zone.");
        return;
    }
    QString cap  = QString::number(capacityInput->value()) + " L";
    QString fill = QString::number(fillInput->value()) + "%";

    if (editRow >= 0) {
        if (updateInDB(currentDbId, idCam, typeCam, loc, cap, fill, state, typeDechet, idZone, driver, phone)) {
            tableWidget->item(editRow, 1)->setText(idCam);
            tableWidget->item(editRow, 2)->setText(typeCam);
            tableWidget->item(editRow, 3)->setText(cap);
            tableWidget->item(editRow, 4)->setText(fill);
            tableWidget->item(editRow, 6)->setText(typeDechet);
            tableWidget->item(editRow, 7)->setText(zoneCombo->currentText());
            tableWidget->item(editRow, 8)->setText(driver);
            tableWidget->item(editRow, 9)->setText(phone);
            tableWidget->item(editRow, 10)->setText(loc);
            QLabel *bl = new QLabel(state);
            bl->setAlignment(Qt::AlignCenter);
            bl->setStyleSheet(stateStyle(state));
            tableWidget->setCellWidget(editRow, 5, bl);
            QMessageBox::information(this, "Succes", "Camion mis a jour !");
        } else {
            QMessageBox::critical(this, "Erreur", "Echec de la mise a jour.");
        }
    } else {
        if (saveToDB(idCam, typeCam, loc, cap, fill, state, typeDechet, idZone, driver, phone)) {
            loadDataFromDB();
            QMessageBox::information(this, "Succes", "Camion ajoute !");
        } else {
            QMessageBox::critical(this, "Erreur", "Echec de l'insertion.");
        }
    }
    clearForm();
    updateCharts();
}

void MainWindow::onModify(int row) { fillForm(row); }

void MainWindow::onDelete(int row)
{
    if (row < 0 || row >= tableWidget->rowCount()) return;
    int id = tableWidget->item(row, 0)->text().toInt();
    auto r = QMessageBox::question(this, "Supprimer",
                                   "Supprimer le camion #" + QString::number(id) + " ?",
                                   QMessageBox::Yes | QMessageBox::No);
    if (r == QMessageBox::Yes) {
        if (deleteFromDB(id)) {
            if (editRow == row) clearForm();
            tableWidget->removeRow(row);
            updateCharts();
            QMessageBox::information(this, "Succes", "Camion supprime !");
        } else {
            QMessageBox::critical(this, "Erreur", "Echec de la suppression.");
        }
    }
}

void MainWindow::onSearchChanged(const QString &txt)
{
    for (int r = 0; r < tableWidget->rowCount(); ++r) {
        bool found = false;
        for (int c = 0; c < 12; ++c) {
            auto *it = tableWidget->item(r, c);
            if (it && it->text().contains(txt, Qt::CaseInsensitive)) { found = true; break; }
        }
        tableWidget->setRowHidden(r, !found);
    }
}

void MainWindow::onStateFilterChanged(int index)
{
    for (int r = 0; r < tableWidget->rowCount(); ++r) tableWidget->setRowHidden(r, false);
    if (index == 0) return;
    QString sel = stateFilter->currentText();
    for (int r = 0; r < tableWidget->rowCount(); ++r) {
        QLabel *bl = qobject_cast<QLabel*>(tableWidget->cellWidget(r, 5));
        tableWidget->setRowHidden(r, !bl || bl->text() != sel);
    }
}

void MainWindow::onTypeFilterChanged(int index)
{
    for (int r = 0; r < tableWidget->rowCount(); ++r) tableWidget->setRowHidden(r, false);
    if (index == 0) return;
    QString sel = typeFilter->currentText();
    for (int r = 0; r < tableWidget->rowCount(); ++r) {
        auto *it = tableWidget->item(r, 6); // col 6 = Type Dechet
        tableWidget->setRowHidden(r, !it || it->text() != sel);
    }
}

void MainWindow::onExportPDF()
{
    QString fn = QFileDialog::getSaveFileName(this, "Exporter",
                                              QDir::homePath() + "/TuniWaste_" + QDate::currentDate().toString("yyyy-MM-dd") + ".html",
                                              "HTML (*.html)");
    if (fn.isEmpty()) return;
    if (!fn.endsWith(".html", Qt::CaseInsensitive)) fn += ".html";
    QFile f(fn);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de creer le fichier."); return;
    }
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    int total = tableWidget->rowCount(); double fillSum = 0;
    for (int r = 0; r < total; ++r) {
        auto *it = tableWidget->item(r, 3);
        if (it) { QString s = it->text(); s.remove("%"); fillSum += s.trimmed().toDouble(); }
    }
    int avg = total > 0 ? (int)(fillSum / total) : 0;
    out << "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>TuniWaste</title><style>"
        << "body{font-family:Segoe UI,sans-serif;margin:32px;background:#F1F8E9;}"
        << ".box{background:white;max-width:1200px;margin:auto;padding:32px;border-radius:12px;}"
        << "h1{color:#2E7D32;text-align:center;}"
        << "table{width:100%;border-collapse:collapse;margin-top:16px;}"
        << "th{background:#388E3C;color:white;padding:10px;text-align:left;}"
        << "td{padding:9px;border-bottom:1px solid #E8F5E9;}"
        << "</style></head><body><div class='box'>"
        << "<h1>TuniWaste - Rapport Camions</h1>"
        << "<p style='text-align:center;color:#9E9E9E;'>Total: " << total << " | Niveau moyen: " << avg << "%</p>"
        << "<table><thead><tr><th>#</th><th>ID</th><th>Capacite</th><th>Niveau</th>"
        << "<th>Etat</th><th>Type</th><th>Zone</th><th>Chauffeur</th><th>Tel</th></tr></thead><tbody>";
    for (int r = 0; r < total; ++r) {
        out << "<tr>";
        for (int c : {0,1,2,3}) out << "<td>" << (tableWidget->item(r,c) ? tableWidget->item(r,c)->text() : "") << "</td>";
        QLabel *bl = qobject_cast<QLabel*>(tableWidget->cellWidget(r, 4));
        out << "<td>" << (bl ? bl->text() : "") << "</td>";
        for (int c : {5,6,7,8}) out << "<td>" << (tableWidget->item(r,c) ? tableWidget->item(r,c)->text() : "") << "</td>";
        out << "</tr>";
    }
    out << "</tbody></table></div></body></html>";
    f.close();
    auto rep = QMessageBox::question(this, "Exporte", "Ouvrir le fichier ?\n" + fn, QMessageBox::Yes | QMessageBox::No);
    if (rep == QMessageBox::Yes) QDesktopServices::openUrl(QUrl::fromLocalFile(fn));
}
