#include "mainwindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>
#include <QGroupBox>
#include <QScrollArea>
#include <QFileDialog>
#include <QDateTime>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>

// ==================== MainWindow Implementation ====================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), nextId(4), currentEditingRow(-1)
{
    setupUI();
    loadPoubelleData();
    applyStyles();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    createSidebar();
    createFormPanel();
    createMainContent();

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(formPanel);
    mainLayout->addWidget(mainContent);

    setWindowTitle("TuniWaste - Gestion des poubelles");
    resize(1920, 900);  // Augmenté de 1800 à 1920 (Full HD)
    setMinimumSize(1800, 800);  // Augmenté de 1700 à 1800
}

void MainWindow::createSidebar()
{
    sidebar = new QWidget();
    sidebar->setFixedWidth(280);
    sidebar->setObjectName("sidebar");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setSpacing(0);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);

    // Logo Section
    QWidget *logoWidget = new QWidget();
    QHBoxLayout *logoLayout = new QHBoxLayout(logoWidget);
    logoLayout->setContentsMargins(20, 30, 20, 30);

    QLabel *logoLabel = new QLabel();
    logoLabel->setFixedSize(80, 80);
    logoLabel->setStyleSheet("background: transparent; border-radius: 15px;");

    // Try to load logo from different locations
    QPixmap logo;
    QStringList logoPaths = {
        QCoreApplication::applicationDirPath() + "/logo.png",  // Dossier build en premier
        "logo.png",
        "./logo.png",
        "../logo.png",
        "../../logo.png",
        "./build/logo.png"
    };

    for (const QString &path : logoPaths) {
        logo = QPixmap(path);
        if (!logo.isNull()) {
            break;
        }
    }

    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoLabel->setScaledContents(false);
    } else {
        logoLabel->setStyleSheet("background: #9BCB4E; border-radius: 40px;");
    }

    QLabel *titleLabel = new QLabel("TuniWaste");
    titleLabel->setObjectName("logoTitle");

    logoLayout->addWidget(logoLabel);
    logoLayout->addWidget(titleLabel);
    logoLayout->addStretch();

    sidebarLayout->addWidget(logoWidget);

    // Menu Items
    QStringList menuItems = {
        "🏠 Tableau de bord",
        "👤 Gestion des utilisateurs",
        "🚛 Gestion des camions",
        "📍 Gestion des zones",
        "🗑️ Gestion des poubelles",
        "📊 Gestion de recyclage",
        "📄 Rapports",
        "⚙️ Paramètres"
    };

    for (int i = 0; i < menuItems.size(); ++i) {
        QPushButton *menuBtn = new QPushButton(menuItems[i]);
        menuBtn->setObjectName("menuItem");
        menuBtn->setCursor(Qt::PointingHandCursor);
        menuBtn->setFixedHeight(50);

        if (i == 4) { // Gestion des poubelles est active
            menuBtn->setProperty("active", true);
        }

        connect(menuBtn, &QPushButton::clicked, this, &MainWindow::onMenuItemClicked);

        menuButtons.append(menuBtn);
        sidebarLayout->addWidget(menuBtn);
    }

    sidebarLayout->addStretch();

    // User Profile
    // TODO: Cette section sera connectée au module de gestion des utilisateurs
    // Lorsque l'intégration sera faite:
    // 1. Remplacer "Ahmed" par currentUser->getName()
    // 2. Charger la photo de profil depuis currentUser->getAvatarPath()
    // 3. Afficher le rôle: currentUser->getRole() (Admin, Gestionnaire, etc.)

    QWidget *userWidget = new QWidget();
    userWidget->setObjectName("userProfile");
    userWidget->setStyleSheet("border-top: 1px solid rgba(255, 255, 255, 0.2);");

    QHBoxLayout *userLayout = new QHBoxLayout(userWidget);
    userLayout->setContentsMargins(20, 20, 20, 20);

    // Avatar - sera remplacé par l'image de l'utilisateur connecté
    userAvatar = new QLabel();
    userAvatar->setObjectName("userAvatar");
    userAvatar->setFixedSize(50, 50);
    userAvatar->setStyleSheet("border-radius: 25px; background: #ddd;");

    // Nom de l'utilisateur - sera mis à jour dynamiquement
    userNameLabel = new QLabel("Ahmed");  // Valeur par défaut
    userNameLabel->setObjectName("userName");

    userLayout->addWidget(userAvatar);
    userLayout->addWidget(userNameLabel);
    userLayout->addStretch();

    sidebarLayout->addWidget(userWidget);
}

void MainWindow::createFormPanel()
{
    formPanel = new QWidget();
    formPanel->setFixedWidth(400);
    formPanel->setObjectName("formPanel");

    QVBoxLayout *formLayout = new QVBoxLayout(formPanel);
    formLayout->setContentsMargins(25, 30, 25, 30);
    formLayout->setSpacing(20);

    // Form Title
    formTitleLabel = new QLabel("📝 Ajouter une poubelle");
    formTitleLabel->setObjectName("formTitle");
    QFont titleFont = formTitleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    formTitleLabel->setFont(titleFont);
    formLayout->addWidget(formTitleLabel);

    // Separator
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("background: #ddd; max-height: 1px;");
    formLayout->addWidget(separator);

    formLayout->addSpacing(10);

    // Form Fields - Vertical Layout with better spacing
    QVBoxLayout *fieldsLayout = new QVBoxLayout();
    fieldsLayout->setSpacing(4);  // Réduit de 18 à 4 pour rapprocher les éléments
    fieldsLayout->setContentsMargins(0, 0, 0, 0);

    // Localisation
    QLabel *localisationLabel = new QLabel("📍 Localisation");
    localisationLabel->setObjectName("formLabel");
    localisationInput = new QLineEdit();
    localisationInput->setObjectName("formInput");
    localisationInput->setPlaceholderText("Ex: Rue de la République, Tunis");
    localisationInput->setFixedHeight(40);
    fieldsLayout->addWidget(localisationLabel);
    fieldsLayout->addWidget(localisationInput);
    fieldsLayout->addSpacing(12);  // Espacement entre les champs

    // Type
    QLabel *typeLabel = new QLabel("🗂️ Type de déchet");
    typeLabel->setObjectName("formLabel");
    typeCombo = new QComboBox();
    typeCombo->setObjectName("formInput");
    typeCombo->addItem("🔵 Plastique");
    typeCombo->addItem("🟢 Verre");
    typeCombo->addItem("🟡 Papier");
    typeCombo->addItem("🟤 Organique");
    typeCombo->addItem("⚪ Métal");
    typeCombo->addItem("⚫ Mixte");
    typeCombo->setFixedHeight(40);
    fieldsLayout->addWidget(typeLabel);
    fieldsLayout->addWidget(typeCombo);
    fieldsLayout->addSpacing(12);  // Espacement entre les champs

    // Capacité
    QLabel *capaciteLabel = new QLabel("📦 Capacité");
    capaciteLabel->setObjectName("formLabel");
    capaciteInput = new QSpinBox();
    capaciteInput->setObjectName("formInput");
    capaciteInput->setSuffix(" L");
    capaciteInput->setMinimum(10);
    capaciteInput->setMaximum(1000);
    capaciteInput->setValue(120);
    capaciteInput->setFixedHeight(40);
    fieldsLayout->addWidget(capaciteLabel);
    fieldsLayout->addWidget(capaciteInput);
    fieldsLayout->addSpacing(12);  // Espacement entre les champs

    // Niveau
    QLabel *niveauLabel = new QLabel("📊 Niveau de remplissage");
    niveauLabel->setObjectName("formLabel");
    niveauInput = new QSpinBox();
    niveauInput->setObjectName("formInput");
    niveauInput->setSuffix(" %");
    niveauInput->setMinimum(0);
    niveauInput->setMaximum(100);
    niveauInput->setValue(0);
    niveauInput->setFixedHeight(40);
    fieldsLayout->addWidget(niveauLabel);
    fieldsLayout->addWidget(niveauInput);
    fieldsLayout->addSpacing(12);  // Espacement entre les champs

    // État
    QLabel *etatLabel = new QLabel("⚙️ État");
    etatLabel->setObjectName("formLabel");
    etatCombo = new QComboBox();
    etatCombo->setObjectName("formInput");
    etatCombo->addItem("✅ Opérationnelle");
    etatCombo->addItem("🔧 Maintenance");
    etatCombo->addItem("❌ Hors service");
    etatCombo->addItem("🔴 Pleine");
    etatCombo->setFixedHeight(40);
    fieldsLayout->addWidget(etatLabel);
    fieldsLayout->addWidget(etatCombo);
    fieldsLayout->addSpacing(12);  // Espacement entre les champs

    // ID Zone
    QLabel *idZoneLabel = new QLabel("🏷️ ID Zone");
    idZoneLabel->setObjectName("formLabel");
    idZoneInput = new QLineEdit();
    idZoneInput->setObjectName("formInput");
    idZoneInput->setPlaceholderText("Ex: ZONE-001, ZONE-002");
    idZoneInput->setFixedHeight(40);
    fieldsLayout->addWidget(idZoneLabel);
    fieldsLayout->addWidget(idZoneInput);

    formLayout->addLayout(fieldsLayout);
    formLayout->addSpacing(20);

    // Save Button
    saveButton = new QPushButton("💾 Enregistrer");
    saveButton->setObjectName("saveButton");
    saveButton->setFixedHeight(42);
    saveButton->setCursor(Qt::PointingHandCursor);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onAddPoubelle);

    formLayout->addWidget(saveButton);
    formLayout->addStretch();

    // Info Box
    QLabel *infoBox = new QLabel(
        "💡 <b>Astuce:</b><br>"
        "Cliquez sur 'Modifier' dans le tableau pour éditer une poubelle existante."
        );
    infoBox->setObjectName("infoBox");
    infoBox->setWordWrap(true);
    formLayout->addWidget(infoBox);
}

void MainWindow::createMainContent()
{
    mainContent = new QWidget();
    mainContent->setObjectName("mainContent");

    QVBoxLayout *contentLayout = new QVBoxLayout(mainContent);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(15);

    // Header
    QWidget *headerWidget = new QWidget();
    headerWidget->setObjectName("header");
    headerWidget->setFixedHeight(50);

    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 10, 20, 10);

    QLabel *breadcrumb = new QLabel("Tableau de bord / Gestion des poubelles");
    breadcrumb->setObjectName("breadcrumb");

    QWidget *headerButtonsWidget = new QWidget();
    QHBoxLayout *headerButtonsLayout = new QHBoxLayout(headerButtonsWidget);
    headerButtonsLayout->setSpacing(10);
    headerButtonsLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *notifBtn = new QPushButton("🔔");
    notifBtn->setObjectName("headerBtn");
    notifBtn->setFixedSize(40, 40);

    QPushButton *settingsBtn = new QPushButton("⚙️");
    settingsBtn->setObjectName("headerBtn");
    settingsBtn->setFixedSize(40, 40);

    headerButtonsLayout->addWidget(notifBtn);
    headerButtonsLayout->addWidget(settingsBtn);

    headerLayout->addWidget(breadcrumb);
    headerLayout->addStretch();
    headerLayout->addWidget(headerButtonsWidget);

    contentLayout->addWidget(headerWidget);

    // Content Wrapper
    QWidget *contentWrapper = new QWidget();
    contentWrapper->setObjectName("contentWrapper");

    QVBoxLayout *wrapperLayout = new QVBoxLayout(contentWrapper);
    wrapperLayout->setContentsMargins(20, 20, 20, 20);
    wrapperLayout->setSpacing(15);

    // Page Title
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *pageTitle = new QLabel("Liste des poubelles");
    pageTitle->setObjectName("pageTitle");
    titleLayout->addWidget(pageTitle);
    titleLayout->addStretch();

    wrapperLayout->addLayout(titleLayout);

    // Search and Filters
    QHBoxLayout *filtersLayout = new QHBoxLayout();
    filtersLayout->setSpacing(15);

    searchInput = new QLineEdit();
    searchInput->setObjectName("searchInput");
    searchInput->setPlaceholderText("🔍 Rechercher...");
    searchInput->setFixedHeight(38);
    searchInput->setMinimumWidth(300);
    connect(searchInput, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    etatFilter = new QComboBox();
    etatFilter->setObjectName("filterSelect");
    etatFilter->addItem("Tous les états");
    etatFilter->addItem("Opérationnelle");
    etatFilter->addItem("Maintenance");
    etatFilter->addItem("Hors service");
    etatFilter->addItem("Pleine");
    etatFilter->setFixedHeight(38);
    etatFilter->setMinimumWidth(180);
    connect(etatFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFilterChanged);

    typeFilter = new QComboBox();
    typeFilter->setObjectName("filterSelect");
    typeFilter->addItem("Tous les types");
    typeFilter->addItem("Plastique");
    typeFilter->addItem("Verre");
    typeFilter->addItem("Papier");
    typeFilter->addItem("Organique");
    typeFilter->addItem("Métal");
    typeFilter->addItem("Mixte");
    typeFilter->setFixedHeight(38);
    typeFilter->setMinimumWidth(180);

    filtersLayout->addWidget(searchInput);
    filtersLayout->addWidget(etatFilter);
    filtersLayout->addWidget(typeFilter);
    filtersLayout->addStretch();

    wrapperLayout->addLayout(filtersLayout);

    // Table
    poubelleTable = new QTableWidget();
    poubelleTable->setObjectName("poubelleTable");
    poubelleTable->setColumnCount(8);
    poubelleTable->setHorizontalHeaderLabels({
        "🆔 ID", "📍 Localisation", "🗂️ Type", "📦 Capacité", "📊 Niveau", "⚙️ État", "🏷️ ID Zone", "⚡ Actions"
    });

    // Configuration des colonnes - Utiliser ResizeToContents pour éviter clipping
    poubelleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);  // Mode par défaut
    poubelleTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);  // Niveau
    poubelleTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);  // État
    poubelleTable->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);  // Actions

    // Définir des largeurs minimales initiales
    poubelleTable->setColumnWidth(0, 60);    // ID
    poubelleTable->setColumnWidth(1, 300);   // Localisation
    poubelleTable->setColumnWidth(2, 100);   // Type
    poubelleTable->setColumnWidth(3, 90);    // Capacité
    poubelleTable->setColumnWidth(4, 100);   // Niveau (min)
    poubelleTable->setColumnWidth(5, 150);   // État (min)
    poubelleTable->setColumnWidth(6, 100);   // ID Zone
    poubelleTable->setColumnWidth(7, 220);   // Actions (min)

    poubelleTable->verticalHeader()->setDefaultSectionSize(60);  // Hauteur des lignes augmentée pour éviter clipping vertical
    poubelleTable->verticalHeader()->setVisible(false);
    poubelleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    poubelleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    poubelleTable->setShowGrid(false);
    poubelleTable->setAlternatingRowColors(true);

    // Configuration du scroll et de la taille
    poubelleTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    poubelleTable->setMinimumHeight(300);
    poubelleTable->setMaximumHeight(500);  // Hauteur max pour forcer le scroll
    poubelleTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);  // Toujours visible
    poubelleTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    wrapperLayout->addWidget(poubelleTable, 1);  // Facteur d'étirement 1

    contentLayout->addWidget(contentWrapper);

    // Export PDF Button - placed below the table
    QHBoxLayout *exportLayout = new QHBoxLayout();
    exportLayout->addStretch();

    exportPdfButton = new QPushButton("📄 Exporter PDF");
    exportPdfButton->setObjectName("exportPdfButton");
    exportPdfButton->setFixedHeight(36);
    exportPdfButton->setCursor(Qt::PointingHandCursor);
    connect(exportPdfButton, &QPushButton::clicked, this, &MainWindow::onExportPDF);

    exportLayout->addWidget(exportPdfButton);
    contentLayout->addLayout(exportLayout);

    // Add Chart Widget
    createChartWidget();
    contentLayout->addWidget(chartWidget);
}

void MainWindow::loadPoubelleData()
{
    addTableRow(1, "Avenue Habib Bourguiba, Tunis Centre", 85, "Pleine", "ZONE-001", 240, "Plastique");
    addTableRow(2, "Rue de Marseille, Ariana", 45, "Opérationnelle", "ZONE-002", 120, "Verre");
    addTableRow(3, "Boulevard 7 Novembre, Sfax", 20, "Maintenance", "ZONE-003", 180, "Papier");

    poubelleTable->resizeColumnsToContents();  // Ajuste les colonnes après chargement pour éviter clipping
    updateChartData();
}

void MainWindow::createChartWidget()
{
    chartWidget = new QWidget();
    chartWidget->setObjectName("chartWidget");
    chartWidget->setMinimumHeight(350);

    QVBoxLayout *chartLayout = new QVBoxLayout(chartWidget);
    chartLayout->setContentsMargins(25, 25, 25, 25);
    chartLayout->setSpacing(15);

    // Chart Title
    QLabel *chartTitle = new QLabel("📊 Statistiques des Poubelles");
    chartTitle->setObjectName("chartTitle");
    QFont titleFont = chartTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    chartTitle->setFont(titleFont);
    chartLayout->addWidget(chartTitle);

    // Charts Container
    QHBoxLayout *chartsContainer = new QHBoxLayout();
    chartsContainer->setSpacing(20);

    // État Chart
    QWidget *etatChartWidget = new QWidget();
    etatChartWidget->setObjectName("miniChart");
    etatChartWidget->setProperty("chartType", "etat");
    QVBoxLayout *etatChartLayout = new QVBoxLayout(etatChartWidget);
    etatChartLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *etatChartTitle = new QLabel("État des Poubelles");
    etatChartTitle->setObjectName("miniChartTitle");
    etatChartLayout->addWidget(etatChartTitle);

    etatChartLayout->addStretch();

    // Type Chart
    QWidget *typeChartWidget = new QWidget();
    typeChartWidget->setObjectName("miniChart");
    typeChartWidget->setProperty("chartType", "type");
    QVBoxLayout *typeChartLayout = new QVBoxLayout(typeChartWidget);
    typeChartLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *typeChartTitle = new QLabel("Types de Poubelles");
    typeChartTitle->setObjectName("miniChartTitle");
    typeChartLayout->addWidget(typeChartTitle);

    typeChartLayout->addStretch();

    // Niveau moyen Chart
    QWidget *niveauChartWidget = new QWidget();
    niveauChartWidget->setObjectName("miniChart");
    QVBoxLayout *niveauChartLayout = new QVBoxLayout(niveauChartWidget);
    niveauChartLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *niveauChartTitle = new QLabel("Niveau Moyen");
    niveauChartTitle->setObjectName("miniChartTitle");
    niveauChartLayout->addWidget(niveauChartTitle);

    QLabel *avgNiveau = new QLabel("50%");
    avgNiveau->setObjectName("statValue");
    QFont statFont = avgNiveau->font();
    statFont.setPointSize(36);
    statFont.setBold(true);
    avgNiveau->setFont(statFont);
    avgNiveau->setAlignment(Qt::AlignCenter);
    niveauChartLayout->addWidget(avgNiveau);

    QLabel *niveauLabel = new QLabel("Taux de remplissage moyen");
    niveauLabel->setObjectName("statLabel");
    niveauLabel->setAlignment(Qt::AlignCenter);
    niveauChartLayout->addWidget(niveauLabel);

    niveauChartLayout->addStretch();

    chartsContainer->addWidget(etatChartWidget);
    chartsContainer->addWidget(typeChartWidget);
    chartsContainer->addWidget(niveauChartWidget);

    chartLayout->addLayout(chartsContainer);
}

void MainWindow::updateChartData()
{
    // Count statistics
    int operationnelleCount = 0;
    int maintenanceCount = 0;
    int horsServiceCount = 0;
    int pleineCount = 0;
    QMap<QString, int> typeCount;
    int totalNiveau = 0;

    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        // Count état
        QWidget *etatWidget = poubelleTable->cellWidget(row, 5);
        QLabel *etatLabel = etatWidget ? etatWidget->findChild<QLabel*>() : nullptr;
        if (etatLabel) {
            QString etat = etatLabel->text();
            if (etat == "Opérationnelle") operationnelleCount++;
            else if (etat == "Maintenance") maintenanceCount++;
            else if (etat == "Hors service") horsServiceCount++;
            else if (etat == "Pleine") pleineCount++;
        }

        // Count types
        QString type = poubelleTable->item(row, 2)->text();
        typeCount[type]++;

        // Sum niveau
        QWidget *niveauWidget = poubelleTable->cellWidget(row, 4);
        QLabel *niveauLabel = niveauWidget ? niveauWidget->findChild<QLabel*>() : nullptr;
        if (niveauLabel) {
            QString niveauStr = niveauLabel->text();
            totalNiveau += niveauStr.remove("%").trimmed().toInt();
        }
    }

    int avgNiveau = poubelleTable->rowCount() > 0 ? totalNiveau / poubelleTable->rowCount() : 0;

    // Update état chart
    QList<QWidget*> charts = chartWidget->findChildren<QWidget*>("miniChart");
    for (QWidget *chart : charts) {
        if (chart->property("chartType").toString() == "etat") {
            QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(chart->layout());
            if (layout) {
                while (layout->count() > 2) {
                    QLayoutItem *item = layout->takeAt(1);
                    delete item->widget();
                    delete item;
                }

                addStatBar(layout, "Opérationnelle", operationnelleCount, poubelleTable->rowCount(), "#4CAF50");
                addStatBar(layout, "Pleine", pleineCount, poubelleTable->rowCount(), "#F5A623");
                addStatBar(layout, "Maintenance", maintenanceCount, poubelleTable->rowCount(), "#2196F3");
                addStatBar(layout, "Hors service", horsServiceCount, poubelleTable->rowCount(), "#F44336");

                layout->addStretch();
            }
        } else if (chart->property("chartType").toString() == "type") {
            QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(chart->layout());
            if (layout) {
                while (layout->count() > 2) {
                    QLayoutItem *item = layout->takeAt(1);
                    delete item->widget();
                    delete item;
                }

                QStringList colors = {"#9C27B0", "#00BCD4", "#FF9800", "#8BC34A", "#607D8B", "#E91E63"};
                int colorIndex = 0;
                for (auto it = typeCount.begin(); it != typeCount.end(); ++it) {
                    addStatBar(layout, it.key(), it.value(), poubelleTable->rowCount(),
                               colors[colorIndex % colors.size()]);
                    colorIndex++;
                }

                layout->addStretch();
            }
        }
    }

    // Update niveau moyen
    QLabel *avgNiveauLabel = chartWidget->findChild<QLabel*>("statValue");
    if (avgNiveauLabel) {
        avgNiveauLabel->setText(QString::number(avgNiveau) + "%");

        // Change color based on level
        QString color = "#4CAF50"; // Green
        if (avgNiveau >= 80) color = "#F44336"; // Red
        else if (avgNiveau >= 60) color = "#F5A623"; // Orange

        avgNiveauLabel->setStyleSheet("color: " + color + ";");
    }
}

void MainWindow::addStatBar(QVBoxLayout *layout, const QString &label, int count, int total, const QString &color)
{
    QWidget *barContainer = new QWidget();
    QVBoxLayout *barLayout = new QVBoxLayout(barContainer);
    barLayout->setContentsMargins(0, 5, 0, 5);
    barLayout->setSpacing(5);

    // Label and count
    QHBoxLayout *labelLayout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel(label);
    nameLabel->setObjectName("barLabel");
    QLabel *countLabel = new QLabel(QString::number(count));
    countLabel->setObjectName("barCount");
    labelLayout->addWidget(nameLabel);
    labelLayout->addStretch();
    labelLayout->addWidget(countLabel);
    barLayout->addLayout(labelLayout);

    // Progress bar
    QWidget *barBackground = new QWidget();
    barBackground->setFixedHeight(10);
    barBackground->setStyleSheet("background: #f0f0f0; border-radius: 5px;");

    QWidget *barFill = new QWidget(barBackground);
    int percentage = total > 0 ? (count * 100 / total) : 0;
    barFill->setFixedHeight(10);
    barFill->setFixedWidth(barBackground->width() * percentage / 100);
    barFill->setStyleSheet(QString("background: %1; border-radius: 5px;").arg(color));

    barLayout->addWidget(barBackground);

    layout->insertWidget(layout->count() - 1, barContainer);
}

void MainWindow::addTableRow(int id, const QString &localisation, int niveau,
                             const QString &etat, const QString &idZone,
                             int capacite, const QString &type)
{
    int row = poubelleTable->rowCount();
    poubelleTable->insertRow(row);

    poubelleTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
    poubelleTable->setItem(row, 1, new QTableWidgetItem(localisation));
    poubelleTable->setItem(row, 2, new QTableWidgetItem(type));
    poubelleTable->setItem(row, 3, new QTableWidgetItem(QString::number(capacite) + " L"));
    poubelleTable->setItem(row, 6, new QTableWidgetItem(idZone));

    // ========== COLONNE NIVEAU (Colonne 4) ==========
    QWidget *niveauWidget = new QWidget();
    niveauWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *niveauLayout = new QHBoxLayout(niveauWidget);
    niveauLayout->setContentsMargins(5, 5, 5, 5);  // Ajout de margins pour espace

    QLabel *niveauBadge = new QLabel(QString::number(niveau) + "%");
    niveauBadge->setAlignment(Qt::AlignCenter);
    niveauBadge->setStyleSheet(getNiveauStyle(niveau) + " min-width: 80px;");  // Min-width pour éviter clipping
    niveauBadge->setFixedHeight(26);

    niveauLayout->addStretch();
    niveauLayout->addWidget(niveauBadge);
    niveauLayout->addStretch();

    poubelleTable->setCellWidget(row, 4, niveauWidget);

    // ========== COLONNE ÉTAT (Colonne 5) ==========
    QWidget *etatWidget = new QWidget();
    etatWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *etatLayout = new QHBoxLayout(etatWidget);
    etatLayout->setContentsMargins(5, 5, 5, 5);

    QLabel *etatBadge = new QLabel(etat);
    etatBadge->setAlignment(Qt::AlignCenter);
    etatBadge->setWordWrap(true);  // Permet wrap pour mots longs
    etatBadge->setStyleSheet(getEtatStyle(etat) + " min-width: 150px; padding: 8px;");  // Min-width + padding
    etatBadge->setFixedHeight(26);

    etatLayout->addStretch();
    etatLayout->addWidget(etatBadge);
    etatLayout->addStretch();

    poubelleTable->setCellWidget(row, 5, etatWidget);

    // ========== COLONNE ACTIONS (Colonne 7) ==========
    QWidget *actionsWidget = new QWidget();
    actionsWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setContentsMargins(5, 0, 5, 0);
    actionsLayout->setSpacing(10);  // Espace entre boutons

    // Bouton Modifier
    QPushButton *modifyBtn = new QPushButton();
    modifyBtn->setFixedSize(40, 40);  // Taille augmentée pour visibilité
    modifyBtn->setCursor(Qt::PointingHandCursor);
    modifyBtn->setToolTip("Modifier cette poubelle");
    QIcon modifyIcon(":/icons/modify.png");  // Utilisez si ressources.qrc configuré
    if (!modifyIcon.isNull()) {
        modifyBtn->setIcon(modifyIcon);
        modifyBtn->setIconSize(QSize(24, 24));
    } else {
        modifyBtn->setText("✏️");  // Fallback emoji
    }
    modifyBtn->setStyleSheet("background: #E8F5E9; border-radius: 20px;");  // Style visible (vert clair)
    connect(modifyBtn, &QPushButton::clicked, [this, row]() { onModifyPoubelle(row); });

    // Bouton Supprimer
    QPushButton *deleteBtn = new QPushButton();
    deleteBtn->setFixedSize(40, 40);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setToolTip("Supprimer cette poubelle");
    QIcon deleteIcon(":/icons/delete.png");
    if (!deleteIcon.isNull()) {
        deleteBtn->setIcon(deleteIcon);
        deleteBtn->setIconSize(QSize(24, 24));
    } else {
        deleteBtn->setText("🗑️");  // Fallback emoji
    }
    deleteBtn->setStyleSheet("background: #FFEBEE; border-radius: 20px;");  // Style visible (rouge clair)
    connect(deleteBtn, &QPushButton::clicked, [this, row]() { onDeletePoubelle(row); });

    actionsLayout->addStretch();
    actionsLayout->addWidget(modifyBtn);
    actionsLayout->addWidget(deleteBtn);
    actionsLayout->addStretch();

    poubelleTable->setCellWidget(row, 7, actionsWidget);

    // Définir une hauteur de ligne optimale
    poubelleTable->setRowHeight(row, 60);  // Augmentée pour espace
}

QString MainWindow::getEtatStyle(const QString &etat)
{
    QString style = "QLabel { "
                    "background-color: %1; "
                    "color: white; "
                    "border-radius: 8px; "
                    "font-weight: bold; "
                    "font-size: 10px; "
                    "}";

    if (etat == "Opérationnelle") {
        return style.arg("#4CAF50");
    } else if (etat == "Maintenance") {
        return style.arg("#2196F3");
    } else if (etat == "Hors service") {
        return style.arg("#F44336");
    } else if (etat == "Pleine") {
        return style.arg("#FF9800");
    }
    return "";
}

QString MainWindow::getNiveauStyle(int niveau)
{
    QString bgColor;

    if (niveau >= 80) {
        bgColor = "#F44336";  // Rouge
    } else if (niveau >= 60) {
        bgColor = "#FF9800";  // Orange
    } else if (niveau >= 40) {
        bgColor = "#2196F3";  // Bleu
    } else {
        bgColor = "#4CAF50";  // Vert
    }

    return QString("QLabel { "
                   "background-color: %1; "
                   "color: white; "
                   "border-radius: 8px; "
                   "font-weight: bold; "
                   "font-size: 10px; "
                   "}")
        .arg(bgColor);
}

void MainWindow::clearFormInputs()
{
    localisationInput->clear();
    typeCombo->setCurrentIndex(0);
    capaciteInput->setValue(120);
    niveauInput->setValue(0);
    etatCombo->setCurrentIndex(0);
    idZoneInput->clear();
    currentEditingRow = -1;
    formTitleLabel->setText("📝 Ajouter une poubelle");
    saveButton->setText("💾 Enregistrer");
}

void MainWindow::setFormForEditing(int row)
{
    if (row < 0 || row >= poubelleTable->rowCount()) return;

    currentEditingRow = row;
    QString id = poubelleTable->item(row, 0)->text();

    localisationInput->setText(poubelleTable->item(row, 1)->text());
    typeCombo->setCurrentText(poubelleTable->item(row, 2)->text());

    QString capaciteStr = poubelleTable->item(row, 3)->text();
    capaciteInput->setValue(capaciteStr.remove(" L").toInt());

    QWidget *niveauWidget = poubelleTable->cellWidget(row, 4);
    QLabel *niveauLabel = niveauWidget ? niveauWidget->findChild<QLabel*>() : nullptr;
    if (niveauLabel) {
        QString niveauStr = niveauLabel->text();
        niveauInput->setValue(niveauStr.remove("%").trimmed().toInt());
    }

    QWidget *etatWidget = poubelleTable->cellWidget(row, 5);
    QLabel *etatLabel = etatWidget ? etatWidget->findChild<QLabel*>() : nullptr;
    if (etatLabel) {
        etatCombo->setCurrentText(etatLabel->text());
    }

    idZoneInput->setText(poubelleTable->item(row, 6)->text());

    formTitleLabel->setText("✏️ Modifier la poubelle #" + id);
    saveButton->setText("💾 Mettre à jour");
}

void MainWindow::applyStyles()
{
    QString styleSheet = R"(
        QMainWindow {
            background: #F7F7F7;
        }
        #sidebar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #7FB069, stop:1 #6FA055);
        }
        #logoTitle {
            color: white;
            font-size: 20px;
            font-weight: bold;
            letter-spacing: 1px;
        }
        #menuItem {
            background: transparent;
            color: rgba(255, 255, 255, 0.85);
            border: none;
            text-align: left;
            padding-left: 30px;
            font-size: 13px;
            font-weight: 500;
        }
        #menuItem:hover {
            background: rgba(255, 255, 255, 0.15);
            color: white;
        }
        #menuItem[active="true"] {
            background: rgba(163, 198, 81, 0.35);
            color: white;
            border-left: 4px solid #9BCB4E;
            font-weight: bold;
        }
        #userName {
            color: white;
            font-size: 13px;
            font-weight: 600;
        }
        #formPanel {
            background: white;
            border-right: 1px solid #E5E5E5;
        }
        #formTitle {
            color: #2C3E50;
            padding-bottom: 5px;
        }
        #formLabel {
            color: #2C3E50;
            font-weight: 700;
            font-size: 12px;
            margin-bottom: 8px;
            letter-spacing: 0.3px;
        }
        #formInput {
            border: 2px solid #E0E0E0;
            border-radius: 12px;
            padding: 10px 14px;
            font-size: 13px;
            background: #FAFAFA;
            color: #2C3E50;
            font-weight: 500;
        }
        #formInput:hover {
            border-color: #9BCB4E;
            background: white;
        }
        #formInput:focus {
            border-color: #9BCB4E;
            background: white;
            outline: none;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            width: 20px;
            border-radius: 4px;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background: #9BCB4E;
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #666;
            margin-right: 10px;
        }
        #saveButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #9BCB4E, stop:1 #8AB83E);
            color: white;
            border: none;
            border-radius: 12px;
            font-size: 14px;
            font-weight: bold;
            letter-spacing: 0.5px;
        }
        #saveButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #B7D97A, stop:1 #9BCB4E);
        }
        #saveButton:pressed {
            background: #8AB83E;
        }
        #infoBox {
            background: #E8F4F8;
            color: #1565C0;
            padding: 16px;
            border-radius: 12px;
            border-left: 5px solid #2196F3;
            font-size: 13px;
            line-height: 1.6;
        }
        #mainContent {
            background: #F7F7F7;
        }
        #header {
            background: white;
            border-radius: 14px;
            box-shadow: 0 2px 8px rgba(0,0,0,0.04);
        }
        #breadcrumb {
            color: #7F8C8D;
            font-size: 14px;
            font-weight: 500;
        }
        #headerBtn {
            background: #F5F5F5;
            border: none;
            border-radius: 12px;
            font-size: 18px;
        }
        #headerBtn:hover {
            background: #9BCB4E;
            color: white;
        }
        #contentWrapper {
            background: white;
            border-radius: 16px;
            box-shadow: 0 2px 12px rgba(0,0,0,0.06);
        }
        #pageTitle {
            font-size: 22px;
            font-weight: 800;
            color: #2C3E50;
            letter-spacing: -0.5px;
        }
        #searchInput {
            border: 2px solid #E0E0E0;
            border-radius: 12px;
            padding: 0 16px;
            font-size: 14px;
            color: #2C3E50;
            background: #FAFAFA;
        }
        #searchInput:focus {
            border-color: #9BCB4E;
            background: white;
        }
        #filterSelect {
            border: 2px solid #E0E0E0;
            border-radius: 12px;
            padding: 0 16px;
            font-size: 14px;
            background: #FAFAFA;
            color: #2C3E50;
            font-weight: 500;
        }
        #filterSelect:hover {
            border-color: #9BCB4E;
            background: white;
        }
        #poubelleTable {
            background: white;
            border: none;
            gridline-color: #F0F0F0;
            color: #2C3E50;
            font-size: 12px;
        }
        #poubelleTable::item {
            padding: 12px 10px;
            border-bottom: 1px solid #F0F0F0;
        }
        #poubelleTable::item:selected {
            background: #F8FFF9;
            color: #2C3E50;
        }
        #poubelleTable::item:alternate {
            background: #FAFAFA;
        }
        QHeaderView::section {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #F8F9FA, stop:1 #F0F1F2);
            padding: 10px 10px;
            border: none;
            border-bottom: 2px solid #E0E0E0;
            border-right: 1px solid #EEEEEE;
            font-weight: 700;
            font-size: 11px;
            color: #2C3E50;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }
        QHeaderView::section:first {
            border-top-left-radius: 10px;
        }
        QHeaderView::section:last {
            border-top-right-radius: 10px;
            border-right: none;
        }
        QPushButton#modifyButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 5px;
            font-size: 9px;
            font-weight: bold;
        }
        QPushButton#modifyButton:hover {
            background-color: #45A049;
        }
        QPushButton#deleteButton {
            background-color: #F44336;
            color: white;
            border: none;
            border-radius: 5px;
            font-size: 9px;
            font-weight: bold;
        }
        QPushButton#deleteButton:hover {
            background-color: #E53935;
        }
        #paginationInfo {
            color: #7F8C8D;
            font-size: 14px;
            font-weight: 500;
        }
        #pageButton {
            border: 2px solid #E0E0E0;
            background: white;
            border-radius: 10px;
            font-weight: 600;
            color: #2C3E50;
        }
        #pageButton:hover {
            background: #9BCB4E;
            color: white;
            border-color: #9BCB4E;
        }
        #pageButton[active="true"] {
            background: #9BCB4E;
            color: white;
            border-color: #9BCB4E;
            font-weight: bold;
        }
        #itemsPerPage {
            border: 2px solid #E0E0E0;
            border-radius: 10px;
            padding: 0 12px;
            background: white;
            font-weight: 500;
        }
        #exportPdfButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #E74C3C, stop:1 #C0392B);
            color: white;
            border: none;
            border-radius: 10px;
            padding: 0 20px;
            font-size: 13px;
            font-weight: bold;
        }
        #exportPdfButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #EC7063, stop:1 #E74C3C);
        }
        #chartWidget {
            background: white;
            border-radius: 16px;
            box-shadow: 0 2px 12px rgba(0,0,0,0.06);
        }
        #chartTitle {
            color: #2C3E50;
            font-weight: 800;
        }
        #miniChart {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #FAFAFA, stop:1 #F5F5F5);
            border-radius: 14px;
            border: 2px solid #E8E8E8;
        }
        #miniChartTitle {
            color: #2C3E50;
            font-size: 14px;
            font-weight: 700;
            margin-bottom: 15px;
        }
        #statValue {
            color: #9BCB4E;
            font-weight: 900;
        }
        #statLabel {
            color: #7F8C8D;
            font-size: 13px;
            font-weight: 500;
        }
        #barLabel {
            color: #2C3E50;
            font-size: 12px;
            font-weight: 600;
        }
        #barCount {
            color: #2C3E50;
            font-weight: 700;
            font-size: 12px;
        }
        // Styles supplémentaires pour boutons actions
        QPushButton {
            border: none;
            border-radius: 20px;
            min-width: 40px;
            min-height: 40px;
        }
        QPushButton:hover {
            opacity: 0.8;
        }
    )";

    setStyleSheet(styleSheet);
}

// ==================== Slots Implementation ====================
void MainWindow::onAddPoubelle()
{
    QString localisation = localisationInput->text();
    QString idZone = idZoneInput->text();

    if (localisation.isEmpty() || idZone.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs obligatoires");
        return;
    }

    QString type = typeCombo->currentText();
    // Enlever les emojis du type
    type = type.split(" ").last();
    int capacite = capaciteInput->value();
    int niveau = niveauInput->value();
    QString etat = etatCombo->currentText();
    // Enlever les emojis de l'état
    etat = etat.split(" ").last();

    if (currentEditingRow >= 0) {
        // Mode édition
        QString id = poubelleTable->item(currentEditingRow, 0)->text();

        poubelleTable->item(currentEditingRow, 1)->setText(localisation);
        poubelleTable->item(currentEditingRow, 2)->setText(type);
        poubelleTable->item(currentEditingRow, 3)->setText(QString::number(capacite) + " L");
        poubelleTable->item(currentEditingRow, 6)->setText(idZone);

        // Update niveau
        QWidget *niveauWidget = poubelleTable->cellWidget(currentEditingRow, 4);
        QLabel *niveauLabel = niveauWidget ? niveauWidget->findChild<QLabel*>() : nullptr;
        if (niveauLabel) {
            niveauLabel->setText(QString::number(niveau) + "%");
            niveauLabel->setStyleSheet(getNiveauStyle(niveau) + " min-width: 80px;");
        }

        // Update état
        QWidget *etatWidget = poubelleTable->cellWidget(currentEditingRow, 5);
        QLabel *etatLabel = etatWidget ? etatWidget->findChild<QLabel*>() : nullptr;
        if (etatLabel) {
            etatLabel->setText(etat);
            etatLabel->setStyleSheet(getEtatStyle(etat) + " min-width: 150px; padding: 8px;");
        }

        QMessageBox::information(this, "Succès", "Poubelle #" + id + " modifiée avec succès!");
        clearFormInputs();
        poubelleTable->resizeColumnsToContents();  // Réajuste après mise à jour
        updateChartData();
    } else {
        // Mode ajout
        addTableRow(nextId++, localisation, niveau, etat, idZone, capacite, type);

        // Scroller automatiquement vers la nouvelle ligne
        poubelleTable->scrollToBottom();

        QMessageBox::information(this, "Succès", "Poubelle ajoutée avec succès!");
        clearFormInputs();
        poubelleTable->resizeColumnsToContents();  // Réajuste après ajout
        updateChartData();
    }
}

void MainWindow::onModifyPoubelle(int row)
{
    setFormForEditing(row);
}

void MainWindow::onDeletePoubelle(int row)
{
    if (row < 0 || row >= poubelleTable->rowCount()) return;

    QString id = poubelleTable->item(row, 0)->text();
    QString localisation = poubelleTable->item(row, 1)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmer la suppression",
                                  "Voulez-vous vraiment supprimer la poubelle #" + id + " (" + localisation + ") ?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (currentEditingRow == row) {
            clearFormInputs();
        }

        poubelleTable->removeRow(row);
        poubelleTable->resizeColumnsToContents();  // Réajuste après suppression
        updateChartData();
        QMessageBox::information(this, "Succès", "Poubelle supprimée avec succès!");
    }
}

void MainWindow::onExportPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Exporter le Rapport",
                                                    QDir::homePath() + "/TuniWaste_Poubelles_" + QDate::currentDate().toString("yyyy-MM-dd") + ".html",
                                                    "Fichiers HTML (*.html);;Tous les fichiers (*)");

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".html", Qt::CaseInsensitive)) {
        fileName += ".html";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier!");
        return;
    }

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#else
    out.setEncoding(QStringConverter::Utf8);
#endif

    // Calculate statistics
    int operationnelleCount = 0;
    int pleineCount = 0;
    int totalNiveau = 0;

    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        QWidget *etatWidget = poubelleTable->cellWidget(row, 5);
        QLabel *etatLabel = etatWidget ? etatWidget->findChild<QLabel*>() : nullptr;
        if (etatLabel) {
            QString etat = etatLabel->text();
            if (etat == "Opérationnelle") operationnelleCount++;
            else if (etat == "Pleine") pleineCount++;
        }

        QWidget *niveauWidget = poubelleTable->cellWidget(row, 4);
        QLabel *niveauLabel = niveauWidget ? niveauWidget->findChild<QLabel*>() : nullptr;
        if (niveauLabel) {
            QString niveauStr = niveauLabel->text();
            totalNiveau += niveauStr.remove("%").trimmed().toInt();
        }
    }

    int avgNiveau = poubelleTable->rowCount() > 0 ? totalNiveau / poubelleTable->rowCount() : 0;

    // Generate HTML
    out << "<!DOCTYPE html>\n";
    out << "<html>\n<head>\n";
    out << "<meta charset='UTF-8'>\n";
    out << "<title>TuniWaste - Rapport des Poubelles</title>\n";
    out << "<style>\n";
    out << "body { font-family: Arial, sans-serif; margin: 40px; background: #F7F7F7; }\n";
    out << ".container { max-width: 1200px; margin: 0 auto; background: white; padding: 40px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }\n";
    out << "h1 { color: #34495e; text-align: center; font-size: 36px; margin-bottom: 10px; }\n";
    out << "h2 { color: #34495e; font-size: 24px; margin-top: 30px; border-bottom: 3px solid #9BCB4E; padding-bottom: 10px; }\n";
    out << ".subtitle { text-align: center; color: #666; margin-bottom: 10px; }\n";
    out << ".date { text-align: center; color: #999; font-size: 14px; margin-bottom: 40px; }\n";
    out << ".stats { display: flex; justify-content: space-around; margin: 30px 0; }\n";
    out << ".stat-box { flex: 1; margin: 0 10px; padding: 20px; border-radius: 10px; text-align: center; }\n";
    out << ".stat-box.total { background: #E8F5E9; border: 2px solid #4CAF50; }\n";
    out << ".stat-box.active { background: #E3F2FD; border: 2px solid #4DA3FF; }\n";
    out << ".stat-box.niveau { background: #FFF3E0; border: 2px solid #F5A623; }\n";
    out << ".stat-title { font-size: 14px; color: #666; margin-bottom: 10px; }\n";
    out << ".stat-value { font-size: 48px; font-weight: bold; margin: 10px 0; }\n";
    out << ".stat-box.total .stat-value { color: #4CAF50; }\n";
    out << ".stat-box.active .stat-value { color: #4DA3FF; }\n";
    out << ".stat-box.niveau .stat-value { color: #F5A623; }\n";
    out << "table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n";
    out << "th { background: #34495e; color: white; padding: 15px; text-align: left; font-weight: bold; }\n";
    out << "td { padding: 12px 15px; border-bottom: 1px solid #eee; }\n";
    out << "tr:nth-child(even) { background: #f9f9f9; }\n";
    out << "tr:hover { background: #f0f0f0; }\n";
    out << ".status { padding: 6px 12px; border-radius: 15px; font-weight: bold; font-size: 12px; display: inline-block; }\n";
    out << ".status-operationnelle { background: #E8F5E9; color: #4CAF50; }\n";
    out << ".status-maintenance { background: #E3F2FD; color: #2196F3; }\n";
    out << ".status-hors-service { background: #FFEBEE; color: #F44336; }\n";
    out << ".status-pleine { background: #FFF3E0; color: #F5A623; }\n";
    out << ".footer { text-align: center; color: #999; margin-top: 50px; padding-top: 20px; border-top: 1px solid #eee; font-size: 12px; }\n";
    out << "@media print { body { margin: 0; background: white; } .container { box-shadow: none; } }\n";
    out << "</style>\n";
    out << "</head>\n<body>\n";
    out << "<div class='container'>\n";

    // Header
    out << "<h1>🗑️ TuniWaste</h1>\n";
    out << "<div class='subtitle'>Rapport de Gestion des Poubelles</div>\n";
    out << "<div class='date'>Généré le " << QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm") << "</div>\n";

    // Statistics
    out << "<h2>📊 Statistiques</h2>\n";
    out << "<div class='stats'>\n";
    out << "<div class='stat-box total'>\n";
    out << "<div class='stat-title'>Total Poubelles</div>\n";
    out << "<div class='stat-value'>" << poubelleTable->rowCount() << "</div>\n";
    out << "</div>\n";
    out << "<div class='stat-box active'>\n";
    out << "<div class='stat-title'>Opérationnelles</div>\n";
    out << "<div class='stat-value'>" << operationnelleCount << "</div>\n";
    out << "</div>\n";
    out << "<div class='stat-box niveau'>\n";
    out << "<div class='stat-title'>Niveau Moyen</div>\n";
    out << "<div class='stat-value'>" << avgNiveau << "%</div>\n";
    out << "</div>\n";
    out << "</div>\n";

    // Table
    out << "<h2>📋 Liste des Poubelles</h2>\n";
    out << "<table>\n";
    out << "<thead>\n<tr>\n";
    out << "<th>ID</th><th>Localisation</th><th>Type</th><th>Capacité</th><th>Niveau</th><th>État</th><th>Zone</th>\n";
    out << "</tr>\n</thead>\n<tbody>\n";

    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        out << "<tr>\n";
        out << "<td>" << poubelleTable->item(row, 0)->text() << "</td>\n";
        out << "<td>" << poubelleTable->item(row, 1)->text() << "</td>\n";
        out << "<td>" << poubelleTable->item(row, 2)->text() << "</td>\n";
        out << "<td>" << poubelleTable->item(row, 3)->text() << "</td>\n";

        QWidget *niveauWidget = poubelleTable->cellWidget(row, 4);
        QLabel *niveauLabel = niveauWidget ? niveauWidget->findChild<QLabel*>() : nullptr;
        if (niveauLabel) {
            out << "<td>" << niveauLabel->text() << "</td>\n";
        }

        QWidget *etatWidget = poubelleTable->cellWidget(row, 5);
        QLabel *etatLabel = etatWidget ? etatWidget->findChild<QLabel*>() : nullptr;
        if (etatLabel) {
            QString etat = etatLabel->text();
            QString statusClass = "status-operationnelle";
            if (etat == "Maintenance") statusClass = "status-maintenance";
            else if (etat == "Hors service") statusClass = "status-hors-service";
            else if (etat == "Pleine") statusClass = "status-pleine";
            out << "<td><span class='status " << statusClass << "'>" << etat << "</span></td>\n";
        }

        out << "<td>" << poubelleTable->item(row, 6)->text() << "</td>\n";
        out << "</tr>\n";
    }

    out << "</tbody>\n</table>\n";

    // Footer
    out << "<div class='footer'>\n";
    out << "TuniWaste © " << QDate::currentDate().year() << " - Gestion des déchets en Tunisie\n";
    out << "</div>\n";

    out << "</div>\n</body>\n</html>";

    file.close();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Succès",
                                  "Le fichier HTML a été créé avec succès!\n\n"
                                  "📂 Emplacement: " + fileName + "\n\n"
                                                   "Voulez-vous l'ouvrir maintenant?\n\n"
                                                   "💡 Astuce: Vous pouvez l'imprimer en PDF depuis votre navigateur (Ctrl+P → Enregistrer en PDF)",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}

void MainWindow::onMenuItemClicked()
{
    for (QPushButton *btn : menuButtons) {
        btn->setProperty("active", false);
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }

    QPushButton *clickedBtn = qobject_cast<QPushButton*>(sender());
    if (clickedBtn) {
        clickedBtn->setProperty("active", true);
        clickedBtn->style()->unpolish(clickedBtn);
        clickedBtn->style()->polish(clickedBtn);
    }
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < 7; ++col) {
            QTableWidgetItem *item = poubelleTable->item(row, col);
            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        poubelleTable->setRowHidden(row, !match);
    }
}

void MainWindow::onFilterChanged(int index)
{
    if (index == 0) {
        for (int row = 0; row < poubelleTable->rowCount(); ++row) {
            poubelleTable->setRowHidden(row, false);
        }
        return;
    }

    QString selectedEtat = etatFilter->currentText();

    for (int row = 0; row < poubelleTable->rowCount(); ++row) {
        QWidget *etatWidget = poubelleTable->cellWidget(row, 5);
        QLabel *etatLabel = etatWidget ? etatWidget->findChild<QLabel*>() : nullptr;
        QString etat = etatLabel ? etatLabel->text() : "";

        poubelleTable->setRowHidden(row, etat != selectedEtat);
    }
}

// ==================== User Integration Methods ====================
// TODO: Ces méthodes seront appelées par le module de gestion des utilisateurs

void MainWindow::setCurrentUser(const QString &name, const QString &avatarPath)
{
    // Mettre à jour le nom de l'utilisateur
    if (userNameLabel) {
        userNameLabel->setText(name);
    }

    // Charger et afficher l'avatar si un chemin est fourni
    if (userAvatar && !avatarPath.isEmpty()) {
        QPixmap avatar(avatarPath);
        if (!avatar.isNull()) {
            // Créer un avatar circulaire
            QPixmap rounded(50, 50);
            rounded.fill(Qt::transparent);

            QPainter painter(&rounded);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(QBrush(avatar.scaled(50, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(0, 0, 50, 50);

            userAvatar->setPixmap(rounded);
            userAvatar->setStyleSheet("border-radius: 25px;");
        }
    }
}

void MainWindow::updateUserProfile(const QString &name, const QString &role)
{
    if (userNameLabel) {
        QString displayText = name;
        if (!role.isEmpty()) {
            displayText = name + "\n" + role;
        }
        userNameLabel->setText(displayText);
    }
}
