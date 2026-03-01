#include "gestion_des_poubelles.h"
#include "ui_gestion_des_poubelles.h"
#include "poubelle.h"           // ← MODÈLE : classe purement C++
#include <QSqlRecord>          // ← nécessaire pour model->record()
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QHeaderView>
#include <QMessageBox>
#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QMap>
#include <QGroupBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QDateTime>
#include <QProgressBar>
#include <QFrame>
#include <QPrinter>
#include <QPageSize>
#include <QPageLayout>
#include <QTextDocument>
#include <QPrintDialog>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QIntValidator>


// ============================================
// DONUT CHART WIDGET
// ============================================
DonutChart::DonutChart(const QString &title, QWidget *parent)
    : QWidget(parent), title(title), total(0)
{
    setMinimumSize(250, 300);
    setStyleSheet("background-color: white; border: 1px solid #ddd; border-radius: 8px;");

    colors["Moyen"]          = QColor(255, 152, 0);
    colors["Pleine"]         = QColor(244, 67, 54);
    colors["Vide"]           = QColor(255, 193, 7);
    colors["Opérationnelle"] = QColor(76, 175, 80);
    colors["Maintenance"]    = QColor(33, 150, 243);
    colors["Hors service"]   = QColor(244, 67, 54);

    colors["Plastique"]  = QColor(255, 152, 0);
    colors["Papier"]     = QColor(255, 193, 7);
    colors["Verre"]      = QColor(33, 150, 243);
    colors["Organique"]  = QColor(76, 175, 80);
    colors["Autre"]      = QColor(156, 39, 176);
}

void DonutChart::setData(const QMap<QString, int> &newData)
{
    data  = newData;
    total = 0;
    for (auto count : data) total += count;
    update();
}

void DonutChart::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();

    QFont titleFont;
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(Qt::black);
    painter.drawText(10, 20, w - 20, 25, Qt::AlignLeft, title);

    if (total == 0) {
        painter.setFont(QFont("Arial", 10));
        painter.drawText(QRect(0, 100, w, 50), Qt::AlignCenter, "Aucune donnée");
        return;
    }

    int   centerX     = w / 2;
    int   centerY     = 90;
    int   outerRadius = 45;
    int   innerRadius = 30;
    float startAngle  = 0;

    QStringList order;
    if (title == "Types de Poubelles")
        order << "Plastique" << "Papier" << "Verre" << "Organique" << "Autre";
    else if (title == "État des Poubelles")
        order << "Moyen" << "Pleine" << "Vide";

    for (const QString &key : order) {
        if (!data.contains(key) || data[key] == 0) continue;

        int   count = data[key];
        float angle = (count * 360.0f) / total;
        QColor color = colors.value(key, QColor(128, 128, 128));

        painter.setBrush(QBrush(color));
        painter.setPen(Qt::NoPen);

        QPainterPath path;
        path.moveTo(centerX, centerY);
        path.arcTo(centerX - outerRadius, centerY - outerRadius,
                   2 * outerRadius, 2 * outerRadius, startAngle, angle);
        path.arcTo(centerX - innerRadius, centerY - innerRadius,
                   2 * innerRadius, 2 * innerRadius, startAngle + angle, -angle);
        path.closeSubpath();
        painter.drawPath(path);

        startAngle += angle;
    }

    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.setPen(Qt::black);
    painter.drawText(centerX - 30, centerY - 10, 60, 20, Qt::AlignCenter, QString::number(total));

    painter.setFont(QFont("Arial", 9));

    QVector<QPair<QString, int>> sortedData;
    for (const QString &key : order) {
        if (data.contains(key) && data[key] > 0)
            sortedData.append(qMakePair(key, data[key]));
    }

    int legendY = 160;
    for (const auto &item : sortedData) {
        QString key   = item.first;
        int     count = item.second;
        int     legendX = 20;

        painter.fillRect(legendX, legendY - 6, 8, 8, colors.value(key));
        painter.setPen(Qt::black);
        painter.drawRect(legendX, legendY - 6, 8, 8);
        painter.drawText(legendX + 12, legendY, key + ": " + QString::number(count));

        legendY += 18;
    }
}

// ============================================
// MAIN WINDOW
// ============================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::gestion_des_poubelles), currentEditingRow(-1), nextId(1)
{
    ui->setupUi(this);
    setWindowTitle("TuniWaste - Gestion des poubelles");
    resize(1600, 950);
    setMinimumSize(1400, 850);

    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width()  - 1600) / 2;
    int y = (screenGeometry.height() - 950)  / 2;
    move(x, y);

    setupUI();
    applyStyles();
    loadDataFromDB();   // Charger depuis Oracle via le modèle
    loadZones();        // Charger les zones dans le select
    updateCharts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ============================================
// CHARGEMENT DEPUIS ORACLE  (VUE → appelle le MODÈLE)
// ============================================
void MainWindow::loadDataFromDB()
{
    poubelleTable->setRowCount(0);

    Poubelle p;
    QSqlQueryModel *model = p.afficher();

    // --- FORCE LE CHARGEMENT COMPLET ---
    // Oracle peut avoir un délai, on force Qt à récupérer toutes les lignes
    while (model->canFetchMore()) {
        model->fetchMore();
    }

    if (model->lastError().isValid()) {
        qDebug() << "Erreur lors du chargement initial :" << model->lastError().text();
        delete model;
        return;
    }

    // On remplit le tableau
    for (int row = 0; row < model->rowCount(); ++row) {
        QString localisation = model->data(model->index(row, 0)).toString();
        int     niveau       = model->data(model->index(row, 1)).toInt();
        QString etat         = model->data(model->index(row, 2)).toString();
        QString type         = model->data(model->index(row, 3)).toString();
        QString idZone       = model->data(model->index(row, 4)).toString();

        addTableRow(localisation, niveau, etat, type, idZone);
    }

    delete model;
    updateCharts();
}

// ============================================
// SETUP UI
// ============================================
void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    createSidebar();
    createMainContent();

    mainLayout->addWidget(sidebar, 0);
    mainLayout->addWidget(mainContent, 1);
}

// ============================================
// SIDEBAR
// ============================================
void MainWindow::createSidebar()
{
    sidebar = new QWidget();
    sidebar->setFixedWidth(280);
    sidebar->setObjectName("sidebar");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setSpacing(0);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *logoWidget = new QWidget();
    QHBoxLayout *logoLayout = new QHBoxLayout(logoWidget);
    logoLayout->setContentsMargins(10, 15, 10, 15);
    logoLayout->setSpacing(10);
    logoLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *logoLabel = new QLabel();
    logoLabel->setFixedSize(100, 100);
    logoLabel->setScaledContents(false);

    // --- CHARGEMENT DU LOGO (Version Robuste) ---
    QStringList pathsToTry;
    QString appDir = QCoreApplication::applicationDirPath();
    pathsToTry << appDir + "/logo.png";                      // Même dossier que l'EXE
    pathsToTry << QDir(appDir).absoluteFilePath("../logo.png"); // Dossier build
    pathsToTry << QDir(appDir).absoluteFilePath("../../logo.png"); // Dossier source (projet)
    pathsToTry << "c:/Users/eyae8/Documents/tuniwaste/logo.png"; // Chemin absolu direct

    QPixmap logoPixmap;
    for (const QString &path : pathsToTry) {
        if (logoPixmap.load(path)) {
            qDebug() << "Logo chargé avec succès depuis :" << path;
            break;
        }
    }

    if (!logoPixmap.isNull()) {
        QPixmap scaledLogo = logoPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        logoLabel->setPixmap(scaledLogo);
        logoLabel->setAlignment(Qt::AlignCenter);
    } else {
        qDebug() << "ERREUR : Impossible de trouver logo.png";
        logoLabel->setStyleSheet("background: #9BCB4E; border-radius: 50px;");
    }

    QLabel *titleLabel = new QLabel("TuniWaste");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setObjectName("sidebarTitle");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    logoLayout->addWidget(logoLabel,  0, Qt::AlignVCenter);
    logoLayout->addWidget(titleLabel, 0, Qt::AlignVCenter);
    logoLayout->addStretch();

    sidebarLayout->addWidget(logoWidget);

    QStringList menuItems = {
        "Tableau de bord",
        "Gestion des utilisateurs",
        "Gestion des camions",
        "Gestion des zones",
        "Gestion des poubelles",
        "Gestion de recyclage",
        "Rapports",
        "Paramètres"
    };

    for (int i = 0; i < menuItems.size(); ++i) {
        QPushButton *btn = new QPushButton(menuItems[i]);
        btn->setObjectName("menuItem");
        btn->setFixedHeight(50);
        btn->setCursor(Qt::PointingHandCursor);
        if (i == 4) btn->setProperty("active", true);
        sidebarLayout->addWidget(btn);
    }

    sidebarLayout->addStretch();

    QWidget     *userWidget = new QWidget();
    userWidget->setObjectName("userWidget");
    QHBoxLayout *userLayout = new QHBoxLayout(userWidget);
    userLayout->setContentsMargins(15, 15, 15, 15);

    userLabel = new QLabel("Utilisateur"); // Nom par défaut
    userLabel->setObjectName("userName");
    userLayout->addWidget(userLabel);
    userLayout->addStretch();

    sidebarLayout->addWidget(userWidget);
}

void MainWindow::setUserName(const QString &name)
{
    if (userLabel) {
        userLabel->setText(name);
    }
}

void MainWindow::loadZones()
{
    idZoneCombo->clear();
    QSqlQuery query("SELECT ID_ZONE FROM ZONE ORDER BY ID_ZONE");
    while (query.next()) {
        idZoneCombo->addItem(query.value(0).toString());
    }
    
    if (idZoneCombo->count() == 0) {
        idZoneCombo->addItem("Aucune zone");
    }
}

// ============================================
// MAIN CONTENT
// ============================================
void MainWindow::createMainContent()
{
    mainContent = new QWidget();
    mainContent->setObjectName("mainContent");
    mainContent->setStyleSheet("background-color: white;");

    QVBoxLayout *mainLayout = new QVBoxLayout(mainContent);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *breadcrumb = new QLabel("Tableau de bord / Gestion des poubelles");
    breadcrumb->setStyleSheet("color: #999; font-size: 12px;");
    headerLayout->addWidget(breadcrumb);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(15);

    createFormPanel();
    contentLayout->addWidget(formPanel, 0);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(10);

    createTablePanel();
    rightLayout->addWidget(tablePanel, 1);

    createStatsPanel();
    rightLayout->addWidget(statsPanel, 1);

    contentLayout->addLayout(rightLayout, 1);
    mainLayout->addLayout(contentLayout, 1);
}

// ============================================
// FORM PANEL
// ============================================
void MainWindow::createFormPanel()
{
    formPanel = new QWidget();
    formPanel->setFixedWidth(330);
    formPanel->setObjectName("formPanel");
    formPanel->setStyleSheet("background-color: #f9f9f9; border: 1px solid #e0e0e0; border-radius: 8px;");

    QVBoxLayout *layout = new QVBoxLayout(formPanel);
    layout->setSpacing(12);
    layout->setContentsMargins(25, 20, 25, 20);

    QLabel *formTitle = new QLabel("Ajouter une poubelle");
    formTitle->setObjectName("formTitle");
    QFont titleFont = formTitle->font();
    titleFont.setPointSize(13);
    titleFont.setBold(true);
    formTitle->setFont(titleFont);
    layout->addWidget(formTitle);

    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("background-color: #ddd;");
    layout->addWidget(separator);

    // Localisation
    QLabel *locLabel = new QLabel("Localisation");
    locLabel->setObjectName("formLabel");
    layout->addWidget(locLabel);
    localisationInput = new QLineEdit();
    localisationInput->setPlaceholderText("Ex: Rue de la République, Tunis");
    localisationInput->setObjectName("formInput");
    localisationInput->setMinimumHeight(40);
    
    // Contrôle de saisie en temps réel (Max 50 caractères)
    localisationInput->setMaxLength(50);
    layout->addWidget(localisationInput);

    // Type
    QLabel *typeLabel = new QLabel("Type de déchet");
    typeLabel->setObjectName("formLabel");
    layout->addWidget(typeLabel);
    typeCombo = new QComboBox();
    typeCombo->addItems({"Plastique", "Papier", "Verre", "Organique", "Autre"});
    typeCombo->setObjectName("formInput");
    typeCombo->setMinimumHeight(40);
    layout->addWidget(typeCombo);

    // Capacité
    QLabel *capLabel = new QLabel("Capacité");
    capLabel->setObjectName("formLabel");
    layout->addWidget(capLabel);
    capaciteInput = new QSpinBox();
    capaciteInput->setRange(10, 5000);
    capaciteInput->setValue(240);
    capaciteInput->setObjectName("formInput");
    capaciteInput->setMinimumHeight(40);
    layout->addWidget(capaciteInput);

    // Niveau
    QLabel *nivLabel = new QLabel("Niveau de remplissage");
    nivLabel->setObjectName("formLabel");
    layout->addWidget(nivLabel);
    niveauInput = new QSpinBox();
    niveauInput->setRange(0, 100);
    niveauInput->setObjectName("formInput");
    niveauInput->setMinimumHeight(40);
    layout->addWidget(niveauInput);

    // État
    QLabel *etatLabel = new QLabel("État");
    etatLabel->setObjectName("formLabel");
    layout->addWidget(etatLabel);
    etatCombo = new QComboBox();
    etatCombo->addItems({"Opérationnelle", "Pleine", "Maintenance", "Hors service"});
    etatCombo->setObjectName("formInput");
    etatCombo->setMinimumHeight(40);
    layout->addWidget(etatCombo);

    // ID Zone
    QLabel *zoneLabel = new QLabel("ID Zone (Sélectionnée)");
    zoneLabel->setObjectName("formLabel");
    layout->addWidget(zoneLabel);
    idZoneCombo = new QComboBox();
    idZoneCombo->setObjectName("formInput");
    idZoneCombo->setMinimumHeight(40);
    layout->addWidget(idZoneCombo);

    layout->addSpacing(20);

    enregistrerBtn = new QPushButton("Enregistrer");
    enregistrerBtn->setObjectName("btnEnregistrer");
    enregistrerBtn->setMinimumHeight(50);
    enregistrerBtn->setCursor(Qt::PointingHandCursor);
    connect(enregistrerBtn, &QPushButton::clicked, this, &MainWindow::onEnregistrerClicked);
    layout->addWidget(enregistrerBtn);

    QLabel *tipsLabel = new QLabel("💡 Astuce:\nDouble-cliquez ou utilisez 'Modifier' pour éditer une poubelle.");
    tipsLabel->setObjectName("tipsLabel");
    tipsLabel->setStyleSheet("background-color: #e3f2fd; padding: 10px; border-radius: 4px; font-size: 11px; color: #1976d2;");
    layout->addWidget(tipsLabel);

    layout->addStretch();
}

// ============================================
// TABLE PANEL
// ============================================
void MainWindow::createTablePanel()
{
    tablePanel = new QWidget();
    tablePanel->setObjectName("tablePanel");

    QVBoxLayout *layout = new QVBoxLayout(tablePanel);
    layout->setSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *tableTitle = new QLabel("Liste des poubelles");
    QFont titleFont = tableTitle->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    tableTitle->setFont(titleFont);
    titleLayout->addWidget(tableTitle);
    titleLayout->addStretch();

    exportPdfBtn = new QPushButton("Exporter PDF");
    exportPdfBtn->setObjectName("btnExport");
    exportPdfBtn->setMinimumHeight(40);
    exportPdfBtn->setMaximumWidth(150);
    connect(exportPdfBtn, &QPushButton::clicked, this, &MainWindow::onExporterPDFClicked);
    titleLayout->addWidget(exportPdfBtn);
    layout->addLayout(titleLayout);

    QHBoxLayout *filterLayout = new QHBoxLayout();

    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("🔍 Rechercher...");
    searchInput->setObjectName("searchInput");
    searchInput->setMaximumHeight(40);
    searchInput->setMinimumWidth(200);
    connect(searchInput, &QLineEdit::textChanged, this, &MainWindow::onSearchChanged);
    filterLayout->addWidget(searchInput);

    etatFilterCombo = new QComboBox();
    etatFilterCombo->addItems({"Tous les états", "Opérationnelle", "Pleine", "Maintenance"});
    etatFilterCombo->setObjectName("filterCombo");
    etatFilterCombo->setMaximumHeight(40);
    etatFilterCombo->setMaximumWidth(150);
    connect(etatFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onEtatFilterChanged);
    filterLayout->addWidget(etatFilterCombo);

    filterLayout->addStretch();
    layout->addLayout(filterLayout);

    poubelleTable = new QTableWidget();
    poubelleTable->setColumnCount(6);
    poubelleTable->setHorizontalHeaderLabels({"LOCALISATION","NIVEAU","ÉTAT","TYPE","ID ZONE","ACTIONS"});
    poubelleTable->setObjectName("poubelleTable");
    poubelleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    poubelleTable->setSelectionMode(QAbstractItemView::SingleSelection);
    poubelleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    poubelleTable->setShowGrid(false);

    QHeaderView *header = poubelleTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    poubelleTable->verticalHeader()->setDefaultSectionSize(60);

    connect(poubelleTable, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::onTableCellDoubleClicked);

    layout->addWidget(poubelleTable, 1);
}

// ============================================
// STATS PANEL
// ============================================
void MainWindow::createStatsPanel()
{
    statsPanel = new QWidget();
    statsPanel->setObjectName("statsPanel");
    statsPanel->setStyleSheet("background-color: white;");

    QVBoxLayout *layout = new QVBoxLayout(statsPanel);
    layout->setSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *statsTitle = new QLabel("Statistiques des Poubelles");
    QFont titleFont = statsTitle->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    statsTitle->setFont(titleFont);
    layout->addWidget(statsTitle);

    QHBoxLayout *chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(15);

    etatChart        = new DonutChart("État des Poubelles");
    typeChart        = new DonutChart("Types de Poubelles");
    niveauDonutChart = new NiveauDonutChart();

    chartsLayout->addWidget(etatChart,        1);
    chartsLayout->addWidget(typeChart,        1);
    chartsLayout->addWidget(niveauDonutChart, 1);

    layout->addLayout(chartsLayout, 1);
}

// ============================================
// ADD TABLE ROW
// ============================================
void MainWindow::addTableRow(const QString &localisation, int niveau,
                             const QString &etat, const QString &type, const QString &idZone)
{
    int row = poubelleTable->rowCount();
    poubelleTable->insertRow(row);

    auto makeItem = [](const QString &text) {
        QTableWidgetItem *item = new QTableWidgetItem(text);
        item->setForeground(Qt::black);
        item->setBackground(Qt::white);
        return item;
    };

    poubelleTable->setItem(row, 0, makeItem(localisation));
    poubelleTable->setItem(row, 1, makeItem(QString::number(niveau) + "%"));
    poubelleTable->setItem(row, 2, makeItem(etat));
    poubelleTable->setItem(row, 3, makeItem(type));
    poubelleTable->setItem(row, 4, makeItem(idZone));

    // --- Boutons d'actions ---
    QWidget     *actionsWidget = new QWidget();
    QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setContentsMargins(0, 5, 0, 5);
    actionsLayout->setSpacing(10);
    actionsLayout->addStretch();

    int capturedRow = row;

    QPushButton *modBtn = new QPushButton("Modifier");
    modBtn->setMaximumWidth(85);
    modBtn->setMinimumHeight(35);
    modBtn->setStyleSheet("background-color: #66BB6A; color: white; border: none;"
                          " border-radius: 4px; font-weight: bold;");
    connect(modBtn, &QPushButton::clicked, [this, capturedRow]() {
        // Remplir le formulaire uniquement
        currentEditingRow = capturedRow;
        localisationInput->setText(poubelleTable->item(capturedRow, 0)->text());
        niveauInput->setValue(poubelleTable->item(capturedRow, 1)->text().replace("%","").toInt());
        etatCombo->setCurrentText(poubelleTable->item(capturedRow, 2)->text());
        typeCombo->setCurrentText(poubelleTable->item(capturedRow, 3)->text());
        idZoneCombo->setCurrentText(poubelleTable->item(capturedRow, 4)->text());
        // Passer en mode édition : changer le bouton Enregistrer
        enregistrerBtn->setText("Enregistrer la modification");
        enregistrerBtn->setStyleSheet(
            "background-color: #1976D2; color: white; border: none;"
            " border-radius: 4px; font-weight: bold; font-size: 13px; padding: 10px;");
    });

    QPushButton *delBtn = new QPushButton("Supprimer");
    delBtn->setMaximumWidth(85);
    delBtn->setMinimumHeight(35);
    delBtn->setStyleSheet("background-color: #f44336; color: white; border: none;"
                          " border-radius: 4px; font-weight: bold;");
    connect(delBtn, &QPushButton::clicked, [this, capturedRow]() {
        currentEditingRow = capturedRow;
        onSupprimerClicked();
    });

    actionsLayout->addWidget(modBtn);
    actionsLayout->addWidget(delBtn);
    actionsLayout->addStretch();

    poubelleTable->setCellWidget(row, 5, actionsWidget);
}

// ============================================
// UPDATE CHARTS
// ============================================
void MainWindow::updateCharts()
{
    QMap<QString, int> stateCount;
    QMap<QString, int> typeCount;
    int totalNiveau = 0;
    int rows        = 0;

    stateCount["Moyen"]          = 0;
    stateCount["Pleine"]         = 0;
    stateCount["Vide"]           = 0;
    stateCount["Opérationnelle"] = 0;
    stateCount["Maintenance"]    = 0;
    stateCount["Hors service"]   = 0;

    typeCount["Papier"]    = 0;
    typeCount["Plastique"] = 0;
    typeCount["Verre"]     = 0;
    typeCount["Organique"] = 0;
    typeCount["Autre"]     = 0;

    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        QString etat = poubelleTable->item(row, 2)->text();
        if (stateCount.contains(etat)) stateCount[etat]++;

        QString type = poubelleTable->item(row, 3)->text();
        if (typeCount.contains(type)) typeCount[type]++;

        int niveau = poubelleTable->item(row, 1)->text().replace("%","").toInt();
        totalNiveau += niveau;
        rows++;
    }

    if (etatChart)        etatChart->setData(stateCount);
    if (typeChart)        typeChart->setData(typeCount);
    if (rows > 0 && niveauDonutChart)
        niveauDonutChart->setNiveau(totalNiveau / rows);
}

// ============================================
// SLOTS - VUE (appellent le MODÈLE, zéro requête SQL ici)
// ============================================

void MainWindow::onTableCellDoubleClicked(int row, int /*column*/)
{
    currentEditingRow = row;
    localisationInput->setText(poubelleTable->item(row, 0)->text());
    niveauInput->setValue(poubelleTable->item(row, 1)->text().replace("%","").toInt());
    etatCombo->setCurrentText(poubelleTable->item(row, 2)->text());
    typeCombo->setCurrentText(poubelleTable->item(row, 3)->text());
    idZoneCombo->setCurrentText(poubelleTable->item(row, 4)->text());
}

// ---- CREATE ou UPDATE selon currentEditingRow ----
void MainWindow::onEnregistrerClicked()
{
    // 1. Vérification des champs vides
    QString localisation = localisationInput->text().trimmed();
    QString idZoneStr = idZoneCombo->currentText();

    if (localisation.isEmpty()) {
        QMessageBox::warning(this, "Contrôle de saisie", "La localisation ne peut pas être vide !");
        localisationInput->setFocus();
        return;
    }

    if (idZoneStr == "Aucune zone" || idZoneStr.isEmpty()) {
        QMessageBox::warning(this, "Contrôle de saisie", "Veuillez sélectionner une Zone valide !");
        return;
    }

    int idZone = idZoneStr.toInt();

    // ── MODE MODIFICATION ──
    if (currentEditingRow != -1) {
        QString ancienneLocalisation = poubelleTable->item(currentEditingRow, 0)->text();

        Poubelle p(0,
                   localisation,
                   niveauInput->value(),
                   etatCombo->currentText(),
                   typeCombo->currentText(),
                   idZone);

        bool test = p.modifier(ancienneLocalisation);

        if (test) {
            loadDataFromDB();
            currentEditingRow = -1;
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la modification.");
        }

        // Remettre le bouton en mode ajout
        enregistrerBtn->setText("Enregistrer");
        enregistrerBtn->setStyleSheet(
            "background-color: #66BB6A; color: #000; border: none;"
            " border-radius: 4px; font-weight: bold; font-size: 14px; padding: 10px;");

        localisationInput->clear();
        // Optionnel: rafraîchir les zones au cas où
        loadZones();
        niveauInput->setValue(0);
        return;
    }

    Poubelle p(0,
               localisation,
               niveauInput->value(),
               etatCombo->currentText(),
               typeCombo->currentText(),
               idZone);

    bool test = p.ajouter();

    if (test) {
        QMessageBox::information(this, "Succès", "Poubelle ajoutée avec succès !");
        loadDataFromDB();
        localisationInput->clear();
        niveauInput->setValue(0);
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout. Vérifiez les données.");
    }
}

// ---- UPDATE (appelé via onEnregistrerClicked en mode édition) ----
void MainWindow::onModifierClicked()
{
    // La logique de modification est gérée dans onEnregistrerClicked()
    // quand currentEditingRow != -1
}

// ---- DELETE ----
void MainWindow::onSupprimerClicked()
{
    if (currentEditingRow == -1) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une ligne !");
        return;
    }

    QString localisation = poubelleTable->item(currentEditingRow, 0)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmation",
        "Supprimer la poubelle : " + localisation + " ?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) return;

    // Instancier le MODÈLE et appeler supprimer()
    Poubelle p;
    bool test = p.supprimer(localisation);

    if (test) {
        QMessageBox::information(this, "Succès", "Poubelle supprimée avec succès !");
        loadDataFromDB();   // Actualiser l'affichage
        currentEditingRow = -1;
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression.");
    }
}

// ---- EXPORTER PDF ----
void MainWindow::onExporterPDFClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "Documents PDF (*.pdf)");
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) fileName += ".pdf";

    QString htmlContent = R"(
        <html><head><meta charset='utf-8'>
        <style>
            body { font-family: Arial, sans-serif; margin: 20px; color: #333; }
            h1   { color: #66BB6A; text-align: center; border-bottom: 3px solid #66BB6A; padding-bottom: 10px; }
            table{ border-collapse: collapse; width: 100%; margin-top: 20px; }
            th   { background-color: #66BB6A; color: white; padding: 12px; border: 1px solid #ddd; }
            td   { padding: 10px; border: 1px solid #ddd; }
            tr:nth-child(even) { background-color: #f9f9f9; }
        </style></head><body>
        <h1>Rapport de Gestion des Poubelles</h1>
        <p style='text-align:right'>Date: )";

    htmlContent += QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm");
    htmlContent += R"(</p><table><thead><tr>
        <th>LOCALISATION</th><th>NIVEAU</th><th>ÉTAT</th><th>TYPE</th><th>ID ZONE</th>
    </tr></thead><tbody>)";

    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        htmlContent += "<tr>";
        for (int col = 0; col < 5; ++col)
            htmlContent += "<td>" + poubelleTable->item(row, col)->text() + "</td>";
        htmlContent += "</tr>";
    }

    htmlContent += "</tbody></table></body></html>";

    QTextDocument doc;
    doc.setHtml(htmlContent);

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    doc.print(&printer);

    QMessageBox::information(this, "Succès", "PDF exporté !\n" + fileName);
}

// ---- RECHERCHE (live search dans le tableau) ----
void MainWindow::onSearchChanged(const QString &text)
{
    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        bool match = text.isEmpty();
        for (int col = 0; col < 5 && !match; ++col) {
            if (poubelleTable->item(row, col)->text().contains(text, Qt::CaseInsensitive))
                match = true;
        }
        poubelleTable->setRowHidden(row, !match);
    }
}

// ---- FILTRE PAR ÉTAT ----
void MainWindow::onEtatFilterChanged(int index)
{
    if (index == 0) {
        for (int row = 0; row < poubelleTable->rowCount(); ++row)
            poubelleTable->setRowHidden(row, false);
        return;
    }

    QString selected = etatFilterCombo->currentText();
    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        QString etat = poubelleTable->item(row, 2)->text();
        poubelleTable->setRowHidden(row, etat != selected);
    }
}

// ============================================
// STYLES
// ============================================
void MainWindow::applyStyles()
{
    QString styleSheet = R"(
        QMainWindow  { background: #f5f5f5; }
        #sidebar     { background: #66BB6A; color: white; }
        #sidebarTitle{ color: white; }
        #menuItem    { background: transparent; color: white; border: none;
                       padding: 0 20px; text-align: left; font-size: 13px; }
        #menuItem:hover            { background: rgba(255,255,255,0.2); }
        #menuItem[active="true"]   { background: rgba(155,203,78,0.3);
                                     border-left: 4px solid #9BCB4E; }
        #userWidget  { border-top: 1px solid rgba(255,255,255,0.2);
                       background: rgba(0,0,0,0.1); }
        #userName    { color: white; font-weight: bold; }
        #mainContent { background: white; }
        #formPanel   { background: #f9f9f9; border: 1px solid #e0e0e0; }
        #formTitle   { color: #2c3e50; }
        #formLabel   { color: #333; font-weight: bold; font-size: 12px; }
        #formInput   { border: 1px solid #ddd; border-radius: 4px; padding: 8px;
                       background: white; color: #000; }
        #formInput:focus { border: 2px solid #66BB6A; }
        QComboBox    { border: 1px solid #ddd; border-radius: 4px; padding: 8px;
                       background: white; color: #000; }
        QComboBox QAbstractItemView { background-color: white; color: #000; }
        #btnEnregistrer { background-color: #66BB6A; color: #000; border: none;
                          border-radius: 4px; font-weight: bold; font-size: 14px; padding: 10px; }
        #btnEnregistrer:hover { background-color: #5AA55A; }
        #tipsLabel   { border-left: 4px solid #1976d2; }
        #btnExport   { background: #FF9800; color: white; border: none;
                       border-radius: 4px; font-weight: bold; }
        #searchInput, #filterCombo { border: 1px solid #ddd; border-radius: 4px;
                                     padding: 8px; background: white; }
        #poubelleTable { background: white; border: 1px solid #ddd; color: #000; }
        QHeaderView::section { background: #f5f5f5; color: #333; padding: 8px;
                               border: none; font-weight: bold; font-size: 12px; }
        #statsPanel  { background: white; }
    )";

    this->setStyleSheet(styleSheet);
}
