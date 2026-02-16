#include "gestion_des_poubelles.h"
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

// ============================================
// DONUT CHART WIDGET
// ============================================
DonutChart::DonutChart(const QString &title, QWidget *parent)
    : QWidget(parent), title(title), total(0)
{
    setMinimumSize(250, 300);
    setStyleSheet("background-color: white; border: 1px solid #ddd; border-radius: 8px;");

    // Couleurs pour États (Moyen, Pleine, Vide)
    colors["Moyen"] = QColor(255, 152, 0);               // Orange
    colors["Pleine"] = QColor(244, 67, 54);              // Rouge
    colors["Vide"] = QColor(255, 193, 7);                // Jaune/Or
    colors["Opérationnelle"] = QColor(76, 175, 80);      // Vert
    colors["Maintenance"] = QColor(33, 150, 243);        // Bleu
    colors["Hors service"] = QColor(244, 67, 54);        // Rouge

    // Couleurs pour Types (Papier, Plastique, Verre, Organique, Autre)
    colors["Plastique"] = QColor(255, 152, 0);           // Orange
    colors["Papier"] = QColor(255, 193, 7);              // Jaune/Or
    colors["Verre"] = QColor(33, 150, 243);              // Bleu
    colors["Organique"] = QColor(76, 175, 80);           // Vert
    colors["Autre"] = QColor(156, 39, 176);              // Violet
}

void DonutChart::setData(const QMap<QString, int> &newData)
{
    data = newData;
    total = 0;
    for (auto count : data) {
        total += count;
    }
    update();
}

void DonutChart::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    // Titre
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

    // Donut - dans le même ordre que la légende
    int centerX = w / 2;
    int centerY = 90;
    int outerRadius = 45;
    int innerRadius = 30;

    float startAngle = 0;

    QStringList order;
    if (title == "Types de Poubelles") {
        order << "Plastique" << "Papier" << "Verre" << "Organique" << "Autre";
    } else if (title == "État des Poubelles") {
        order << "Moyen" << "Pleine" << "Vide";
    }

    for (const QString &key : order) {
        if (!data.contains(key) || data[key] == 0) continue;

        int count = data[key];
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

    // Nombre au centre
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.setPen(Qt::black);
    painter.drawText(centerX - 30, centerY - 10, 60, 20, Qt::AlignCenter, QString::number(total));

    // Légende CENTRÉE avec couleurs correctes - dans le même ordre que le donut
    painter.setFont(QFont("Arial", 9));

    // Afficher la légende dans l'ordre des données qui existent
    QVector<QPair<QString, int>> sortedData;

    if (title == "Types de Poubelles") {
        // Ordre pour Types
        QStringList order;
        order << "Plastique" << "Papier" << "Verre" << "Organique" << "Autre";

        for (const QString &key : order) {
            if (data.contains(key) && data[key] > 0) {
                sortedData.append(qMakePair(key, data[key]));
            }
        }
    } else if (title == "État des Poubelles") {
        // Ordre pour États
        QStringList order;
        order << "Moyen" << "Pleine" << "Vide";

        for (const QString &key : order) {
            if (data.contains(key) && data[key] > 0) {
                sortedData.append(qMakePair(key, data[key]));
            }
        }
    }

    int legendY = 160;  // Initialiser la position Y
    for (const auto &item : sortedData) {
        QString key = item.first;
        int count = item.second;
        int legendX = 20;

        // Carré de couleur
        painter.fillRect(legendX, legendY - 6, 8, 8, colors.value(key));
        painter.setPen(Qt::black);
        painter.drawRect(legendX, legendY - 6, 8, 8);

        // Texte avec couleur noire
        painter.setPen(Qt::black);
        painter.drawText(legendX + 12, legendY, key + ": " + QString::number(count));

        legendY += 18;
    }
}

// ============================================
// MAIN WINDOW
// ============================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentEditingRow(-1), nextId(1)
{
    setWindowTitle("TuniWaste - Gestion des poubelles");
    resize(1600, 950);
    setMinimumSize(1400, 850);

    // Center window
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - 1600) / 2;
    int y = (screenGeometry.height() - 950) / 2;
    move(x, y);

    setupUI();
    applyStyles();
    loadDummyData();
    updateCharts();
}

MainWindow::~MainWindow() {}

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

void MainWindow::createSidebar()
{
    sidebar = new QWidget();
    sidebar->setFixedWidth(280);
    sidebar->setObjectName("sidebar");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setSpacing(0);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);

    // Logo et Titre côte à côte
    QWidget *logoWidget = new QWidget();
    QHBoxLayout *logoLayout = new QHBoxLayout(logoWidget);
    logoLayout->setContentsMargins(10, 15, 10, 15);
    logoLayout->setSpacing(10);
    logoLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *logoLabel = new QLabel();
    logoLabel->setFixedSize(100, 100);
    logoLabel->setScaledContents(false);

    // Obtenir le chemin de l'exécutable et remonter au dossier build
    QString appDir = QCoreApplication::applicationDirPath();
    QDir dir(appDir);
    dir.cdUp();
    QString logoPath = dir.absoluteFilePath("logo.png");

    // Charger le logo depuis le dossier build
    QPixmap logoPixmap(logoPath);
    if (!logoPixmap.isNull()) {
        QPixmap scaledLogo = logoPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        logoLabel->setPixmap(scaledLogo);
        logoLabel->setAlignment(Qt::AlignCenter);
    } else {
        // Si le logo n'existe pas, afficher un cercle de couleur par défaut
        logoLabel->setStyleSheet("background: #9BCB4E; border-radius: 50px;");
    }

    QLabel *titleLabel = new QLabel("TuniWaste");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setObjectName("sidebarTitle");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    logoLayout->addWidget(logoLabel, 0, Qt::AlignVCenter);
    logoLayout->addWidget(titleLabel, 0, Qt::AlignVCenter);
    logoLayout->addStretch();

    sidebarLayout->addWidget(logoWidget);

    // Menu Items
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

    // User profile
    QWidget *userWidget = new QWidget();
    userWidget->setObjectName("userWidget");
    QHBoxLayout *userLayout = new QHBoxLayout(userWidget);
    userLayout->setContentsMargins(15, 15, 15, 15);

    QLabel *userLabel = new QLabel("Ahmed");
    userLabel->setObjectName("userName");
    userLayout->addWidget(userLabel);
    userLayout->addStretch();

    sidebarLayout->addWidget(userWidget);
}

void MainWindow::createMainContent()
{
    mainContent = new QWidget();
    mainContent->setObjectName("mainContent");
    mainContent->setStyleSheet("background-color: white;");

    QVBoxLayout *mainLayout = new QVBoxLayout(mainContent);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *breadcrumb = new QLabel("Tableau de bord / Gestion des poubelles");
    breadcrumb->setObjectName("breadcrumb");
    breadcrumb->setStyleSheet("color: #999; font-size: 12px;");

    headerLayout->addWidget(breadcrumb);
    headerLayout->addStretch();

    QLabel *notifIcon = new QLabel("️");
    QLabel *settingsIcon = new QLabel("️");
    headerLayout->addWidget(notifIcon);
    headerLayout->addWidget(settingsIcon);

    mainLayout->addLayout(headerLayout);

    // Content area with form and table
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(15);

    // Form Panel
    createFormPanel();
    contentLayout->addWidget(formPanel, 0);

    // Table and Stats Panel
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(10);

    createTablePanel();
    rightLayout->addWidget(tablePanel, 1);

    createStatsPanel();
    rightLayout->addWidget(statsPanel, 1);

    contentLayout->addLayout(rightLayout, 1);

    mainLayout->addLayout(contentLayout, 1);
}

void MainWindow::createFormPanel()
{
    formPanel = new QWidget();
    formPanel->setFixedWidth(330);
    formPanel->setObjectName("formPanel");
    formPanel->setStyleSheet("background-color: #f9f9f9; border: 1px solid #e0e0e0; border-radius: 8px;");

    QVBoxLayout *layout = new QVBoxLayout(formPanel);
    layout->setSpacing(12);
    layout->setContentsMargins(25, 20, 25, 20);

    // Title
    QLabel *formTitle = new QLabel("Ajouter une poubelle");
    formTitle->setObjectName("formTitle");
    QFont titleFont = formTitle->font();
    titleFont.setPointSize(13);
    titleFont.setBold(true);
    formTitle->setFont(titleFont);
    layout->addWidget(formTitle);

    // Separator
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
    QLabel *zoneLabel = new QLabel("ID Zone");
    zoneLabel->setObjectName("formLabel");
    layout->addWidget(zoneLabel);
    idZoneInput = new QLineEdit();
    idZoneInput->setPlaceholderText("Ex: ZONE-001, ZONE-002");
    idZoneInput->setObjectName("formInput");
    idZoneInput->setMinimumHeight(40);
    layout->addWidget(idZoneInput);

    layout->addSpacing(20);

    // Button Enregistrer SEULEMENT
    enregistrerBtn = new QPushButton("Enregistrer");
    enregistrerBtn->setObjectName("btnEnregistrer");
    enregistrerBtn->setMinimumHeight(50);
    enregistrerBtn->setCursor(Qt::PointingHandCursor);
    enregistrerBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #66BB6A;"
        "    color: #000000;"
        "    border: none;"
        "    border-radius: 4px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    padding: 10px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #5AA55A;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #4A9449;"
        "}"
        );
    connect(enregistrerBtn, &QPushButton::clicked, this, &MainWindow::onEnregistrerClicked);
    layout->addWidget(enregistrerBtn);

    // Astuce
    QLabel *tipsLabel = new QLabel("💡 Astuce:\nCliquez sur 'Modifier' dans le tableau pour éditer une poubelle existante.");
    tipsLabel->setObjectName("tipsLabel");
    tipsLabel->setStyleSheet("background-color: #e3f2fd; padding: 10px; border-radius: 4px; font-size: 11px; color: #1976d2;");
    layout->addWidget(tipsLabel);

    layout->addStretch();
}

void MainWindow::createTablePanel()
{
    tablePanel = new QWidget();
    tablePanel->setObjectName("tablePanel");

    QVBoxLayout *layout = new QVBoxLayout(tablePanel);
    layout->setSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);

    // Title
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *tableTitle = new QLabel("Liste des poubelles");
    tableTitle->setObjectName("tableTitle");
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
    exportPdfBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #FF9800;"
        "    color: #FFFFFF;"
        "    border: none;"
        "    border-radius: 4px;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "    padding: 8px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #E68900;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #D68000;"
        "}"
        );
    connect(exportPdfBtn, &QPushButton::clicked, this, &MainWindow::onExporterPDFClicked);
    titleLayout->addWidget(exportPdfBtn);

    layout->addLayout(titleLayout);

    // Filters
    QHBoxLayout *filterLayout = new QHBoxLayout();

    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("🔍Rechercher...");
    searchInput->setObjectName("searchInput");
    searchInput->setMaximumHeight(40);
    searchInput->setMinimumWidth(200);
    connect(searchInput, &QLineEdit::textChanged, this, &MainWindow::onSearchChanged);
    filterLayout->addWidget(searchInput);

    QLabel *etatFilterLabel = new QLabel("Tous les états");
    etatFilterCombo = new QComboBox();
    etatFilterCombo->addItems({"Tous les états", "Opérationnelle", "Pleine", "Maintenance"});
    etatFilterCombo->setObjectName("filterCombo");
    etatFilterCombo->setMaximumHeight(40);
    etatFilterCombo->setMaximumWidth(150);
    connect(etatFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onEtatFilterChanged);
    filterLayout->addWidget(etatFilterLabel);
    filterLayout->addWidget(etatFilterCombo);

    QLabel *typeFilterLabel = new QLabel("Tous les types");
    typeFilterCombo = new QComboBox();
    typeFilterCombo->addItems({"Tous les types", "Plastique", "Papier", "Verre", "Organique"});
    typeFilterCombo->setObjectName("filterCombo");
    typeFilterCombo->setMaximumHeight(40);
    typeFilterCombo->setMaximumWidth(150);
    connect(typeFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onTypeFilterChanged);
    filterLayout->addWidget(typeFilterLabel);
    filterLayout->addWidget(typeFilterCombo);

    filterLayout->addStretch();
    layout->addLayout(filterLayout);

    // Table
    poubelleTable = new QTableWidget();
    poubelleTable->setColumnCount(6);
    poubelleTable->setHorizontalHeaderLabels({"CAPACITÉ", "NIVEAU", "ÉTAT", "TYPE", "ID ZONE", "ACTIONS"});
    poubelleTable->setObjectName("poubelleTable");
    poubelleTable->setAlternatingRowColors(false);  // Désactiver les couleurs alternées
    poubelleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    poubelleTable->setSelectionMode(QAbstractItemView::SingleSelection);
    poubelleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    poubelleTable->setShowGrid(false);  // Supprimer la grille
    poubelleTable->setStyleSheet(
        "QTableWidget { border: none; background-color: white; }"
        "QTableWidget::item { border: none; padding: 5px; background-color: white; }"
        "QTableWidget::item:selected { background-color: white; border: none; }"
        "QHeaderView::section { background-color: #f5f5f5; color: #333; padding: 8px; border: none; font-weight: bold; }"
        );

    QHeaderView *header = poubelleTable->horizontalHeader();
    header->setStyleSheet("background-color: #f5f5f5; font-weight: bold;");
    header->setSectionResizeMode(QHeaderView::Stretch);

    poubelleTable->verticalHeader()->setDefaultSectionSize(60);

    connect(poubelleTable, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::onTableCellDoubleClicked);

    layout->addWidget(poubelleTable, 1);
}

void MainWindow::createStatsPanel()
{
    statsPanel = new QWidget();
    statsPanel->setObjectName("statsPanel");
    statsPanel->setStyleSheet("background-color: white;");

    QVBoxLayout *layout = new QVBoxLayout(statsPanel);
    layout->setSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);

    // Title
    QLabel *statsTitle = new QLabel("Statistiques des Poubelles");
    statsTitle->setObjectName("statsTitle");
    QFont titleFont = statsTitle->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    statsTitle->setFont(titleFont);
    layout->addWidget(statsTitle);

    // Charts and level
    QHBoxLayout *chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(15);

    // Donut 1: État
    etatChart = new DonutChart("État des Poubelles");
    chartsLayout->addWidget(etatChart, 1);

    // Donut 2: Types
    typeChart = new DonutChart("Types de Poubelles");
    chartsLayout->addWidget(typeChart, 1);

    // Niveau Moyen - Donut Chart
    niveauDonutChart = new NiveauDonutChart();
    chartsLayout->addWidget(niveauDonutChart, 1);

    layout->addLayout(chartsLayout, 1);
}

void MainWindow::loadDummyData()
{
    addTableRow("240 L", 45, "Moyen", "ZONE-001");
    addTableRow("120 L", 78, "Pleine", "ZONE-002");
    addTableRow("240 L", 20, "Vide", "ZONE-001");
}

void MainWindow::addTableRow(const QString &capacite, int niveau,
                             const QString &etat, const QString &idZone)
{
    int row = poubelleTable->rowCount();
    poubelleTable->insertRow(row);

    // Capacité - BLANC
    QTableWidgetItem *capItem = new QTableWidgetItem(capacite);
    capItem->setForeground(Qt::black);
    capItem->setBackground(Qt::white);
    poubelleTable->setItem(row, 0, capItem);

    // Niveau - BLANC (pas de couleur de fond)
    QTableWidgetItem *nivItem = new QTableWidgetItem(QString::number(niveau) + "%");
    nivItem->setForeground(Qt::black);
    nivItem->setBackground(Qt::white);
    poubelleTable->setItem(row, 1, nivItem);

    // État - BLANC (sans gras)
    QTableWidgetItem *etatItem = new QTableWidgetItem(etat);
    etatItem->setForeground(Qt::black);
    etatItem->setBackground(Qt::white);
    poubelleTable->setItem(row, 2, etatItem);

    // Type - BLANC
    QTableWidgetItem *typeItem = new QTableWidgetItem(typeCombo->currentText());
    typeItem->setForeground(Qt::black);
    typeItem->setBackground(Qt::white);
    poubelleTable->setItem(row, 3, typeItem);

    // ID Zone - BLANC
    QTableWidgetItem *zoneItem = new QTableWidgetItem(idZone);
    zoneItem->setForeground(Qt::black);
    zoneItem->setBackground(Qt::white);
    poubelleTable->setItem(row, 4, zoneItem);

    // Actions - CENTRÉES
    QWidget *actionsWidget = new QWidget();
    QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setContentsMargins(0, 5, 0, 5);
    actionsLayout->setSpacing(10);
    actionsLayout->addStretch();  // Avant les boutons

    QPushButton *modBtn = new QPushButton("Modifier");
    modBtn->setObjectName("tableActionBtn");
    modBtn->setMaximumWidth(85);
    modBtn->setMinimumHeight(35);
    modBtn->setStyleSheet("background-color: #66BB6A; color: white; border: none; border-radius: 4px; font-weight: bold; font-size: 11px;");

    QPushButton *delBtn = new QPushButton("Supprimer");
    delBtn->setObjectName("tableActionBtn");
    delBtn->setMaximumWidth(85);
    delBtn->setMinimumHeight(35);
    delBtn->setStyleSheet("background-color: #f44336; color: white; border: none; border-radius: 4px; font-weight: bold; font-size: 11px;");

    actionsLayout->addWidget(modBtn);
    actionsLayout->addWidget(delBtn);
    actionsLayout->addStretch();  // Après les boutons

    poubelleTable->setCellWidget(row, 5, actionsWidget);
}

void MainWindow::updateCharts()
{
    // Count states and types
    QMap<QString, int> stateCount;
    QMap<QString, int> typeCount;
    int totalNiveau = 0;
    int rows = 0;

    // Initialiser les états avec les bonnes clés
    stateCount["Moyen"] = 0;
    stateCount["Pleine"] = 0;
    stateCount["Vide"] = 0;
    stateCount["Opérationnelle"] = 0;
    stateCount["Maintenance"] = 0;
    stateCount["Hors service"] = 0;

    typeCount["Papier"] = 0;
    typeCount["Plastique"] = 0;
    typeCount["Verre"] = 0;
    typeCount["Organique"] = 0;
    typeCount["Autre"] = 0;

    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        QString etat = poubelleTable->item(row, 2)->text();
        if (stateCount.contains(etat)) {
            stateCount[etat]++;
        }

        // Get type from table
        QString type = poubelleTable->item(row, 3)->text();
        if (typeCount.contains(type)) {
            typeCount[type]++;
        }

        int niveau = poubelleTable->item(row, 1)->text().replace("%", "").toInt();
        totalNiveau += niveau;
        rows++;
    }

    // Update charts
    if (etatChart) etatChart->setData(stateCount);
    if (typeChart) typeChart->setData(typeCount);

    // Update niveau moyen donut
    if (rows > 0 && niveauDonutChart) {
        int moyenne = totalNiveau / rows;
        niveauDonutChart->setNiveau(moyenne);
    }
}

// SLOTS
void MainWindow::onTableCellDoubleClicked(int row, int column)
{
    currentEditingRow = row;
    localisationInput->setText("Modif. - Row " + QString::number(row));
    niveauInput->setValue(poubelleTable->item(row, 1)->text().replace("%", "").toInt());
    idZoneInput->setText(poubelleTable->item(row, 3)->text());
}

void MainWindow::onEnregistrerClicked()
{
    if (localisationInput->text().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Remplissez les champs!");
        return;
    }

    addTableRow(capaciteInput->text() + " L", niveauInput->value(),
                etatCombo->currentText(), idZoneInput->text());
    updateCharts();
    localisationInput->clear();

    QMessageBox::information(this, "Succès", "Poubelle ajoutée!");
}

void MainWindow::onModifierClicked()
{
    if (currentEditingRow == -1) {
        QMessageBox::warning(this, "Erreur", "Sélectionnez une ligne!");
        return;
    }

    poubelleTable->item(currentEditingRow, 1)->setText(QString::number(niveauInput->value()) + "%");
    poubelleTable->item(currentEditingRow, 2)->setText(etatCombo->currentText());
    poubelleTable->item(currentEditingRow, 3)->setText(typeCombo->currentText());
    poubelleTable->item(currentEditingRow, 4)->setText(idZoneInput->text());

    updateCharts();
    QMessageBox::information(this, "Succès", "Poubelle modifiée!");
}

void MainWindow::onSupprimerClicked()
{
    if (currentEditingRow == -1) {
        QMessageBox::warning(this, "Erreur", "Sélectionnez une ligne!");
        return;
    }

    poubelleTable->removeRow(currentEditingRow);
    updateCharts();
    QMessageBox::information(this, "Succès", "Poubelle supprimée!");
}

void MainWindow::onExporterPDFClicked()
{
    // Ouvrir la boîte de dialogue pour sauvegarder le fichier
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Exporter en PDF", "",
                                                    "Documents PDF (*.pdf)");

    if (fileName.isEmpty()) {
        return;
    }

    // S'assurer que l'extension .pdf est présente
    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        fileName += ".pdf";
    }

    // Créer un document HTML avec les données du tableau
    QString htmlContent = R"(
        <html>
        <head>
            <meta charset='utf-8'>
            <title>Gestion des Poubelles - Rapport</title>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    margin: 20px;
                    color: #333;
                }
                h1 {
                    color: #66BB6A;
                    text-align: center;
                    border-bottom: 3px solid #66BB6A;
                    padding-bottom: 10px;
                }
                .info {
                    text-align: right;
                    color: #666;
                    margin-bottom: 20px;
                    font-size: 12px;
                }
                table {
                    border-collapse: collapse;
                    width: 100%;
                    margin-top: 20px;
                }
                th {
                    background-color: #66BB6A;
                    color: white;
                    padding: 12px;
                    text-align: left;
                    border: 1px solid #ddd;
                    font-weight: bold;
                }
                td {
                    padding: 10px;
                    border: 1px solid #ddd;
                    border-top: 1px solid #eee;
                }
                tr:nth-child(even) {
                    background-color: #f9f9f9;
                }
                .footer {
                    text-align: center;
                    margin-top: 30px;
                    color: #999;
                    font-size: 11px;
                    border-top: 1px solid #ddd;
                    padding-top: 15px;
                }
            </style>
        </head>
        <body>
            <h1>Rapport de Gestion des Poubelles</h1>
            <div class="info">
                <p><strong>Date:</strong> )";

    htmlContent += QDateTime::currentDateTime().toString("dd/MM/yyyy");
    htmlContent += R"(</p>
                <p><strong>Heure:</strong> )";
    htmlContent += QDateTime::currentDateTime().toString("hh:mm:ss");
    htmlContent += R"(</p>
            </div>
            <table>
                <thead>
                    <tr>
                        <th style="width: 15%;">CAPACITÉ</th>
                        <th style="width: 15%;">NIVEAU</th>
                        <th style="width: 15%;">ÉTAT</th>
                        <th style="width: 20%;">TYPE</th>
                        <th style="width: 20%;">ID ZONE</th>
                    </tr>
                </thead>
                <tbody>)";

    // Ajouter les données du tableau
    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        htmlContent += "<tr>";
        for (int col = 0; col < 5; ++col) {
            htmlContent += "<td>" + poubelleTable->item(row, col)->text() + "</td>";
        }
        htmlContent += "</tr>";
    }

    htmlContent += R"(
                </tbody>
            </table>
            <div class="footer">
                <p>Document généré par TuniWaste - Gestion des Poubelles</p>
                <p>© 2026 - Tous droits réservés</p>
            </div>
        </body>
        </html>
    )";

    // Créer un document texte et l'imprimer en PDF
    QTextDocument doc;
    doc.setHtml(htmlContent);

    // Créer une imprimante PDF
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    // Utiliser les marges par défaut
    // Imprimer le document
    doc.print(&printer);

    // Afficher un message de succès
    QMessageBox::information(this, "Succès",
                             "Fichier PDF exporté avec succès!\n\n" + fileName);
}

void MainWindow::onSearchChanged(const QString &text)
{
    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        bool match = text.isEmpty();
        for (int col = 0; col < 5 && !match; ++col) {
            if (poubelleTable->item(row, col)->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
            }
        }
        poubelleTable->setRowHidden(row, !match);
    }
}

void MainWindow::onEtatFilterChanged(int index)
{
    if (index == 0) {
        for (int row = 0; row < poubelleTable->rowCount(); ++row) {
            poubelleTable->setRowHidden(row, false);
        }
        return;
    }

    QString selected = etatFilterCombo->currentText();
    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        QString etat = poubelleTable->item(row, 2)->text();
        poubelleTable->setRowHidden(row, etat != selected);
    }
}

void MainWindow::onTypeFilterChanged(int index)
{
    // À implémenter
}

void MainWindow::applyStyles()
{
    QString styleSheet = R"(
        QMainWindow {
            background: #f5f5f5;
        }

        #sidebar {
            background: #66BB6A;
            color: white;
        }

        #sidebarTitle {
            color: white;
        }

        #menuItem {
            background: transparent;
            color: white;
            border: none;
            padding: 0 20px;
            text-align: left;
            font-size: 13px;
        }

        #menuItem:hover {
            background: rgba(255,255,255,0.2);
        }

        #menuItem[active="true"] {
            background: rgba(155,203,78,0.3);
            border-left: 4px solid #9BCB4E;
        }

        #userWidget {
            border-top: 1px solid rgba(255,255,255,0.2);
            background: rgba(0,0,0,0.1);
        }

        #userName {
            color: white;
            font-weight: bold;
        }

        #mainContent {
            background: white;
        }

        #breadcrumb {
            color: #999;
        }

        #formPanel {
            background: #f9f9f9;
            border: 1px solid #e0e0e0;
        }

        #formTitle {
            color: #2c3e50;
        }

        #formLabel {
            color: #333;
            font-weight: bold;
            font-size: 12px;
        }

        #formInput {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 8px;
            background: white;
            color: #000;
        }

        #formInput:focus {
            border: 2px solid #66BB6A;
            background: #f0f8f0;
        }

        QComboBox {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 8px;
            background: white;
            color: #000;
        }

        QComboBox:focus {
            border: 2px solid #66BB6A;
            background: #f0f8f0;
        }

        QComboBox::drop-down {
            border: none;
        }

        QComboBox QAbstractItemView {
            background-color: white;
            color: #000;
            selection-background-color: #ddd;
            selection-color: #000;
        }

        #btnEnregistrer {
            background-color: #66BB6A;
            color: #000000;
            border: none;
            border-radius: 4px;
            font-weight: bold;
            font-size: 14px;
            padding: 10px;
        }

        #btnEnregistrer:hover {
            background-color: #5AA55A;
            color: #000000;
        }

        #btnEnregistrer:pressed {
            background-color: #4A9449;
            color: #000000;
        }

        QPushButton#btnEnregistrer {
            background-color: #66BB6A;
            color: #000000;
        }

        #btnSupprimer {
            background: #f44336;
            color: white;
            border: none;
            border-radius: 4px;
            font-weight: bold;
        }

        #tipsLabel {
            border-left: 4px solid #1976d2;
        }

        #tableTitle {
            color: #333;
        }

        #btnExport {
            background: #f44336;
            color: white;
            border: none;
            border-radius: 4px;
            font-weight: bold;
        }

        #searchInput, #filterCombo {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 8px;
            background: white;
        }

        #searchInput:focus, #filterCombo:focus {
            border: 2px solid #66BB6A;
        }

        #poubelleTable {
            background: white;
            gridline-color: #f0f0f0;
            border: 1px solid #ddd;
            color: #000;
        }

        QHeaderView::section {
            background: #f5f5f5;
            color: #333;
            padding: 8px;
            border: none;
            font-weight: bold;
            font-size: 12px;
        }

        #statsPanel {
            background: white;
        }

        #statsTitle {
            color: #333;
        }

        #niveauTitle {
            color: #333;
        }

        #niveauValue {
            color: #9BCB4E;
        }

        #niveauSubtitle {
            color: #999;
        }

        #niveauWidget {
            background: white;
        }
    )";

    this->setStyleSheet(styleSheet);
}
