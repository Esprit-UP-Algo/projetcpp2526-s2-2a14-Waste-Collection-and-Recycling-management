#include "mainwindow.h"
#include "RECYCLAGE.h"
#include "gestionzones.h"
#include <QCoreApplication>
#include <QDate>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPdfWriter>
#include <QScrollArea>
#include <QUrl>
#include <QVBoxLayout>
#include <algorithm>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), nextUserId(5), currentSortColumn(-1),
      currentSortOrder(Qt::AscendingOrder) {
  users.append({1, "Ahmed Ben Ali", "ahmed.benali@example.com",
                "+216 98 123 456", "Administrateur"});
  users.append({2, "Sami Trabelsi", "sami.trabelsi@example.com",
                "+216 22 456 789", "Employe"});
  users.append({3, "Ali Haddad", "ali.haddad@example.com", "+216 55 789 123",
                "Employe"});
  users.append({4, "Nadia Ayari", "nadia.ayari@example.com", "+216 20 321 654",
                "Employe"});

  filteredUsers = users;

  stackedWidget = new QStackedWidget(this);
  setCentralWidget(stackedWidget);

  setupLoginScreen();
  setupUserManagementScreen();

  stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow() {}

void MainWindow::setupLoginScreen() {
  QWidget *loginWidget = new QWidget();
  loginWidget->setStyleSheet("QWidget { background-color: #E8F5E9; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(loginWidget);
  mainLayout->setAlignment(Qt::AlignCenter);

  QWidget *card = new QWidget();
  card->setFixedWidth(450);
  card->setStyleSheet("QWidget#loginCard { "
                      "   background-color: #FFFFFF; "
                      "   border: 2px solid #E0E0E0; "
                      "   border-radius: 12px; "
                      "}");
  card->setObjectName("loginCard");

  QVBoxLayout *cardLayout = new QVBoxLayout(card);
  cardLayout->setSpacing(20);
  cardLayout->setContentsMargins(40, 40, 40, 40);

  // Logo and Title Container
  QWidget *logoTitleWidget = new QWidget();
  logoTitleWidget->setStyleSheet("QWidget { background: transparent; }");
  QHBoxLayout *logoTitleLayout = new QHBoxLayout(logoTitleWidget);
  logoTitleLayout->setSpacing(15);
  logoTitleLayout->setAlignment(Qt::AlignCenter);

  QLabel *logoLabel = new QLabel();
  QPixmap logo;

  // Plus de chemins de recherche pour le logo (très robuste)
  if (logo.load(":/logo.png")) {
  } else if (logo.load("logo.png")) {
  } else if (logo.load("./logo.png")) {
  } else if (logo.load("../logo.png")) {    // Cas où l'exe est dans build/Debug
  } else if (logo.load("../../logo.png")) { // Cas où l'exe est plus profond
  } else if (logo.load(QCoreApplication::applicationDirPath() + "/logo.png")) {
  } else if (logo.load(QCoreApplication::applicationDirPath() +
                       "/../logo.png")) {
  }

  if (!logo.isNull()) {
    logo.setMask(logo.createMaskFromColor(Qt::white));
    logoLabel->setPixmap(
        logo.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setStyleSheet("background: transparent; border: none;");
  } else {
    logoLabel->setText(
        "♻️"); // Emoji recyclage plus pertinent que la corbeille grise
    logoLabel->setStyleSheet(
        "font-size: 80px; color: #6FA85E; background: transparent;");
  }
  logoLabel->setAlignment(Qt::AlignCenter);
  logoTitleLayout->addWidget(logoLabel);

  QLabel *titleLogoLabel = new QLabel("TuniWaste");
  titleLogoLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: "
                                "#6FA85E; background: transparent;");
  logoTitleLayout->addWidget(titleLogoLabel);

  cardLayout->addWidget(logoTitleWidget);

  QLabel *titleLabel = new QLabel("Bienvenue sur TuniWaste");
  titleLabel->setStyleSheet(
      "font-size: 16px; font-weight: bold; color: #000000; margin: 10px 0px; "
      "background: transparent;");
  titleLabel->setAlignment(Qt::AlignCenter);
  cardLayout->addWidget(titleLabel);

  QLabel *emailLabel = new QLabel("Email");
  emailLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: "
                            "#000000; background: transparent;");
  cardLayout->addWidget(emailLabel);

  QLineEdit *emailEdit = new QLineEdit();
  emailEdit->setPlaceholderText("nom@example.com");
  emailEdit->setStyleSheet("QLineEdit { "
                           "   padding: 12px; "
                           "   border: 2px solid #CCCCCC; "
                           "   border-radius: 8px; "
                           "   font-size: 14px; "
                           "   background-color: #FFFFFF; "
                           "   color: #000000; "
                           "}"
                           "QLineEdit:focus { border: 2px solid #A3C651; }");
  cardLayout->addWidget(emailEdit);

  QLabel *passwordLabel = new QLabel("Mot de passe");
  passwordLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: "
                               "#000000; background: transparent;");
  cardLayout->addWidget(passwordLabel);

  QLineEdit *passwordEdit = new QLineEdit();
  passwordEdit->setPlaceholderText("Password");
  passwordEdit->setEchoMode(QLineEdit::Password);
  passwordEdit->setStyleSheet("QLineEdit { "
                              "   padding: 12px; "
                              "   border: 2px solid #CCCCCC; "
                              "   border-radius: 8px; "
                              "   font-size: 14px; "
                              "   background-color: #FFFFFF; "
                              "   color: #000000; "
                              "}"
                              "QLineEdit:focus { border: 2px solid #A3C651; }");
  cardLayout->addWidget(passwordEdit);

  cardLayout->addSpacing(10);

  QPushButton *loginButton = new QPushButton("Connexion");
  loginButton->setStyleSheet(
      "QPushButton { "
      "   background-color: #A3C651; "
      "   color: #FFFFFF; "
      "   font-size: 16px; "
      "   font-weight: bold; "
      "   padding: 14px; "
      "   border: none; "
      "   border-radius: 8px; "
      "}"
      "QPushButton:hover { background-color: #8FB544; }"
      "QPushButton:pressed { background-color: #7DA43A; }");
  connect(loginButton, &QPushButton::clicked, this,
          &MainWindow::onLoginClicked);
  cardLayout->addWidget(loginButton);

  QPushButton *forgotButton = new QPushButton("Mot de passe oublié ?");
  forgotButton->setStyleSheet("QPushButton { "
                              "   color: #5A8F47; "
                              "   font-size: 13px; "
                              "   background: transparent; "
                              "   border: none; "
                              "   text-decoration: underline; "
                              "}"
                              "QPushButton:hover { "
                              "   color: #A3C651; "
                              "   cursor: pointer; "
                              "}");
  forgotButton->setCursor(Qt::PointingHandCursor);
  connect(forgotButton, &QPushButton::clicked, this,
          &MainWindow::onForgotPasswordClicked);

  QHBoxLayout *forgotLayout = new QHBoxLayout();
  forgotLayout->addStretch();
  forgotLayout->addWidget(forgotButton);
  forgotLayout->addStretch();
  cardLayout->addLayout(forgotLayout);

  mainLayout->addWidget(card);

  stackedWidget->addWidget(loginWidget);
}

QWidget *MainWindow::createSidebar() {
  QWidget *sidebar = new QWidget();
  sidebar->setFixedWidth(343);
  sidebar->setStyleSheet("QWidget { background-color: #6FA85E; }");

  QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
  sidebarLayout->setContentsMargins(0, 0, 0, 0);
  sidebarLayout->setSpacing(0);

  QWidget *logoContainer = new QWidget();
  logoContainer->setStyleSheet("QWidget { background-color: #6FA85E; }");
  QHBoxLayout *logoLayout = new QHBoxLayout(logoContainer);
  logoLayout->setContentsMargins(20, 20, 20, 20);
  logoLayout->setSpacing(15);
  logoLayout->setAlignment(Qt::AlignLeft);

  // Logo Image
  QLabel *logoLabel = new QLabel();
  QPixmap logo;

  // Chemins de recherche pour la sidebar
  if (logo.load(":/logo.png")) {
  } else if (logo.load("logo.png")) {
  } else if (logo.load("../logo.png")) {
  } else if (logo.load("../../logo.png")) {
  } else if (logo.load(QCoreApplication::applicationDirPath() + "/logo.png")) {
  }

  if (!logo.isNull()) {
    logo.setMask(logo.createMaskFromColor(Qt::white));
    logoLabel->setPixmap(
        logo.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setStyleSheet("background: transparent;");
  } else {
    logoLabel->setText("♻️");
    logoLabel->setStyleSheet(
        "font-size: 50px; color: white; background: transparent;");
  }
  logoLayout->addWidget(logoLabel);

  // App Title
  QLabel *appTitleLabel = new QLabel("TuniWaste");
  appTitleLabel->setStyleSheet("color: #FFFFFF; font-size: 26px; font-weight: "
                               "bold; background: transparent;");
  appTitleLabel->setAlignment(Qt::AlignVCenter);
  logoLayout->addWidget(appTitleLabel);

  logoLayout->addStretch();

  sidebarLayout->addWidget(logoContainer);

  QString buttonStyle = "QPushButton { "
                        "   background-color: #6FA85E; "
                        "   color: #FFFFFF; "
                        "   text-align: left; "
                        "   padding: 15px 20px; "
                        "   border: none; "
                        "   font-size: 15px; "
                        "}"
                        "QPushButton:hover { background-color: #7DB86D; }";

  QPushButton *dashboardBtn = new QPushButton("🏠  Tableau de bord");
  dashboardBtn->setStyleSheet(buttonStyle);
  connect(dashboardBtn, &QPushButton::clicked,
          [this]() { stackedWidget->setCurrentIndex(1); });
  sidebarLayout->addWidget(dashboardBtn);

  QPushButton *usersBtn = new QPushButton("👥  Gestion des utilisateurs");
  usersBtn->setStyleSheet(buttonStyle);
  connect(usersBtn, &QPushButton::clicked,
          [this]() { stackedWidget->setCurrentIndex(1); });
  sidebarLayout->addWidget(usersBtn);

  QPushButton *trucksBtn = new QPushButton("🚛  Gestion des camions");
  trucksBtn->setStyleSheet(buttonStyle);
  sidebarLayout->addWidget(trucksBtn);

  QPushButton *binsBtn = new QPushButton("🗑️  Gestion des poubelles");
  binsBtn->setStyleSheet(buttonStyle);
  sidebarLayout->addWidget(binsBtn);

  QPushButton *zonesBtn = new QPushButton("📍  Gestion des zones");
  zonesBtn->setStyleSheet(buttonStyle);
  connect(zonesBtn, &QPushButton::clicked, this, &MainWindow::onZonesClicked);
  sidebarLayout->addWidget(zonesBtn);

  QPushButton *recycleBtn = new QPushButton("♻️  Gestion de recyclage");
  recycleBtn->setStyleSheet(buttonStyle);
  connect(recycleBtn, &QPushButton::clicked, this,
          &MainWindow::onRecyclageClicked);
  sidebarLayout->addWidget(recycleBtn);

  QPushButton *collectBtn = new QPushButton("📋  Suivi des collectes");
  collectBtn->setStyleSheet(buttonStyle);
  sidebarLayout->addWidget(collectBtn);

  QPushButton *reportsBtn = new QPushButton("📊  Rapports");
  reportsBtn->setStyleSheet(buttonStyle);
  sidebarLayout->addWidget(reportsBtn);

  sidebarLayout->addStretch();

  QPushButton *settingsBtn = new QPushButton("⚙️  Parametres");
  settingsBtn->setStyleSheet(buttonStyle);
  sidebarLayout->addWidget(settingsBtn);

  return sidebar;
}

void MainWindow::setupUserManagementScreen() {
  QWidget *mainWidget = new QWidget();
  mainWidget->setStyleSheet("QWidget { background-color: #F5F5F5; }");

  QHBoxLayout *mainLayout = new QHBoxLayout(mainWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  mainLayout->addWidget(createSidebar());

  QWidget *contentArea = new QWidget();
  contentArea->setStyleSheet("QWidget { background-color: #F5F5F5; }");

  QVBoxLayout *contentLayout = new QVBoxLayout(contentArea);
  contentLayout->setContentsMargins(40, 30, 40, 30);
  contentLayout->setSpacing(20);

  // Top section with title and breadcrumb
  QWidget *topWidget = new QWidget();
  topWidget->setStyleSheet("QWidget { background-color: transparent; }");
  QVBoxLayout *topLayout = new QVBoxLayout(topWidget);
  topLayout->setContentsMargins(0, 0, 0, 0);
  topLayout->setSpacing(10);

  QWidget *titleWidget = new QWidget();
  titleWidget->setStyleSheet("QWidget { background-color: transparent; }");
  QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
  titleLayout->setContentsMargins(0, 0, 0, 0);

  QLabel *titleLabel = new QLabel("TuniWaste");
  titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: "
                            "#6FA85E; background: transparent;");
  titleLayout->addWidget(titleLabel);
  titleLayout->addStretch();

  QLabel *breadcrumbLabel = new QLabel("[HOME] Tableau de bord");
  breadcrumbLabel->setStyleSheet(
      "font-size: 14px; color: #999999; background-color: #FFFFFF; "
      "padding: 8px 15px; border: 1px solid #DDDDDD; border-radius: 4px;");
  titleLayout->addWidget(breadcrumbLabel);

  topLayout->addWidget(titleWidget);

  QLabel *pathLabel =
      new QLabel("Tableau de bord / Tableau - bord / Gestion des utilisateurs");
  pathLabel->setStyleSheet(
      "font-size: 13px; color: #999999; background: transparent;");
  topLayout->addWidget(pathLabel);

  contentLayout->addWidget(topWidget);

  QLabel *pageTitle = new QLabel("Gestion des utilisateurs");
  pageTitle->setStyleSheet("font-size: 28px; font-weight: bold; color: "
                           "#000000; background: transparent;");
  contentLayout->addWidget(pageTitle);

  // Search and Filter Section - Enhanced
  QWidget *searchFilterWidget = new QWidget();
  searchFilterWidget->setStyleSheet("QWidget { "
                                    "   background-color: #FFFFFF; "
                                    "   border: 1px solid #E0E0E0; "
                                    "   border-radius: 6px; "
                                    "   padding: 15px; "
                                    "}");
  QHBoxLayout *searchFilterLayout = new QHBoxLayout(searchFilterWidget);
  searchFilterLayout->setSpacing(15);

  // Search by ID
  QLineEdit *searchIdEdit = new QLineEdit();
  searchIdEdit->setPlaceholderText("🔍 Rechercher par ID...");
  searchIdEdit->setFixedWidth(180);
  searchIdEdit->setStyleSheet("QLineEdit { "
                              "   padding: 10px 15px; "
                              "   border: 2px solid #DDDDDD; "
                              "   border-radius: 6px; "
                              "   background-color: #FFFFFF; "
                              "   color: #000000; "
                              "   font-size: 14px; "
                              "}"
                              "QLineEdit:focus { border: 2px solid #6FA85E; }");
  connect(searchIdEdit, &QLineEdit::textChanged,
          [this](const QString &text) { searchEdit->setText(text); });
  searchFilterLayout->addWidget(searchIdEdit);

  // Search by Name
  QLineEdit *searchNameEdit = new QLineEdit();
  searchNameEdit->setPlaceholderText("🔍 Rechercher par Nom...");
  searchNameEdit->setFixedWidth(200);
  searchNameEdit->setStyleSheet(
      "QLineEdit { "
      "   padding: 10px 15px; "
      "   border: 2px solid #DDDDDD; "
      "   border-radius: 6px; "
      "   background-color: #FFFFFF; "
      "   color: #000000; "
      "   font-size: 14px; "
      "}"
      "QLineEdit:focus { border: 2px solid #6FA85E; }");
  connect(searchNameEdit, &QLineEdit::textChanged,
          [this](const QString &text) { searchEdit->setText(text); });
  searchFilterLayout->addWidget(searchNameEdit);

  // Search by Email
  QLineEdit *searchEmailEdit = new QLineEdit();
  searchEmailEdit->setPlaceholderText("🔍 Rechercher par Email...");
  searchEmailEdit->setFixedWidth(220);
  searchEmailEdit->setStyleSheet(
      "QLineEdit { "
      "   padding: 10px 15px; "
      "   border: 2px solid #DDDDDD; "
      "   border-radius: 6px; "
      "   background-color: #FFFFFF; "
      "   color: #000000; "
      "   font-size: 14px; "
      "}"
      "QLineEdit:focus { border: 2px solid #6FA85E; }");
  connect(searchEmailEdit, &QLineEdit::textChanged,
          [this](const QString &text) { searchEdit->setText(text); });
  searchFilterLayout->addWidget(searchEmailEdit);

  searchFilterLayout->addStretch();

  // Sort Options
  QLabel *sortLabel = new QLabel("Trier par:");
  sortLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: "
                           "#000000; background: transparent;");
  searchFilterLayout->addWidget(sortLabel);

  QComboBox *sortCombo = new QComboBox();
  sortCombo->addItem("Plus récent (ID ↓)");
  sortCombo->addItem("Plus ancien (ID ↑)");
  sortCombo->addItem("Nom (A-Z)");
  sortCombo->addItem("Nom (Z-A)");
  sortCombo->addItem("Email (A-Z)");
  sortCombo->addItem("Email (Z-A)");
  sortCombo->setFixedWidth(200);
  sortCombo->setStyleSheet("QComboBox { "
                           "   padding: 10px 15px; "
                           "   border: 2px solid #DDDDDD; "
                           "   border-radius: 6px; "
                           "   background-color: #FFFFFF; "
                           "   color: #000000; "
                           "   font-size: 14px; "
                           "   font-weight: bold; "
                           "}"
                           "QComboBox:focus { border: 2px solid #6FA85E; }"
                           "QComboBox::drop-down { border: none; width: 30px; }"
                           "QComboBox QAbstractItemView { "
                           "   background-color: #FFFFFF; "
                           "   color: #000000; "
                           "   selection-background-color: #6FA85E; "
                           "   selection-color: #FFFFFF; "
                           "   border: 1px solid #DDDDDD; "
                           "   padding: 5px; "
                           "}");
  connect(
      sortCombo,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      [this](int index) {
        switch (index) {
        case 0: // Plus récent (ID descendant)
          currentSortColumn = 0;
          currentSortOrder = Qt::DescendingOrder;
          break;
        case 1: // Plus ancien (ID ascendant)
          currentSortColumn = 0;
          currentSortOrder = Qt::AscendingOrder;
          break;
        case 2: // Nom A-Z
          currentSortColumn = 1;
          currentSortOrder = Qt::AscendingOrder;
          break;
        case 3: // Nom Z-A
          currentSortColumn = 1;
          currentSortOrder = Qt::DescendingOrder;
          break;
        case 4: // Email A-Z
          currentSortColumn = 2;
          currentSortOrder = Qt::AscendingOrder;
          break;
        case 5: // Email Z-A
          currentSortColumn = 2;
          currentSortOrder = Qt::DescendingOrder;
          break;
        }
        filterAndSortUsers();
      });
  searchFilterLayout->addWidget(sortCombo);

  contentLayout->addWidget(searchFilterWidget);

  // Search and Add User Section
  QWidget *actionWidget = new QWidget();
  actionWidget->setStyleSheet("QWidget { background-color: transparent; }");
  QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
  actionLayout->setContentsMargins(0, 0, 0, 0);
  actionLayout->setSpacing(15);

  // Hidden main search (for backend filtering)
  searchEdit = new QLineEdit();
  searchEdit->setVisible(false);
  actionLayout->addWidget(searchEdit);

  // Filter Dropdown
  roleFilter = new QComboBox();
  roleFilter->addItem("Tous les roles");
  roleFilter->addItem("Administrateur");
  roleFilter->addItem("Employe");
  roleFilter->setFixedWidth(180);
  roleFilter->setStyleSheet(
      "QComboBox { "
      "   padding: 10px 15px; "
      "   border: 1px solid #DDDDDD; "
      "   border-radius: 4px; "
      "   background-color: #FFFFFF; "
      "   color: #000000; "
      "   font-size: 14px; "
      "}"
      "QComboBox:focus { border: 1px solid #629952; }"
      "QComboBox::drop-down { border: none; width: 30px; }"
      "QComboBox QAbstractItemView { "
      "   background-color: #FFFFFF; "
      "   color: #000000; "
      "   selection-background-color: #629952; "
      "   selection-color: #FFFFFF; "
      "   border: 1px solid #DDDDDD; "
      "}");
  connect(
      roleFilter,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      [this](int) { filterAndSortUsers(); });
  actionLayout->addWidget(roleFilter);

  actionLayout->addStretch();

  QPushButton *addUserBtn = new QPushButton("+ Ajouter utilisateur");
  addUserBtn->setStyleSheet("QPushButton { "
                            "   background-color: #A3D977; "
                            "   color: #FFFFFF; "
                            "   padding: 10px 25px; "
                            "   border: none; "
                            "   border-radius: 4px; "
                            "   font-weight: bold; "
                            "   font-size: 14px; "
                            "}"
                            "QPushButton:hover { background-color: #8FC65E; }");
  connect(addUserBtn, &QPushButton::clicked, this,
          &MainWindow::onAddUserClicked);

  QPushButton *btnPdf = new QPushButton("📄 Exporter PDF");
    btnPdf->setStyleSheet(
        "QPushButton { "
        "   background-color: #FF9800; "
        "   color: #FFFFFF; "
        "   padding: 10px 25px; "
        "   border: none; "
        "   border-radius: 4px; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #F57C00; }"
        );
    actionLayout->addWidget(btnPdf);
    actionLayout->addWidget(addUserBtn);

    contentLayout->addWidget(actionWidget);

    connect(btnPdf, &QPushButton::clicked, this, [this](){
    QString fileName = QFileDialog::getSaveFileName(
        this, "Exporter les Utilisateurs en PDF", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty())
      return;
    if (QFileInfo(fileName).suffix().isEmpty())
      fileName.append(".pdf");

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setResolution(300);
    writer.setPageMargins(QMarginsF(15, 15, 15, 15));

    QPainter painter(&writer);
    painter.setRenderHint(QPainter::Antialiasing);

    int width = writer.width();
    int headerHeight = 350;

    // Header
    painter.fillRect(QRect(0, 0, width, headerHeight), QColor("#6FA85E"));
    painter.setPen(Qt::white);
    painter.setFont(QFont("Segoe UI", 36, QFont::ExtraBold));
    painter.drawText(QRect(50, 0, width - 100, headerHeight),
                     Qt::AlignVCenter | Qt::AlignLeft, "TuniWaste");
    painter.setFont(QFont("Segoe UI", 16, QFont::Bold));
    painter.drawText(QRect(50, 0, width - 100, headerHeight),
                     Qt::AlignVCenter | Qt::AlignRight,
                     "RAPPORT DE GESTION\nDES UTILISATEURS");

    int y = headerHeight + 100;

    // Date
    painter.setPen(Qt::black);
    painter.setFont(QFont("Segoe UI", 12));
    painter.drawText(50, y,
                     "Date du rapport: " +
                         QDate::currentDate().toString("dd/MM/yyyy"));
    y += 150;

    // Stats
    int adminCount = 0;
    int employeeCount = 0;
    for (const auto &u : filteredUsers) {
      if (u.role == "Administrateur")
        adminCount++;
      else
        employeeCount++;
    }

    // General Stats Section
    painter.setPen(QColor("#6FA85E"));
    painter.setFont(QFont("Segoe UI", 18, QFont::Bold));
    painter.drawText(50, y, "STATISTIQUES DES COMPTES");
    QPen greenPen(QColor("#6FA85E"));
    greenPen.setWidth(3);
    painter.setPen(greenPen);
    painter.drawLine(50, y + 30, width - 50, y + 30);
    y += 100;

    painter.setPen(Qt::black);
    painter.setFont(QFont("Segoe UI", 14));
    painter.drawText(80, y,
                     "• Nombre total d'utilisateurs (filtrés): " +
                         QString::number(filteredUsers.size()));
    y += 60;
    painter.drawText(80, y,
                     "• Administrateurs: " + QString::number(adminCount));
    y += 60;
    painter.drawText(80, y, "• Employés: " + QString::number(employeeCount));
    y += 150;

    // Users Table
    painter.setPen(QColor("#6FA85E"));
    painter.setFont(QFont("Segoe UI", 18, QFont::Bold));
    painter.drawText(50, y, "LISTE DES UTILISATEURS");
    painter.setPen(greenPen);
    painter.drawLine(50, y + 30, width - 50, y + 30);
    y += 100;

    int tableIndent = 50;
    int colID = tableIndent;
    int colNom = tableIndent + 300;
    int colEmail = tableIndent + 1200;
    int colRole = tableIndent + 2200;
    int rowHeight = 80;

    // Table Header
    painter.fillRect(QRect(tableIndent - 20, y - 10,
                           width - (2 * tableIndent) + 40, rowHeight),
                     QColor("#F8F8F8"));
    painter.setPen(Qt::black);
    painter.setFont(QFont("Segoe UI", 13, QFont::Bold));
    painter.drawText(colID, y + 55, "ID");
    painter.drawText(colNom, y + 55, "Nom et Prénom");
    painter.drawText(colEmail, y + 55, "Email");
    painter.drawText(colRole, y + 55, "Rôle");
    y += rowHeight;

    painter.setFont(QFont("Segoe UI", 11));
    painter.setPen(Qt::black);
    QPen linePen(QColor("#E0E0E0"));
    linePen.setWidth(1);

    for (const auto &u : filteredUsers) {
      // New Page check
      if (y > writer.height() - 200) {
        writer.newPage();
        y = 100;
      }

      painter.setPen(Qt::black);
      painter.drawText(colID, y + 55, QString::number(u.id));
      painter.drawText(colNom, y + 55, u.name);
      painter.drawText(colEmail, y + 55, u.email);
      painter.drawText(colRole, y + 55, u.role);

      painter.setPen(linePen);
      painter.drawLine(tableIndent - 20, y + rowHeight,
                       width - tableIndent + 20, y + rowHeight);
      y += rowHeight;
    }

    // Footer
    painter.setPen(Qt::gray);
    painter.setFont(QFont("Segoe UI", 10));
    painter.drawText(
        QRect(0, writer.height() - 150, width, 50), Qt::AlignCenter,
        "TuniWaste - Gestion des Utilisateurs - Document Officiel - " +
            QDate::currentDate().toString("yyyy"));
    painter.end();

    QMessageBox::information(
        this, "Succès",
        "Le rapport des utilisateurs a été généré avec succès.");
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    });

    actionLayout->addWidget(btnPdf);
    actionLayout->addWidget(addUserBtn);

    contentLayout->addWidget(actionWidget);

    // User Table
    userTable = new QTableWidget();
    userTable->setColumnCount(6);
    userTable->setHorizontalHeaderLabels({"ID", "Nom", "Email", "Telephone", "Role", "Actions"});
    userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    userTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    userTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    userTable->horizontalHeader()->resizeSection(5, 180); // Fixed width for Actions column
    userTable->verticalHeader()->setVisible(false);
    userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTable->setSelectionMode(QAbstractItemView::SingleSelection);
    userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    userTable->setAlternatingRowColors(true);
    userTable->setSortingEnabled(true);
    userTable->setShowGrid(true);

    userTable->setStyleSheet(
        "QTableWidget { "
        "   background-color: #FFFFFF; "
        "   border: 1px solid #E0E0E0; "
        "   gridline-color: #E5E5E5; "
        "   color: #000000; "
        "   font-size: 14px; "
        "}"
        "QTableWidget::item { "
        "   padding: 15px 10px; "
        "   border-bottom: 1px solid #E5E5E5; "
        "}"
        "QTableWidget::item:selected { background-color: #F0F7ED; color: #000000; }"
        "QHeaderView::section { "
        "   background-color: #F8F8F8; "
        "   color: #000000; "
        "   padding: 15px 10px; "
        "   border: none; "
        "   border-bottom: 2px solid #E0E0E0; "
        "   border-right: 1px solid #E5E5E5; "
        "   font-weight: bold; "
        "   font-size: 15px; "
        "}"
        "QTableWidget::item:alternate { background-color: #FAFAFA; }"
        );

    connect(userTable->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::onSortByColumn);

    contentLayout->addWidget(userTable);

    mainLayout->addWidget(contentArea);

    updateUserTable();

    stackedWidget->addWidget(mainWidget);
}

void MainWindow::updateUserTable() {
  userTable->setRowCount(0);
  userTable->setSortingEnabled(false);
  userTable->setRowCount(filteredUsers.size());

  for (int i = 0; i < filteredUsers.size(); ++i) {
    const User &user = filteredUsers[i];

    QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(user.id));
    idItem->setTextAlignment(Qt::AlignCenter);
    QFont idFont;
    idFont.setPointSize(14);
    idItem->setFont(idFont);
    userTable->setItem(i, 0, idItem);

    QTableWidgetItem *nameItem = new QTableWidgetItem(user.name);
    QFont nameFont;
    nameFont.setPointSize(14);
    nameItem->setFont(nameFont);
    userTable->setItem(i, 1, nameItem);

    QTableWidgetItem *emailItem = new QTableWidgetItem(user.email);
    QFont emailFont;
    emailFont.setPointSize(14);
    emailItem->setFont(emailFont);
    userTable->setItem(i, 2, emailItem);

    QTableWidgetItem *phoneItem = new QTableWidgetItem(user.phone);
    QFont phoneFont;
    phoneFont.setPointSize(14);
    phoneItem->setFont(phoneFont);
    userTable->setItem(i, 3, phoneItem);

    // Role with colored badge
    QWidget *roleWidget = new QWidget();
    QHBoxLayout *roleLayout = new QHBoxLayout(roleWidget);
    roleLayout->setContentsMargins(5, 5, 5, 5);
    roleLayout->setAlignment(Qt::AlignCenter);

    QLabel *roleLabel = new QLabel(user.role);
    if (user.role == "Administrateur") {
      roleLabel->setStyleSheet("background-color: #FFA726; "
                               "color: #FFFFFF; "
                               "padding: 8px 20px; "
                               "border-radius: 4px; "
                               "font-weight: bold; "
                               "font-size: 13px;");
    } else {
      roleLabel->setStyleSheet("background-color: #5DADE2; "
                               "color: #FFFFFF; "
                               "padding: 8px 20px; "
                               "border-radius: 4px; "
                               "font-weight: bold; "
                               "font-size: 13px;");
    }
    roleLayout->addWidget(roleLabel);

    // Create a placeholder item for sorting
    QTableWidgetItem *roleSortItem = new QTableWidgetItem(user.role);
    userTable->setItem(i, 4, roleSortItem);
    userTable->setCellWidget(i, 4, roleWidget);

    // Actions with styled buttons - Vertical layout
    QWidget *actionWidget = new QWidget();
    QVBoxLayout *actionLayout = new QVBoxLayout(actionWidget);
    actionLayout->setContentsMargins(10, 8, 10, 8);
    actionLayout->setSpacing(10);
    actionLayout->setAlignment(Qt::AlignCenter);

    QPushButton *editBtn = new QPushButton("Modifier");
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setFixedSize(140, 35);
    editBtn->setStyleSheet("QPushButton { "
                           "   background-color: #4CAF50; "
                           "   color: #FFFFFF; "
                           "   padding: 8px 15px; "
                           "   border: none; "
                           "   border-radius: 6px; "
                           "   font-size: 14px; "
                           "   font-weight: bold; "
                           "}"
                           "QPushButton:hover { "
                           "   background-color: #45A049; "
                           "}");
    connect(editBtn, &QPushButton::clicked, [this, i]() { onModifyUser(i); });

    QPushButton *deleteBtn = new QPushButton("Supprimer");
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setFixedSize(140, 35);
    deleteBtn->setStyleSheet("QPushButton { "
                             "   background-color: #F44336; "
                             "   color: #FFFFFF; "
                             "   padding: 8px 15px; "
                             "   border: none; "
                             "   border-radius: 6px; "
                             "   font-size: 14px; "
                             "   font-weight: bold; "
                             "}"
                             "QPushButton:hover { "
                             "   background-color: #DA190B; "
                             "}");
    connect(deleteBtn, &QPushButton::clicked, [this, i]() { onDeleteUser(i); });

    actionLayout->addWidget(editBtn);
    actionLayout->addWidget(deleteBtn);

    userTable->setCellWidget(i, 5, actionWidget);
  }

  // Set row heights to accommodate buttons
  for (int i = 0; i < userTable->rowCount(); ++i) {
    userTable->setRowHeight(i, 110);
  }

  userTable->setSortingEnabled(true);
}

void MainWindow::onSearchTextChanged(const QString &text) {
  filterAndSortUsers();
}

void MainWindow::onSortByColumn(int column) {
  if (currentSortColumn == column) {
    currentSortOrder = (currentSortOrder == Qt::AscendingOrder)
                           ? Qt::DescendingOrder
                           : Qt::AscendingOrder;
  } else {
    currentSortColumn = column;
    currentSortOrder = Qt::AscendingOrder;
  }
  filterAndSortUsers();
}

void MainWindow::filterAndSortUsers() {
  QString searchText = searchEdit->text().trimmed().toLower();
  QString selectedRole = roleFilter->currentText();

  filteredUsers.clear();

  for (const User &user : users) {
    // Apply text search filter (searches in all fields)
    bool matchesSearch = searchText.isEmpty() ||
                         QString::number(user.id).contains(searchText) ||
                         user.name.toLower().contains(searchText) ||
                         user.email.toLower().contains(searchText) ||
                         user.phone.contains(searchText) ||
                         user.role.toLower().contains(searchText);

    // Apply role filter
    bool matchesRole =
        (selectedRole == "Tous les roles") || (user.role == selectedRole);

    // Include user if it matches both filters
    if (matchesSearch && matchesRole) {
      filteredUsers.append(user);
    }
  }

  // Sort filtered users
  if (currentSortColumn >= 0 && currentSortColumn < 5) {
    std::sort(filteredUsers.begin(), filteredUsers.end(),
              [this](const User &a, const User &b) {
                QString valA, valB;
                switch (currentSortColumn) {
                case 0:
                  // For ID, compare as numbers for proper sorting
                  return (currentSortOrder == Qt::AscendingOrder)
                             ? (a.id < b.id)
                             : (a.id > b.id);
                case 1:
                  valA = a.name.toLower();
                  valB = b.name.toLower();
                  break;
                case 2:
                  valA = a.email.toLower();
                  valB = b.email.toLower();
                  break;
                case 3:
                  valA = a.phone;
                  valB = b.phone;
                  break;
                case 4:
                  valA = a.role;
                  valB = b.role;
                  break;
                }
                if (currentSortColumn != 0) {
                  return (currentSortOrder == Qt::AscendingOrder)
                             ? (valA < valB)
                             : (valA > valB);
                }
                return false; // Should not reach here
              });
  }

  updateUserTable();
}

void MainWindow::onLoginClicked() { stackedWidget->setCurrentIndex(1); }

void MainWindow::onAddUserClicked() {
  AddUserDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    User newUser;
    newUser.id = nextUserId++;
    newUser.name = dialog.getName();
    newUser.email = dialog.getEmail();
    newUser.phone = dialog.getPhone();
    newUser.role = dialog.getRole();

    users.append(newUser);
    filterAndSortUsers();

    QMessageBox::information(this, "Succes",
                             "Utilisateur ajoute avec succes !");
  }
}

void MainWindow::onModifyUser(int row) {
  if (row < 0 || row >= filteredUsers.size())
    return;

  User &user = filteredUsers[row];

  // Find the actual user in the main users list
  int actualIndex = -1;
  for (int i = 0; i < users.size(); ++i) {
    if (users[i].id == user.id) {
      actualIndex = i;
      break;
    }
  }

  if (actualIndex == -1)
    return;

  AddUserDialog dialog(this, &user);
  if (dialog.exec() == QDialog::Accepted) {
    users[actualIndex].name = dialog.getName();
    users[actualIndex].email = dialog.getEmail();
    users[actualIndex].phone = dialog.getPhone();
    users[actualIndex].role = dialog.getRole();

    filterAndSortUsers();

    QMessageBox::information(this, "Succes",
                             "Utilisateur modifie avec succes !");
  }
}

void MainWindow::onDeleteUser(int row) {
  if (row < 0 || row >= filteredUsers.size())
    return;

  User &user = filteredUsers[row];

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(
      this, "Confirmation",
      "Etes-vous sur de vouloir supprimer l'utilisateur '" + user.name + "' ?",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    // Find and remove from main users list
    for (int i = 0; i < users.size(); ++i) {
      if (users[i].id == user.id) {
        users.removeAt(i);
        break;
      }
    }

    filterAndSortUsers();

    QMessageBox::information(this, "Succes",
                             "Utilisateur supprime avec succes !");
  }
}

AddUserDialog::AddUserDialog(QWidget *parent, User *editUser)
    : QDialog(parent) {
  setWindowTitle(editUser ? "Modifier l'utilisateur"
                          : "Ajouter un utilisateur");
  setModal(true);
  setFixedSize(500, 450);

  setStyleSheet("QDialog { background-color: #FFFFFF; }"
                "QLabel { color: #000000; background: transparent; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(15);
  mainLayout->setContentsMargins(30, 30, 30, 30);

  QLabel *titleLabel =
      new QLabel(editUser ? "Modifier l'utilisateur" : "Nouvel utilisateur");
  titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: "
                            "#000000; background: transparent;");
  titleLabel->setAlignment(Qt::AlignCenter);
  mainLayout->addWidget(titleLabel);

  mainLayout->addSpacing(10);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setSpacing(15);
  formLayout->setLabelAlignment(Qt::AlignRight);

  QLabel *nameLabel = new QLabel("Nom complet :");
  nameLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: "
                           "#000000; background: transparent;");

  nameEdit = new QLineEdit();
  nameEdit->setPlaceholderText("Nom Prenom");
  if (editUser)
    nameEdit->setText(editUser->name);
  nameEdit->setStyleSheet("QLineEdit { "
                          "   padding: 10px; "
                          "   border: 2px solid #CCCCCC; "
                          "   border-radius: 6px; "
                          "   background-color: #FFFFFF; "
                          "   color: #000000; "
                          "   font-size: 14px; "
                          "}"
                          "QLineEdit:focus { border: 2px solid #A3C651; }");

  QLabel *emailLabel = new QLabel("Email :");
  emailLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: "
                            "#000000; background: transparent;");

  emailEdit = new QLineEdit();
  emailEdit->setPlaceholderText("nom@example.com");
  if (editUser)
    emailEdit->setText(editUser->email);
  emailEdit->setStyleSheet("QLineEdit { "
                           "   padding: 10px; "
                           "   border: 2px solid #CCCCCC; "
                           "   border-radius: 6px; "
                           "   background-color: #FFFFFF; "
                           "   color: #000000; "
                           "   font-size: 14px; "
                           "}"
                           "QLineEdit:focus { border: 2px solid #A3C651; }");

  QLabel *phoneLabel = new QLabel("Telephone :");
  phoneLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: "
                            "#000000; background: transparent;");

  phoneEdit = new QLineEdit();
  phoneEdit->setPlaceholderText("+216 XX XXX XXX");
  if (editUser)
    phoneEdit->setText(editUser->phone);
  phoneEdit->setStyleSheet("QLineEdit { "
                           "   padding: 10px; "
                           "   border: 2px solid #CCCCCC; "
                           "   border-radius: 6px; "
                           "   background-color: #FFFFFF; "
                           "   color: #000000; "
                           "   font-size: 14px; "
                           "}"
                           "QLineEdit:focus { border: 2px solid #A3C651; }");

  QLabel *roleLabel = new QLabel("Role :");
  roleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: "
                           "#000000; background: transparent;");

  roleCombo = new QComboBox();
  roleCombo->addItem("Administrateur");
  roleCombo->addItem("Employe");
  if (editUser) {
    int index = roleCombo->findText(editUser->role);
    if (index >= 0)
      roleCombo->setCurrentIndex(index);
  }
  roleCombo->setStyleSheet("QComboBox { "
                           "   padding: 10px; "
                           "   border: 2px solid #CCCCCC; "
                           "   border-radius: 6px; "
                           "   background-color: #FFFFFF; "
                           "   color: #000000; "
                           "   font-size: 14px; "
                           "}"
                           "QComboBox:focus { border: 2px solid #A3C651; }"
                           "QComboBox::drop-down { border: none; width: 30px; }"
                           "QComboBox QAbstractItemView { "
                           "   background-color: #FFFFFF; "
                           "   color: #000000; "
                           "   selection-background-color: #A3C651; "
                           "   selection-color: #FFFFFF; "
                           "   border: 1px solid #CCCCCC; "
                           "}");

  formLayout->addRow(nameLabel, nameEdit);
  formLayout->addRow(emailLabel, emailEdit);
  formLayout->addRow(phoneLabel, phoneEdit);
  formLayout->addRow(roleLabel, roleCombo);

  mainLayout->addLayout(formLayout);
  mainLayout->addSpacing(10);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setSpacing(10);

  QPushButton *cancelBtn = new QPushButton("Annuler");
  cancelBtn->setStyleSheet("QPushButton { "
                           "   background-color: #FFFFFF; "
                           "   color: #000000; "
                           "   padding: 12px 24px; "
                           "   border: 2px solid #CCCCCC; "
                           "   border-radius: 6px; "
                           "   font-weight: bold; "
                           "   font-size: 14px; "
                           "}"
                           "QPushButton:hover { "
                           "   background-color: #F5F5F5; "
                           "   border: 2px solid #999999; "
                           "}");
  connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

  QPushButton *saveBtn = new QPushButton(editUser ? "Modifier" : "Ajouter");
  saveBtn->setStyleSheet("QPushButton { "
                         "   background-color: #A3C651; "
                         "   color: #FFFFFF; "
                         "   padding: 12px 24px; "
                         "   border: none; "
                         "   border-radius: 6px; "
                         "   font-weight: bold; "
                         "   font-size: 14px; "
                         "}"
                         "QPushButton:hover { background-color: #8FB544; }");
  connect(saveBtn, &QPushButton::clicked, this, &QDialog::accept);

  buttonLayout->addStretch();
  buttonLayout->addWidget(cancelBtn);
  buttonLayout->addWidget(saveBtn);

  mainLayout->addLayout(buttonLayout);
}

void MainWindow::onForgotPasswordClicked() {
  PasswordResetDialog dialog(this);
  dialog.exec();
}

PasswordResetDialog::PasswordResetDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle("Réinitialiser le mot de passe");
  setModal(true);
  setFixedSize(450, 280);

  setStyleSheet("QDialog { background-color: #FFFFFF; }"
                "QLabel { color: #000000; background: transparent; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(20);
  mainLayout->setContentsMargins(30, 30, 30, 30);

  QLabel *iconLabel = new QLabel("🔒");
  iconLabel->setStyleSheet("font-size: 48px; background: transparent;");
  iconLabel->setAlignment(Qt::AlignCenter);
  mainLayout->addWidget(iconLabel);

  QLabel *titleLabel = new QLabel("Mot de passe oublié ?");
  titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: "
                            "#000000; background: transparent;");
  titleLabel->setAlignment(Qt::AlignCenter);
  mainLayout->addWidget(titleLabel);

  QLabel *descLabel = new QLabel(
      "Entrez votre adresse email pour recevoir un lien de réinitialisation.");
  descLabel->setStyleSheet(
      "font-size: 13px; color: #666666; background: transparent;");
  descLabel->setWordWrap(true);
  descLabel->setAlignment(Qt::AlignCenter);
  mainLayout->addWidget(descLabel);

  QLabel *emailLabel = new QLabel("Email :");
  emailLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: "
                            "#000000; background: transparent;");
  mainLayout->addWidget(emailLabel);

  emailEdit = new QLineEdit();
  emailEdit->setPlaceholderText("nom@example.com");
  emailEdit->setStyleSheet("QLineEdit { "
                           "   padding: 12px; "
                           "   border: 2px solid #CCCCCC; "
                           "   border-radius: 6px; "
                           "   background-color: #FFFFFF; "
                           "   color: #000000; "
                           "   font-size: 14px; "
                           "}"
                           "QLineEdit:focus { border: 2px solid #A3C651; }");
  mainLayout->addWidget(emailEdit);

  mainLayout->addSpacing(10);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setSpacing(10);

  QPushButton *cancelBtn = new QPushButton("Annuler");
  cancelBtn->setStyleSheet("QPushButton { "
                           "   background-color: #FFFFFF; "
                           "   color: #000000; "
                           "   padding: 12px 24px; "
                           "   border: 2px solid #CCCCCC; "
                           "   border-radius: 6px; "
                           "   font-weight: bold; "
                           "   font-size: 14px; "
                           "}"
                           "QPushButton:hover { "
                           "   background-color: #F5F5F5; "
                           "   border: 2px solid #999999; "
                           "}");
  connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

  QPushButton *resetBtn = new QPushButton("Envoyer");
  resetBtn->setStyleSheet("QPushButton { "
                          "   background-color: #A3C651; "
                          "   color: #FFFFFF; "
                          "   padding: 12px 24px; "
                          "   border: none; "
                          "   border-radius: 6px; "
                          "   font-weight: bold; "
                          "   font-size: 14px; "
                          "}"
                          "QPushButton:hover { background-color: #8FB544; }");
  connect(resetBtn, &QPushButton::clicked, this,
          &PasswordResetDialog::onResetClicked);

  buttonLayout->addStretch();
  buttonLayout->addWidget(cancelBtn);
  buttonLayout->addWidget(resetBtn);

  mainLayout->addLayout(buttonLayout);
}

void PasswordResetDialog::onResetClicked() {
  QString email = emailEdit->text().trimmed();

  if (email.isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez entrer votre adresse email !");
    return;
  }

  if (!email.contains("@") || !email.contains(".")) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez entrer une adresse email valide !");
    return;
  }

  QMessageBox::information(
      this, "Succès",
      "Un email de réinitialisation a été envoyé à " + email +
          "\n\nVeuillez vérifier votre boîte de réception.");

  accept();
}

void MainWindow::onRecyclageClicked() {
  Recyclage *recyclageWindow = new Recyclage();
  recyclageWindow->setAttribute(Qt::WA_DeleteOnClose);
  recyclageWindow->showMaximized();
}

void MainWindow::onZonesClicked() {
  GestionZones *zonesWindow = new GestionZones();
  zonesWindow->setAttribute(Qt::WA_DeleteOnClose);
  zonesWindow->showMaximized();
}
