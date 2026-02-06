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
#include <QDialog>

// ==================== TruckDialog Implementation ====================
TruckDialog::TruckDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Ajouter un camion");
    setFixedSize(500, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Title
    QLabel *titleLabel = new QLabel("Informations du camion");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Form
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setContentsMargins(0, 20, 0, 20);

    typeInput = new QLineEdit();
    typeInput->setPlaceholderText("Ex: Compact, Grand, Électrique");
    typeInput->setFixedHeight(40);
    formLayout->addRow("Type de camion:", typeInput);

    capacityInput = new QSpinBox();
    capacityInput->setSuffix(" T");
    capacityInput->setMinimum(1);
    capacityInput->setMaximum(50);
    capacityInput->setValue(5);
    capacityInput->setFixedHeight(40);
    formLayout->addRow("Capacité:", capacityInput);

    statusCombo = new QComboBox();
    statusCombo->addItem("Actif");
    statusCombo->addItem("Maintenance");
    statusCombo->addItem("En panne");
    statusCombo->setFixedHeight(40);
    formLayout->addRow("Statut:", statusCombo);

    locationInput = new QLineEdit();
    locationInput->setPlaceholderText("Ex: Tunis, Ariana, Sfax");
    locationInput->setFixedHeight(40);
    formLayout->addRow("Localisation:", locationInput);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    QPushButton *cancelBtn = new QPushButton("Annuler");
    cancelBtn->setFixedHeight(45);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet("background: #f5f5f5; border: 1px solid #ddd; border-radius: 8px; padding: 0 30px; font-size: 14px;");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton *saveBtn = new QPushButton("Enregistrer");
    saveBtn->setFixedHeight(45);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet("background: #A3C651; color: white; border: none; border-radius: 8px; padding: 0 30px; font-size: 14px; font-weight: bold;");
    connect(saveBtn, &QPushButton::clicked, [this]() {
        if (typeInput->text().isEmpty() || locationInput->text().isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs");
            return;
        }
        accept();
    });

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(saveBtn);

    mainLayout->addLayout(buttonLayout);

    setStyleSheet("QDialog { background: white; } QLineEdit, QSpinBox, QComboBox { border: 1px solid #ddd; border-radius: 6px; padding: 8px; } QLabel { color: #333; }");
}

void TruckDialog::setData(const QString &type, int capacity, const QString &status, const QString &location)
{
    typeInput->setText(type);
    capacityInput->setValue(capacity);
    statusCombo->setCurrentText(status);
    locationInput->setText(location);
}

QString TruckDialog::getType() const { return typeInput->text(); }
int TruckDialog::getCapacity() const { return capacityInput->value(); }
QString TruckDialog::getStatus() const { return statusCombo->currentText(); }
QString TruckDialog::getLocation() const { return locationInput->text(); }

// ==================== MainWindow Implementation ====================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), nextId(4)
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
    createMainContent();

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(mainContent);

    setWindowTitle("TuniWaste - Gestion des camions");
    resize(1400, 800);
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
    logoLabel->setStyleSheet("background: white; border-radius: 15px;");
    QPixmap logo("logo.png");
    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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

    QPushButton *searchBtn = new QPushButton("🔍");
    searchBtn->setObjectName("headerBtn");
    searchBtn->setFixedSize(40, 40);
    searchBtn->setCursor(Qt::PointingHandCursor);

    QPushButton *notifBtn = new QPushButton("🔔");
    notifBtn->setObjectName("headerBtn");
    notifBtn->setFixedSize(40, 40);
    notifBtn->setCursor(Qt::PointingHandCursor);

    QPushButton *settingsBtn = new QPushButton("⚙️");
    settingsBtn->setObjectName("headerBtn");
    settingsBtn->setFixedSize(40, 40);
    settingsBtn->setCursor(Qt::PointingHandCursor);

    headerLayout->addWidget(breadcrumb);
    headerLayout->addStretch();
    headerLayout->addWidget(searchBtn);
    headerLayout->addWidget(notifBtn);
    headerLayout->addWidget(settingsBtn);

    contentLayout->addWidget(headerWidget);

    // Content Wrapper
    QWidget *contentWrapper = new QWidget();
    contentWrapper->setObjectName("contentWrapper");

    QVBoxLayout *wrapperLayout = new QVBoxLayout(contentWrapper);
    wrapperLayout->setContentsMargins(30, 30, 30, 30);
    wrapperLayout->setSpacing(25);

    // Title and Add Button
    QWidget *titleWidget = new QWidget();
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *pageTitle = new QLabel("Gestion des camions");
    pageTitle->setObjectName("pageTitle");

    addButton = new QPushButton("+ Ajouter camion");
    addButton->setObjectName("addButton");
    addButton->setCursor(Qt::PointingHandCursor);
    addButton->setFixedHeight(45);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddTruck);

    titleLayout->addWidget(pageTitle);
    titleLayout->addStretch();
    titleLayout->addWidget(addButton);

    wrapperLayout->addWidget(titleWidget);

    // Filters
    QWidget *filterWidget = new QWidget();
    QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(15);

    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("🔍 Rechercher par ID ou type...");
    searchInput->setObjectName("searchInput");
    searchInput->setFixedHeight(45);
    connect(searchInput, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    statusFilter = new QComboBox();
    statusFilter->setObjectName("filterSelect");
    statusFilter->addItem("⚡ Filtrer par : Statut : Tous les statuts");
    statusFilter->addItem("Actif");
    statusFilter->addItem("Maintenance");
    statusFilter->addItem("En panne");
    statusFilter->setFixedHeight(45);
    statusFilter->setMinimumWidth(250);
    statusFilter->setCursor(Qt::PointingHandCursor);
    connect(statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFilterChanged);

    typeFilter = new QComboBox();
    typeFilter->setObjectName("filterSelect");
    typeFilter->addItem("Type de camion");
    typeFilter->addItem("Compact");
    typeFilter->addItem("Grand");
    typeFilter->addItem("Électrique");
    typeFilter->setFixedHeight(45);
    typeFilter->setMinimumWidth(200);
    typeFilter->setCursor(Qt::PointingHandCursor);

    filterLayout->addWidget(searchInput);
    filterLayout->addWidget(statusFilter);
    filterLayout->addWidget(typeFilter);

    wrapperLayout->addWidget(filterWidget);

    // Table
    truckTable = new QTableWidget();
    truckTable->setObjectName("truckTable");
    truckTable->setColumnCount(6);
    truckTable->setHorizontalHeaderLabels({"ID", "Type camion", "Capacité", "Statut", "Localisation", "Actions"});

    truckTable->horizontalHeader()->setStretchLastSection(true);
    truckTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    truckTable->verticalHeader()->setVisible(false);
    truckTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    truckTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    truckTable->setShowGrid(false);
    truckTable->setMinimumHeight(300);

    wrapperLayout->addWidget(truckTable);

    // Pagination
    QWidget *paginationWidget = new QWidget();
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 20, 0, 0);

    QLabel *paginationInfo = new QLabel("Affichage de 1 à 3 sur 3 camions");
    paginationInfo->setObjectName("paginationInfo");

    QPushButton *prevBtn = new QPushButton("<");
    prevBtn->setObjectName("pageButton");
    prevBtn->setFixedSize(35, 35);
    prevBtn->setCursor(Qt::PointingHandCursor);

    QLabel *currentPage = new QLabel("1");
    currentPage->setObjectName("pageButton");
    currentPage->setProperty("active", true);
    currentPage->setFixedSize(35, 35);
    currentPage->setAlignment(Qt::AlignCenter);

    QPushButton *nextBtn = new QPushButton(">");
    nextBtn->setObjectName("pageButton");
    nextBtn->setFixedSize(35, 35);
    nextBtn->setCursor(Qt::PointingHandCursor);

    QComboBox *itemsPerPage = new QComboBox();
    itemsPerPage->setObjectName("itemsPerPage");
    itemsPerPage->addItem("Afficher 10 camions");
    itemsPerPage->addItem("Afficher 25 camions");
    itemsPerPage->addItem("Afficher 50 camions");
    itemsPerPage->setCursor(Qt::PointingHandCursor);

    paginationLayout->addWidget(paginationInfo);
    paginationLayout->addStretch();
    paginationLayout->addWidget(prevBtn);
    paginationLayout->addWidget(currentPage);
    paginationLayout->addWidget(nextBtn);
    paginationLayout->addWidget(itemsPerPage);

    wrapperLayout->addWidget(paginationWidget);

    contentLayout->addWidget(contentWrapper);
}

void MainWindow::loadTruckData()
{
    addTableRow(1, "Compact", "5 T", "Actif", "Tunis");
    addTableRow(2, "Grand", "12 T", "Maintenance", "Ariana");
    addTableRow(3, "Électrique", "8 T", "En panne", "Sfax");
}

void MainWindow::addTableRow(int id, const QString &type, const QString &capacity,
                             const QString &status, const QString &location)
{
    int row = truckTable->rowCount();
    truckTable->insertRow(row);

    truckTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
    truckTable->setItem(row, 1, new QTableWidgetItem(type));
    truckTable->setItem(row, 2, new QTableWidgetItem(capacity));

    // Status badge
    QLabel *statusLabel = new QLabel(status);
    statusLabel->setStyleSheet(getStatusStyle(status));
    statusLabel->setAlignment(Qt::AlignCenter);
    truckTable->setCellWidget(row, 3, statusLabel);

    truckTable->setItem(row, 4, new QTableWidgetItem(location));

    // Action buttons
    QWidget *actionWidget = new QWidget();
    QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
    actionLayout->setContentsMargins(5, 5, 5, 5);
    actionLayout->setSpacing(10);

    QPushButton *modifyBtn = new QPushButton("✏️ Modifier");
    modifyBtn->setObjectName("modifyButton");
    modifyBtn->setCursor(Qt::PointingHandCursor);
    modifyBtn->setFixedHeight(35);

    if (status == "En panne") {
        modifyBtn->setProperty("danger", true);
        modifyBtn->style()->unpolish(modifyBtn);
        modifyBtn->style()->polish(modifyBtn);
    }

    connect(modifyBtn, &QPushButton::clicked, [this, row]() {
        onModifyTruck(row);
    });

    QPushButton *deleteBtn = new QPushButton("🗑️ Supprimer");
    deleteBtn->setObjectName("deleteButton");
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setFixedHeight(35);

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
        return "background: #A3C651; color: white; padding: 6px 15px; border-radius: 20px; font-weight: 500;";
    } else if (status == "Maintenance") {
        return "background: #FDB44B; color: white; padding: 6px 15px; border-radius: 20px; font-weight: 500;";
    } else if (status == "En panne") {
        return "background: #B84446; color: white; padding: 6px 15px; border-radius: 20px; font-weight: 500;";
    }
    return "";
}

void MainWindow::applyStyles()
{
    QString styleSheet = R"(
        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #A3C651, stop:1 #1b5e20);
        }
        #sidebar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #A3C651, stop:1 #1b5e20);
        }
        #logoTitle {
            font-size: 24px;
            font-weight: bold;
            color: white;
        }
        #menuItem {
            background: transparent;
            color: white;
            border: none;
            text-align: left;
            padding-left: 25px;
            font-size: 14px;
        }
        #menuItem:hover {
            background: rgba(255, 255, 255, 0.1);
        }
        #menuItem[active="true"] {
            background: white;
            color: #A3C651;
            font-weight: 600;
        }
        #userName {
            color: white;
            font-weight: bold;
            font-size: 15px;
        }
        #mainContent {
            background: #f5f5f5;
        }
        #header {
            background: white;
            border-radius: 15px;
        }
        #breadcrumb {
            color: #666;
            font-size: 13px;
        }
        #headerBtn {
            background: #f5f5f5;
            border: none;
            border-radius: 10px;
            font-size: 16px;
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
            color: #333;
        }
        #addButton {
            background: #A3C651;
            color: white;
            border: none;
            border-radius: 10px;
            padding: 0 25px;
            font-size: 15px;
            font-weight: 500;
        }
        #addButton:hover {
            background: #8AB344;
        }
        #searchInput {
            border: 1px solid #ddd;
            border-radius: 10px;
            padding: 0 15px;
            font-size: 14px;
        }
        #filterSelect {
            border: 1px solid #ddd;
            border-radius: 10px;
            padding: 0 15px;
            font-size: 14px;
            background: white;
        }
        #truckTable {
            background: white;
            border: none;
            gridline-color: #eee;
        }
        #truckTable::item {
            padding: 15px;
            border-bottom: 1px solid #eee;
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
            color: #333;
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
        #modifyButton[danger="true"] {
            background: #FFEBEE;
            color: #B84446;
        }
        #modifyButton[danger="true"]:hover {
            background: #B84446;
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
    )";

    setStyleSheet(styleSheet);
}

// ==================== Slots Implementation ====================
void MainWindow::onAddTruck()
{
    TruckDialog dialog(this);
    dialog.setWindowTitle("Ajouter un camion");

    if (dialog.exec() == QDialog::Accepted) {
        QString type = dialog.getType();
        int capacity = dialog.getCapacity();
        QString status = dialog.getStatus();
        QString location = dialog.getLocation();

        QString capacityStr = QString::number(capacity) + " T";
        addTableRow(nextId++, type, capacityStr, status, location);

        QMessageBox::information(this, "Succès", "Camion ajouté avec succès!");
    }
}

void MainWindow::onModifyTruck(int row)
{
    if (row < 0 || row >= truckTable->rowCount()) return;

    QString id = truckTable->item(row, 0)->text();
    QString currentType = truckTable->item(row, 1)->text();
    QString currentCapacityStr = truckTable->item(row, 2)->text();
    QString currentLocation = truckTable->item(row, 4)->text();

    int currentCapacity = currentCapacityStr.remove(" T").toInt();

    QWidget *statusWidget = truckTable->cellWidget(row, 3);
    QLabel *statusLabel = qobject_cast<QLabel*>(statusWidget);  // FIXED: Added pointer *
    QString currentStatus = statusLabel ? statusLabel->text() : "Actif";

    TruckDialog dialog(this);
    dialog.setWindowTitle("Modifier le camion #" + id);
    dialog.setData(currentType, currentCapacity, currentStatus, currentLocation);

    if (dialog.exec() == QDialog::Accepted) {
        QString type = dialog.getType();
        int capacity = dialog.getCapacity();
        QString status = dialog.getStatus();
        QString location = dialog.getLocation();

        QString capacityStr = QString::number(capacity) + " T";

        truckTable->item(row, 1)->setText(type);
        truckTable->item(row, 2)->setText(capacityStr);
        truckTable->item(row, 4)->setText(location);

        QLabel *newStatusLabel = new QLabel(status);
        newStatusLabel->setStyleSheet(getStatusStyle(status));
        newStatusLabel->setAlignment(Qt::AlignCenter);
        truckTable->setCellWidget(row, 3, newStatusLabel);

        QMessageBox::information(this, "Succès", "Camion modifié avec succès!");
    }
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
        truckTable->removeRow(row);
        QMessageBox::information(this, "Succès", "Camion supprimé avec succès!");
    }
}

void MainWindow::onMenuItemClicked()
{
    for (QPushButton *btn : menuButtons) {
        btn->setProperty("active", false);
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }

    QPushButton *clickedBtn = qobject_cast<QPushButton*>(sender());  // FIXED: Added pointer *
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
        QLabel *statusLabel = qobject_cast<QLabel*>(statusWidget);  // FIXED: Added pointer *
        QString status = statusLabel ? statusLabel->text() : "";

        truckTable->setRowHidden(row, status != selectedStatus);
    }
}
