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
    loadTruckData();
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

    setWindowTitle("TuniWaste - Gestion des camions");
    resize(1600, 900);
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
        "logo.png",                    // Current directory
        "./logo.png",                  // Current directory (explicit)
        "../logo.png",                 // Parent directory
        "../../logo.png",              // Two levels up
        QCoreApplication::applicationDirPath() + "/logo.png"  // Executable directory
    };

    for (const QString &path : logoPaths) {
        logo = QPixmap(path);
        if (!logo.isNull()) {
            break;  // Logo found, stop searching
        }
    }

    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoLabel->setScaledContents(false);
    } else {
        // Fallback: colored circle if logo not found
        logoLabel->setStyleSheet("background: #A3C651; border-radius: 40px;");
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
        "📍 Gestion des routes",
        "📊 Suivi des collectes",
        "📄 Rapports",
        "⚙️ Paramètres"
    };

    for (int i = 0; i < menuItems.size(); ++i) {
        QPushButton *menuBtn = new QPushButton(menuItems[i]);
        menuBtn->setObjectName("menuItem");
        menuBtn->setCursor(Qt::PointingHandCursor);
        menuBtn->setFixedHeight(50);

        if (i == 2) {
            menuBtn->setProperty("active", true);
        }

        connect(menuBtn, &QPushButton::clicked, this, &MainWindow::onMenuItemClicked);

        menuButtons.append(menuBtn);
        sidebarLayout->addWidget(menuBtn);
    }

    sidebarLayout->addStretch();

    // User Profile
    QWidget *userWidget = new QWidget();
    userWidget->setObjectName("userProfile");
    userWidget->setStyleSheet("border-top: 1px solid rgba(255, 255, 255, 0.2);");

    QHBoxLayout *userLayout = new QHBoxLayout(userWidget);
    userLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *userAvatar = new QLabel();
    userAvatar->setFixedSize(50, 50);
    userAvatar->setStyleSheet("border-radius: 25px; background: #ddd;");

    QLabel *userName = new QLabel("Ahmed");
    userName->setObjectName("userName");

    userLayout->addWidget(userAvatar);
    userLayout->addWidget(userName);
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
    formTitleLabel = new QLabel("📝 Ajouter un camion");
    formTitleLabel->setObjectName("formTitle");
    QFont titleFont = formTitleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    formTitleLabel->setFont(titleFont);
    formLayout->addWidget(formTitleLabel);

    // Separator
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("background: #ddd; max-height: 1px;");
    formLayout->addWidget(separator);

    formLayout->addSpacing(10);

    // Form Fields
    QFormLayout *fieldsLayout = new QFormLayout();
    fieldsLayout->setSpacing(20);
    fieldsLayout->setContentsMargins(0, 0, 0, 0);
    fieldsLayout->setLabelAlignment(Qt::AlignLeft);

    // Type
    QLabel *typeLabel = new QLabel("Type de camion:");
    typeLabel->setObjectName("formLabel");
    typeInput = new QLineEdit();
    typeInput->setObjectName("formInput");
    typeInput->setPlaceholderText("Ex: Compact, Grand, Électrique");
    typeInput->setFixedHeight(45);
    fieldsLayout->addRow(typeLabel, typeInput);

    // Capacity
    QLabel *capacityLabel = new QLabel("Capacité:");
    capacityLabel->setObjectName("formLabel");
    capacityInput = new QSpinBox();
    capacityInput->setObjectName("formInput");
    capacityInput->setSuffix(" T");
    capacityInput->setMinimum(1);
    capacityInput->setMaximum(50);
    capacityInput->setValue(5);
    capacityInput->setFixedHeight(45);
    fieldsLayout->addRow(capacityLabel, capacityInput);

    // Status
    QLabel *statusLabel = new QLabel("Statut:");
    statusLabel->setObjectName("formLabel");
    statusCombo = new QComboBox();
    statusCombo->setObjectName("formInput");
    statusCombo->addItem("Actif");
    statusCombo->addItem("Maintenance");
    statusCombo->addItem("En panne");
    statusCombo->setFixedHeight(45);
    fieldsLayout->addRow(statusLabel, statusCombo);

    // Location
    QLabel *locationLabel = new QLabel("Localisation:");
    locationLabel->setObjectName("formLabel");
    locationInput = new QLineEdit();
    locationInput->setObjectName("formInput");
    locationInput->setPlaceholderText("Ex: Tunis, Ariana, Sfax");
    locationInput->setFixedHeight(45);
    fieldsLayout->addRow(locationLabel, locationInput);

    formLayout->addLayout(fieldsLayout);
    formLayout->addSpacing(20);

    // Save Button (full width)
    saveButton = new QPushButton("💾 Enregistrer");
    saveButton->setObjectName("saveButton");
    saveButton->setFixedHeight(50);
    saveButton->setCursor(Qt::PointingHandCursor);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onAddTruck);

    formLayout->addWidget(saveButton);
    formLayout->addStretch();

    // Info Box
    QLabel *infoBox = new QLabel(
        "💡 <b>Astuce:</b><br>"
        "Cliquez sur 'Modifier' dans le tableau pour éditer un camion existant."
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
    contentLayout->setContentsMargins(30, 30, 30, 30);
    contentLayout->setSpacing(20);

    // Header
    QWidget *headerWidget = new QWidget();
    headerWidget->setObjectName("header");
    headerWidget->setFixedHeight(60);

    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 10, 20, 10);

    QLabel *breadcrumb = new QLabel("Tableau de bord / Gestion des camions");
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
    wrapperLayout->setContentsMargins(25, 25, 25, 25);
    wrapperLayout->setSpacing(20);

    // Page Title
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *pageTitle = new QLabel("Liste des camions");
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
    searchInput->setFixedHeight(45);
    searchInput->setMinimumWidth(300);
    connect(searchInput, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    statusFilter = new QComboBox();
    statusFilter->setObjectName("filterSelect");
    statusFilter->addItem("Tous les statuts");
    statusFilter->addItem("Actif");
    statusFilter->addItem("Maintenance");
    statusFilter->addItem("En panne");
    statusFilter->setFixedHeight(45);
    statusFilter->setMinimumWidth(180);
    connect(statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFilterChanged);

    typeFilter = new QComboBox();
    typeFilter->setObjectName("filterSelect");
    typeFilter->addItem("Tous les types");
    typeFilter->addItem("Compact");
    typeFilter->addItem("Grand");
    typeFilter->addItem("Électrique");
    typeFilter->setFixedHeight(45);
    typeFilter->setMinimumWidth(180);

    filtersLayout->addWidget(searchInput);
    filtersLayout->addWidget(statusFilter);
    filtersLayout->addWidget(typeFilter);
    filtersLayout->addStretch();

    wrapperLayout->addLayout(filtersLayout);

    // Table
    truckTable = new QTableWidget();
    truckTable->setObjectName("truckTable");
    truckTable->setColumnCount(6);
    truckTable->setHorizontalHeaderLabels({"ID", "Type", "Capacité", "Statut", "Localisation", "Actions"});

    // Set column widths for better proportions
    truckTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    truckTable->setColumnWidth(0, 50);   // ID - plus petit
    truckTable->setColumnWidth(1, 150);  // Type
    truckTable->setColumnWidth(2, 100);  // Capacité
    truckTable->setColumnWidth(3, 120);  // Statut
    truckTable->setColumnWidth(4, 150);  // Localisation
    truckTable->setColumnWidth(5, 180);  // Actions - réduit de 200 à 180

    // Allow last section to stretch to fill remaining space
    truckTable->horizontalHeader()->setStretchLastSection(false);

    truckTable->verticalHeader()->setVisible(false);
    truckTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    truckTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    truckTable->setShowGrid(false);

    wrapperLayout->addWidget(truckTable);

    // Pagination
    QHBoxLayout *paginationLayout = new QHBoxLayout();

    QLabel *paginationInfo = new QLabel("Affichage 1-3 sur 3 résultats");
    paginationInfo->setObjectName("paginationInfo");

    QHBoxLayout *pageButtonsLayout = new QHBoxLayout();
    pageButtonsLayout->setSpacing(8);

    for (int i = 1; i <= 3; ++i) {
        QPushButton *pageBtn = new QPushButton(QString::number(i));
        pageBtn->setObjectName("pageButton");
        pageBtn->setFixedSize(40, 40);
        if (i == 1) pageBtn->setProperty("active", true);
        pageButtonsLayout->addWidget(pageBtn);
    }

    QComboBox *itemsPerPage = new QComboBox();
    itemsPerPage->setObjectName("itemsPerPage");
    itemsPerPage->addItems({"10 par page", "25 par page", "50 par page"});
    itemsPerPage->setFixedHeight(40);

    // Export PDF Button
    exportPdfButton = new QPushButton("📄 Exporter PDF");
    exportPdfButton->setObjectName("exportPdfButton");
    exportPdfButton->setFixedHeight(40);
    exportPdfButton->setMinimumWidth(150);  // Largeur minimum pour être bien visible
    exportPdfButton->setCursor(Qt::PointingHandCursor);
    connect(exportPdfButton, &QPushButton::clicked, this, &MainWindow::onExportPDF);

    paginationLayout->addWidget(paginationInfo);
    paginationLayout->addStretch();
    paginationLayout->addLayout(pageButtonsLayout);
    paginationLayout->addWidget(itemsPerPage);
    paginationLayout->addSpacing(15);  // Espace avant le bouton Export
    paginationLayout->addWidget(exportPdfButton);

    wrapperLayout->addLayout(paginationLayout);

    contentLayout->addWidget(contentWrapper);

    // Add Chart Widget
    createChartWidget();
    contentLayout->addWidget(chartWidget);
}

void MainWindow::loadTruckData()
{
    addTableRow(1, "Compact", "5 T", "Actif", "Tunis");
    addTableRow(2, "Grand", "10 T", "Maintenance", "Ariana");
    addTableRow(3, "Électrique", "7 T", "En panne", "Sfax");
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
    QLabel *chartTitle = new QLabel("📊 Statistiques des Camions");
    chartTitle->setObjectName("chartTitle");
    QFont titleFont = chartTitle->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    chartTitle->setFont(titleFont);
    chartLayout->addWidget(chartTitle);

    // Charts Container
    QHBoxLayout *chartsContainer = new QHBoxLayout();
    chartsContainer->setSpacing(20);

    // Status Chart
    QWidget *statusChartWidget = new QWidget();
    statusChartWidget->setObjectName("miniChart");
    QVBoxLayout *statusChartLayout = new QVBoxLayout(statusChartWidget);
    statusChartLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *statusChartTitle = new QLabel("Statut des Camions");
    statusChartTitle->setObjectName("miniChartTitle");
    statusChartLayout->addWidget(statusChartTitle);

    // Status bars will be added dynamically
    statusChartLayout->addStretch();

    // Type Chart
    QWidget *typeChartWidget = new QWidget();
    typeChartWidget->setObjectName("miniChart");
    QVBoxLayout *typeChartLayout = new QVBoxLayout(typeChartWidget);
    typeChartLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *typeChartTitle = new QLabel("Types de Camions");
    typeChartTitle->setObjectName("miniChartTitle");
    typeChartLayout->addWidget(typeChartTitle);

    // Type bars will be added dynamically
    typeChartLayout->addStretch();

    // Capacity Chart
    QWidget *capacityChartWidget = new QWidget();
    capacityChartWidget->setObjectName("miniChart");
    QVBoxLayout *capacityChartLayout = new QVBoxLayout(capacityChartWidget);
    capacityChartLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *capacityChartTitle = new QLabel("Capacité Totale");
    capacityChartTitle->setObjectName("miniChartTitle");
    capacityChartLayout->addWidget(capacityChartTitle);

    QLabel *totalCapacity = new QLabel("22 T");
    totalCapacity->setObjectName("statValue");
    QFont statFont = totalCapacity->font();
    statFont.setPointSize(36);
    statFont.setBold(true);
    totalCapacity->setFont(statFont);
    totalCapacity->setAlignment(Qt::AlignCenter);
    capacityChartLayout->addWidget(totalCapacity);

    QLabel *capacityLabel = new QLabel("Capacité totale de la flotte");
    capacityLabel->setObjectName("statLabel");
    capacityLabel->setAlignment(Qt::AlignCenter);
    capacityChartLayout->addWidget(capacityLabel);

    capacityChartLayout->addStretch();

    chartsContainer->addWidget(statusChartWidget);
    chartsContainer->addWidget(typeChartWidget);
    chartsContainer->addWidget(capacityChartWidget);

    chartLayout->addLayout(chartsContainer);
}

void MainWindow::updateChartData()
{
    // Count statistics
    int activeCount = 0;
    int maintenanceCount = 0;
    int brokenCount = 0;
    QMap<QString, int> typeCount;
    int totalCapacity = 0;

    for (int row = 0; row < truckTable->rowCount(); ++row) {
        // Count status
        QWidget *statusWidget = truckTable->cellWidget(row, 3);
        QLabel *statusLabel = qobject_cast<QLabel*>(statusWidget);
        if (statusLabel) {
            QString status = statusLabel->text();
            if (status == "Actif") activeCount++;
            else if (status == "Maintenance") maintenanceCount++;
            else if (status == "En panne") brokenCount++;
        }

        // Count types
        QString type = truckTable->item(row, 1)->text();
        typeCount[type]++;

        // Sum capacity
        QString capacityStr = truckTable->item(row, 2)->text();
        totalCapacity += capacityStr.remove(" T").toInt();
    }

    // Update chart widget with current data
    // This is a simplified version - in a real app you'd update the actual chart bars
    QWidget *statusChartWidget = chartWidget->findChild<QWidget*>("miniChart");
    if (statusChartWidget) {
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(statusChartWidget->layout());
        if (layout) {
            // Remove old items except title
            while (layout->count() > 2) {
                QLayoutItem *item = layout->takeAt(1);
                delete item->widget();
                delete item;
            }

            // Add status bars
            addStatBar(layout, "Actif", activeCount, truckTable->rowCount(), "#4CAF50");
            addStatBar(layout, "Maintenance", maintenanceCount, truckTable->rowCount(), "#FF9800");
            addStatBar(layout, "En panne", brokenCount, truckTable->rowCount(), "#F44336");

            layout->addStretch();
        }
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

void MainWindow::addTableRow(int id, const QString &type, const QString &capacity,
                             const QString &status, const QString &location)
{
    int row = truckTable->rowCount();
    truckTable->insertRow(row);

    truckTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
    truckTable->setItem(row, 1, new QTableWidgetItem(type));
    truckTable->setItem(row, 2, new QTableWidgetItem(capacity));
    truckTable->setItem(row, 4, new QTableWidgetItem(location));

    // Status badge
    QLabel *statusLabel = new QLabel(status);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(getStatusStyle(status));
    truckTable->setCellWidget(row, 3, statusLabel);

    // Action buttons
    QWidget *actionWidget = new QWidget();
    QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
    actionLayout->setContentsMargins(5, 5, 5, 5);
    actionLayout->setSpacing(8);

    QPushButton *modifyBtn = new QPushButton("✏️ Modifier");
    modifyBtn->setObjectName("modifyButton");
    modifyBtn->setCursor(Qt::PointingHandCursor);
    modifyBtn->setFixedHeight(35);
    connect(modifyBtn, &QPushButton::clicked, [this, row]() {
        onModifyTruck(row);
    });

    QPushButton *deleteBtn = new QPushButton("🗑️");
    deleteBtn->setObjectName("deleteButton");
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setFixedSize(35, 35);
    connect(deleteBtn, &QPushButton::clicked, [this, row]() {
        onDeleteTruck(row);
    });

    actionLayout->addWidget(modifyBtn);
    actionLayout->addWidget(deleteBtn);
    actionLayout->addStretch();

    truckTable->setCellWidget(row, 5, actionWidget);
    truckTable->setRowHeight(row, 60);
}

QString MainWindow::getStatusStyle(const QString &status)
{
    if (status == "Actif") {
        return "background: #E8F5E9; color: #4CAF50; padding: 8px 16px; "
               "border-radius: 20px; font-weight: bold; font-size: 13px;";
    } else if (status == "Maintenance") {
        return "background: #FFF3E0; color: #FF9800; padding: 8px 16px; "
               "border-radius: 20px; font-weight: bold; font-size: 13px;";
    } else if (status == "En panne") {
        return "background: #FFEBEE; color: #F44336; padding: 8px 16px; "
               "border-radius: 20px; font-weight: bold; font-size: 13px;";
    }
    return "";
}

void MainWindow::clearFormInputs()
{
    typeInput->clear();
    capacityInput->setValue(5);
    statusCombo->setCurrentIndex(0);
    locationInput->clear();
    currentEditingRow = -1;
    formTitleLabel->setText("📝 Ajouter un camion");
    saveButton->setText("💾 Enregistrer");
}

void MainWindow::setFormForEditing(int row)
{
    if (row < 0 || row >= truckTable->rowCount()) return;

    currentEditingRow = row;
    QString id = truckTable->item(row, 0)->text();

    typeInput->setText(truckTable->item(row, 1)->text());

    QString capacityStr = truckTable->item(row, 2)->text();
    capacityInput->setValue(capacityStr.remove(" T").toInt());

    QWidget *statusWidget = truckTable->cellWidget(row, 3);
    QLabel *statusLabel = qobject_cast<QLabel*>(statusWidget);
    if (statusLabel) {
        statusCombo->setCurrentText(statusLabel->text());
    }

    locationInput->setText(truckTable->item(row, 4)->text());

    formTitleLabel->setText("✏️ Modifier le camion #" + id);
    saveButton->setText("💾 Mettre à jour");
}

void MainWindow::applyStyles()
{
    QString styleSheet = R"(
        QMainWindow {
            background: #f5f5f5;
        }
        #sidebar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #2C5F2D, stop:1 #1E4620);
        }
        #logoTitle {
            color: white;
            font-size: 22px;
            font-weight: bold;
        }
        #menuItem {
            background: transparent;
            color: rgba(255, 255, 255, 0.8);
            border: none;
            text-align: left;
            padding-left: 30px;
            font-size: 15px;
        }
        #menuItem:hover {
            background: rgba(255, 255, 255, 0.1);
            color: white;
        }
        #menuItem[active="true"] {
            background: rgba(163, 198, 81, 0.3);
            color: white;
            border-left: 4px solid #A3C651;
            font-weight: bold;
        }
        #userName {
            color: white;
            font-size: 14px;
        }
        #formPanel {
            background: white;
            border-right: 1px solid #e0e0e0;
        }
        #formTitle {
            color: #2C5F2D;
        }
        #formLabel {
            color: #000000;
            font-weight: 600;
            font-size: 14px;
        }
        #formInput {
            border: 2px solid #e0e0e0;
            border-radius: 10px;
            padding: 10px 15px;
            font-size: 14px;
            background: white;
            color: #000000;
        }
        #formInput:focus {
            border-color: #A3C651;
        }
        #saveButton {
            background: #A3C651;
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 16px;
            font-weight: bold;
        }
        #saveButton:hover {
            background: #8AB344;
        }
        #infoBox {
            background: #E3F2FD;
            color: #1976D2;
            padding: 15px;
            border-radius: 10px;
            border-left: 4px solid #2196F3;
        }
        #mainContent {
            background: #f5f5f5;
        }
        #header {
            background: white;
            border-radius: 12px;
        }
        #breadcrumb {
            color: #666;
            font-size: 13px;
        }
        #headerBtn {
            background: #f5f5f5;
            border: none;
            border-radius: 10px;
            font-size: 18px;
        }
        #headerBtn:hover {
            background: #A3C651;
        }
        #contentWrapper {
            background: white;
            border-radius: 15px;
        }
        #pageTitle {
            font-size: 26px;
            font-weight: bold;
            color: #000000;
        }
        #searchInput {
            border: 1px solid #ddd;
            border-radius: 10px;
            padding: 0 15px;
            font-size: 14px;
            color: #000000;
        }
        #filterSelect {
            border: 1px solid #ddd;
            border-radius: 10px;
            padding: 0 15px;
            font-size: 14px;
            background: white;
            color: #000000;
        }
        #truckTable {
            background: white;
            border: none;
            gridline-color: #eee;
            color: #000000;
        }
        #truckTable::item {
            padding: 15px;
            border-bottom: 1px solid #eee;
            color: #000000;
        }
        #truckTable::item:selected {
            background: #f9f9f9;
        }
        QHeaderView::section {
            background: #f9f9f9;
            padding: 12px;
            border: none;
            border-bottom: 2px solid #eee;
            font-weight: 600;
            color: #000000;
        }
        #modifyButton {
            background: #E3F2FD;
            color: #2196F3;
            border: none;
            border-radius: 8px;
            padding: 8px 20px;
            font-size: 13px;
        }
        #modifyButton:hover {
            background: #2196F3;
            color: white;
        }
        #deleteButton {
            background: #FFEBEE;
            color: #B84446;
            border: none;
            border-radius: 8px;
            padding: 8px 15px;
            font-size: 13px;
        }
        #deleteButton:hover {
            background: #B84446;
            color: white;
        }
        #paginationInfo {
            color: #666;
            font-size: 13px;
        }
        #pageButton {
            border: 1px solid #ddd;
            background: white;
            border-radius: 8px;
        }
        #pageButton:hover {
            background: #A3C651;
            color: white;
            border-color: #A3C651;
        }
        #pageButton[active="true"] {
            background: #A3C651;
            color: white;
            border-color: #A3C651;
            font-weight: bold;
        }
        #itemsPerPage {
            border: 1px solid #ddd;
            border-radius: 8px;
            padding: 0 10px;
            background: white;
        }
        #exportPdfButton {
            background: #FF5722;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 0 25px;
            font-size: 15px;
            font-weight: bold;
            min-width: 150px;
        }
        #exportPdfButton:hover {
            background: #E64A19;
            transform: scale(1.02);
        }
        #chartWidget {
            background: white;
            border-radius: 15px;
        }
        #chartTitle {
            color: #000000;
        }
        #miniChart {
            background: #f9f9f9;
            border-radius: 12px;
            border: 1px solid #e0e0e0;
        }
        #miniChartTitle {
            color: #000000;
            font-size: 16px;
            font-weight: bold;
            margin-bottom: 15px;
        }
        #statValue {
            color: #A3C651;
        }
        #statLabel {
            color: #666;
            font-size: 13px;
        }
        #barLabel {
            color: #000000;
            font-size: 13px;
        }
        #barCount {
            color: #000000;
            font-weight: bold;
            font-size: 13px;
        }
    )";

    setStyleSheet(styleSheet);
}

// ==================== Slots Implementation ====================
void MainWindow::onAddTruck()
{
    QString type = typeInput->text();
    QString location = locationInput->text();

    if (type.isEmpty() || location.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs obligatoires");
        return;
    }

    int capacity = capacityInput->value();
    QString status = statusCombo->currentText();
    QString capacityStr = QString::number(capacity) + " T";

    if (currentEditingRow >= 0) {
        // Mode édition
        QString id = truckTable->item(currentEditingRow, 0)->text();

        truckTable->item(currentEditingRow, 1)->setText(type);
        truckTable->item(currentEditingRow, 2)->setText(capacityStr);
        truckTable->item(currentEditingRow, 4)->setText(location);

        QLabel *newStatusLabel = new QLabel(status);
        newStatusLabel->setStyleSheet(getStatusStyle(status));
        newStatusLabel->setAlignment(Qt::AlignCenter);
        truckTable->setCellWidget(currentEditingRow, 3, newStatusLabel);

        QMessageBox::information(this, "Succès", "Camion #" + id + " modifié avec succès!");
        clearFormInputs();
        updateChartData();
    } else {
        // Mode ajout
        addTableRow(nextId++, type, capacityStr, status, location);
        QMessageBox::information(this, "Succès", "Camion ajouté avec succès!");
        clearFormInputs();
        updateChartData();
    }
}

void MainWindow::onModifyTruck(int row)
{
    setFormForEditing(row);
}

void MainWindow::onDeleteTruck(int row)
{
    if (row < 0 || row >= truckTable->rowCount()) return;

    QString id = truckTable->item(row, 0)->text();
    QString type = truckTable->item(row, 1)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmer la suppression",
                                  "Voulez-vous vraiment supprimer le camion #" + id + " (" + type + ") ?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Si on supprime la ligne qu'on est en train d'éditer, effacer le formulaire
        if (currentEditingRow == row) {
            clearFormInputs();
        }

        truckTable->removeRow(row);
        updateChartData();
        QMessageBox::information(this, "Succès", "Camion supprimé avec succès!");
    }
}

void MainWindow::onExportPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Exporter le Rapport",
                                                    QDir::homePath() + "/TuniWaste_Camions_" + QDate::currentDate().toString("yyyy-MM-dd") + ".html",
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
    int activeCount = 0;
    int maintenanceCount = 0;
    int brokenCount = 0;
    int totalCapacity = 0;

    for (int row = 0; row < truckTable->rowCount(); ++row) {
        QWidget *statusWidget = truckTable->cellWidget(row, 3);
        QLabel *statusLabel = qobject_cast<QLabel*>(statusWidget);
        if (statusLabel) {
            QString status = statusLabel->text();
            if (status == "Actif") activeCount++;
            else if (status == "Maintenance") maintenanceCount++;
            else if (status == "En panne") brokenCount++;
        }

        QString capacityStr = truckTable->item(row, 2)->text();
        totalCapacity += capacityStr.remove(" T").toInt();
    }

    // Generate HTML
    out << "<!DOCTYPE html>\n";
    out << "<html>\n<head>\n";
    out << "<meta charset='UTF-8'>\n";
    out << "<title>TuniWaste - Rapport des Camions</title>\n";
    out << "<style>\n";
    out << "body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n";
    out << ".container { max-width: 1200px; margin: 0 auto; background: white; padding: 40px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }\n";
    out << "h1 { color: #2C5F2D; text-align: center; font-size: 36px; margin-bottom: 10px; }\n";
    out << "h2 { color: #2C5F2D; font-size: 24px; margin-top: 30px; border-bottom: 3px solid #A3C651; padding-bottom: 10px; }\n";
    out << ".subtitle { text-align: center; color: #666; margin-bottom: 10px; }\n";
    out << ".date { text-align: center; color: #999; font-size: 14px; margin-bottom: 40px; }\n";
    out << ".stats { display: flex; justify-content: space-around; margin: 30px 0; }\n";
    out << ".stat-box { flex: 1; margin: 0 10px; padding: 20px; border-radius: 10px; text-align: center; }\n";
    out << ".stat-box.total { background: #E8F5E9; border: 2px solid #4CAF50; }\n";
    out << ".stat-box.active { background: #E3F2FD; border: 2px solid #2196F3; }\n";
    out << ".stat-box.capacity { background: #FFF3E0; border: 2px solid #FF9800; }\n";
    out << ".stat-title { font-size: 14px; color: #666; margin-bottom: 10px; }\n";
    out << ".stat-value { font-size: 48px; font-weight: bold; margin: 10px 0; }\n";
    out << ".stat-box.total .stat-value { color: #4CAF50; }\n";
    out << ".stat-box.active .stat-value { color: #2196F3; }\n";
    out << ".stat-box.capacity .stat-value { color: #FF9800; }\n";
    out << "table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n";
    out << "th { background: #2C5F2D; color: white; padding: 15px; text-align: left; font-weight: bold; }\n";
    out << "td { padding: 12px 15px; border-bottom: 1px solid #eee; }\n";
    out << "tr:nth-child(even) { background: #f9f9f9; }\n";
    out << "tr:hover { background: #f0f0f0; }\n";
    out << ".status { padding: 6px 12px; border-radius: 15px; font-weight: bold; font-size: 12px; display: inline-block; }\n";
    out << ".status-actif { background: #E8F5E9; color: #4CAF50; }\n";
    out << ".status-maintenance { background: #FFF3E0; color: #FF9800; }\n";
    out << ".status-panne { background: #FFEBEE; color: #F44336; }\n";
    out << ".footer { text-align: center; color: #999; margin-top: 50px; padding-top: 20px; border-top: 1px solid #eee; font-size: 12px; }\n";
    out << "@media print { body { margin: 0; background: white; } .container { box-shadow: none; } }\n";
    out << "</style>\n";
    out << "</head>\n<body>\n";
    out << "<div class='container'>\n";

    // Header
    out << "<h1>🌱 TuniWaste</h1>\n";
    out << "<div class='subtitle'>Rapport de Gestion des Camions</div>\n";
    out << "<div class='date'>Généré le " << QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm") << "</div>\n";

    // Statistics
    out << "<h2>📊 Statistiques</h2>\n";
    out << "<div class='stats'>\n";
    out << "<div class='stat-box total'>\n";
    out << "<div class='stat-title'>Total Camions</div>\n";
    out << "<div class='stat-value'>" << truckTable->rowCount() << "</div>\n";
    out << "</div>\n";
    out << "<div class='stat-box active'>\n";
    out << "<div class='stat-title'>Camions Actifs</div>\n";
    out << "<div class='stat-value'>" << activeCount << "</div>\n";
    out << "</div>\n";
    out << "<div class='stat-box capacity'>\n";
    out << "<div class='stat-title'>Capacité Totale</div>\n";
    out << "<div class='stat-value'>" << totalCapacity << " T</div>\n";
    out << "</div>\n";
    out << "</div>\n";

    // Table
    out << "<h2>📋 Liste des Camions</h2>\n";
    out << "<table>\n";
    out << "<thead>\n<tr>\n";
    out << "<th>ID</th><th>Type</th><th>Capacité</th><th>Statut</th><th>Localisation</th>\n";
    out << "</tr>\n</thead>\n<tbody>\n";

    for (int row = 0; row < truckTable->rowCount(); ++row) {
        out << "<tr>\n";
        out << "<td>" << truckTable->item(row, 0)->text() << "</td>\n";
        out << "<td>" << truckTable->item(row, 1)->text() << "</td>\n";
        out << "<td>" << truckTable->item(row, 2)->text() << "</td>\n";

        QWidget *statusWidget = truckTable->cellWidget(row, 3);
        QLabel *statusLabel = qobject_cast<QLabel*>(statusWidget);
        if (statusLabel) {
            QString status = statusLabel->text();
            QString statusClass = "status-actif";
            if (status == "Maintenance") statusClass = "status-maintenance";
            else if (status == "En panne") statusClass = "status-panne";
            out << "<td><span class='status " << statusClass << "'>" << status << "</span></td>\n";
        }

        out << "<td>" << truckTable->item(row, 4)->text() << "</td>\n";
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
    for (int row = 0; row < truckTable->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < 5; ++col) {
            QTableWidgetItem *item = truckTable->item(row, col);
            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        truckTable->setRowHidden(row, !match);
    }
}

void MainWindow::onFilterChanged(int index)
{
    if (index == 0) {
        for (int row = 0; row < truckTable->rowCount(); ++row) {
            truckTable->setRowHidden(row, false);
        }
        return;
    }

    QString selectedStatus = statusFilter->currentText();

    for (int row = 0; row < truckTable->rowCount(); ++row) {
        QWidget *statusWidget = truckTable->cellWidget(row, 3);
        QLabel *statusLabel = qobject_cast<QLabel*>(statusWidget);
        QString status = statusLabel ? statusLabel->text() : "";

        truckTable->setRowHidden(row, status != selectedStatus);
    }
}
