#include "mainwindow.h"
#include "Dashboard.h"
#include "GestionCamions.h"
#include "GestionPoubelles.h"
#include "RECYCLAGE.h"
#include "gestionzones.h"
#include <QCoreApplication>
#include <QDate>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>
#include <QScrollArea>
#include <QUrl>
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), nextUserId(5), currentSortColumn(-1),
      currentSortOrder(Qt::AscendingOrder), currentUserEditingRow(-1) {

  // Initialize Dummy Data
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
  setupAppShell();

  // Initialize screens
  setupUserManagementScreen();
  setupRecyclingScreen();
  setupZonesScreen();
  setupTrucksScreen();
  setupBinsScreen();
  setupDashboardScreen();

  stackedWidget->setCurrentIndex(0); // Start with login
}

MainWindow::~MainWindow() {}

// =========================================================
// LOGIN SCREEN
// =========================================================

void MainWindow::setupLoginScreen() {
  QWidget *loginWidget = new QWidget();
  loginWidget->setStyleSheet("QWidget { background-color: #E8F5E9; }");

  QGridLayout *mainLayout = new QGridLayout(loginWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setRowStretch(0, 1);
  mainLayout->setRowStretch(2, 1);
  mainLayout->setColumnStretch(0, 1);
  mainLayout->setColumnStretch(2, 1);

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
  if (logo.load(":/logo.png") || logo.load("logo.png") ||
      logo.load("./logo.png")) {
    QImage img = logo.toImage().convertToFormat(QImage::Format_ARGB32);
    for (int y = 0; y < img.height(); ++y) {
      for (int x = 0; x < img.width(); ++x) {
        QRgb pixel = img.pixel(x, y);
        if (qRed(pixel) >= 250 && qGreen(pixel) >= 250 && qBlue(pixel) >= 250) {
          img.setPixel(x, y, qRgba(0, 0, 0, 0));
        }
      }
    }
    logo = QPixmap::fromImage(img);
    logoLabel->setPixmap(
        logo.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  } else {
    logoLabel->setText("🗑️");
    logoLabel->setStyleSheet("font-size: 60px;");
  }
  logoLabel->setAlignment(Qt::AlignCenter);
  logoTitleLayout->addWidget(logoLabel);

  QLabel *titleLogoLabel = new QLabel("TuniWaste");
  titleLogoLabel->setStyleSheet(
      "font-size: 32px; font-weight: bold; color: #6FA85E;");
  logoTitleLayout->addWidget(titleLogoLabel);

  cardLayout->addWidget(logoTitleWidget);

  QLabel *titleLabel = new QLabel("Bienvenue sur TuniWaste");
  titleLabel->setStyleSheet(
      "font-size: 16px; font-weight: bold; color: #000000; margin: 10px 0px;");
  titleLabel->setAlignment(Qt::AlignCenter);
  cardLayout->addWidget(titleLabel);

  cardLayout->addWidget(new QLabel("Email"));
  emailEdit = new QLineEdit();
  emailEdit->setPlaceholderText("nom@example.com");
  emailEdit->setStyleSheet("QLineEdit { padding: 12px; border: 2px solid "
                           "#CCCCCC; border-radius: 8px; }");
  cardLayout->addWidget(emailEdit);

  cardLayout->addWidget(new QLabel("Mot de passe"));
  passwordEdit = new QLineEdit();
  passwordEdit->setPlaceholderText("Password");
  passwordEdit->setEchoMode(QLineEdit::Password);
  passwordEdit->setStyleSheet("QLineEdit { padding: 12px; border: 2px solid "
                              "#CCCCCC; border-radius: 8px; }");
  cardLayout->addWidget(passwordEdit);

  QPushButton *loginButton = new QPushButton("Connexion");
  loginButton->setStyleSheet(
      "QPushButton { background-color: #A3C651; color: white; padding: 14px; "
      "border-radius: 8px; font-weight: bold; }"
      "QPushButton:hover { background-color: #8FB544; }");
  connect(loginButton, &QPushButton::clicked, this,
          &MainWindow::onLoginClicked);
  cardLayout->addWidget(loginButton);

  QPushButton *forgotButton = new QPushButton("Mot de passe oublié ?");
  forgotButton->setStyleSheet(
      "QPushButton { color: #5A8F47; background: transparent; border: none; "
      "text-decoration: underline; }");
  forgotButton->setCursor(Qt::PointingHandCursor);
  connect(forgotButton, &QPushButton::clicked, this,
          &MainWindow::onForgotPasswordClicked);

  QHBoxLayout *forgotLayout = new QHBoxLayout();
  forgotLayout->addStretch();
  forgotLayout->addWidget(forgotButton);
  forgotLayout->addStretch();
  cardLayout->addLayout(forgotLayout);

  mainLayout->addWidget(card, 1, 1);
  stackedWidget->addWidget(loginWidget);
}

void MainWindow::onLoginClicked() {
  QString email = emailEdit->text();
  QString password = passwordEdit->text();

  if (email.isEmpty() || password.isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez entrer votre email et mot de passe.");
    return;
  }

  // Simple validation for demo
  if (email == "admin" || (email.contains("@") && password.length() >= 4)) {
    stackedWidget->setCurrentIndex(1); // Show App Shell
    onDashboardClicked();
  } else {
    QMessageBox::critical(this, "Acces refuse", "Identifiants invalides.");
  }
}

void MainWindow::onForgotPasswordClicked() {
  PasswordResetDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    QMessageBox::information(this, "Réinitialisation",
                             "Un lien de réinitialisation a été envoyé à : " +
                                 dialog.getEmail());
  }
}

// =========================================================
// APP SHELL & SIDEBAR
// =========================================================

void MainWindow::setupAppShell() {
  QWidget *appShell = new QWidget();
  QHBoxLayout *shellLayout = new QHBoxLayout(appShell);
  shellLayout->setContentsMargins(0, 0, 0, 0);
  shellLayout->setSpacing(0);

  shellLayout->addWidget(createSidebar());

  contentStackedWidget = new QStackedWidget();
  shellLayout->addWidget(contentStackedWidget);

  stackedWidget->addWidget(appShell);
}

QWidget *MainWindow::createSidebar() {
  QWidget *sidebar = new QWidget();
  sidebar->setFixedWidth(280);
  sidebar->setStyleSheet(
      "QWidget { background-color: #66BB6A; }"); // Lighter green palette

  QVBoxLayout *layout = new QVBoxLayout(sidebar);
  layout->setContentsMargins(0, 20, 0, 20);
  layout->setSpacing(5);

  // Logo + Title
  QWidget *logoContainer = new QWidget();
  logoContainer->setStyleSheet("background: transparent;");
  QHBoxLayout *logoLayout = new QHBoxLayout(logoContainer);
  logoLayout->setContentsMargins(20, 0, 20, 20);
  logoLayout->setSpacing(10);

  QLabel *logoIcon = new QLabel();
  logoIcon->setStyleSheet("background: transparent;");
  QPixmap logoPix(":/logo.png");
  if (logoPix.isNull())
    logoPix.load("logo.png");
  logoIcon->setPixmap(
      logoPix.scaled(65, 65, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  logoLayout->addWidget(logoIcon);

  QLabel *titleObj = new QLabel("TuniWaste");
  titleObj->setStyleSheet("color: white; font-size: 26px; font-weight: bold; "
                          "background: transparent;");
  logoLayout->addWidget(titleObj);
  logoLayout->addStretch();
  layout->addWidget(logoContainer);

  QString btnStyle =
      "QPushButton { text-align: left; padding: 12px 20px; color: white; "
      "background: transparent; border: none; font-size: 15px; font-weight: "
      "500; }"
      "QPushButton:hover { background-color: #81C784; }";

  auto createBtn = [&](QString text, auto slot) {
    QPushButton *btn = new QPushButton(text);
    btn->setStyleSheet(btnStyle);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, this, slot);
    layout->addWidget(btn);
  };

  createBtn("Tableau de bord", &MainWindow::onDashboardClicked);
  createBtn("Gestion des utilisateurs", &MainWindow::onUsersClicked);
  createBtn("Gestion des camions", &MainWindow::onTrucksClicked);
  createBtn("Gestion des zones", &MainWindow::onZonesClicked);
  createBtn("Gestion des poubelles", &MainWindow::onBinsClicked);
  createBtn("Gestion de recyclage", &MainWindow::onRecycleClicked);
  createBtn("Rapports", &MainWindow::onReportsClicked);
  createBtn("Paramètres", &MainWindow::onSettingsClicked);

  layout->addStretch();

  QLabel *userFooter = new QLabel("Ahmed");
  userFooter->setStyleSheet(
      "color: white; font-weight: bold; padding: 10px 20px; font-size: 14px;");
  layout->addWidget(userFooter);

  return sidebar;
}

void MainWindow::onReportsClicked() {
  QMessageBox::information(this, "Rapports",
                           "Module Rapports en cours de développement.");
}

void MainWindow::onSettingsClicked() {
  QMessageBox::information(this, "Paramètres",
                           "Module Paramètres en cours de développement.");
}

// =========================================================
// USER MANAGEMENT (NEW SPLIT LAYOUT)
// =========================================================

void MainWindow::setupUserManagementScreen() {
  QWidget *mainWidget = new QWidget();
  mainWidget->setStyleSheet("QWidget { background-color: #F8F9FA; }");

  QHBoxLayout *mainLayout = new QHBoxLayout(mainWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // 1. Left Panel (Form)
  createUserFormPanel();
  mainLayout->addWidget(userFormPanel);

  // 2. Right Panel (Table + Stats)
  createUserMainContent();
  mainLayout->addWidget(userMainContent);

  contentStackedWidget->addWidget(mainWidget);
}

void MainWindow::createUserFormPanel() {
  userFormPanel = new QWidget();
  userFormPanel->setFixedWidth(320);
  userFormPanel->setObjectName("formPanel");
  userFormPanel->setStyleSheet(
      "#formPanel { background: white; border-right: 1px solid #E5E5E5; }");

  QVBoxLayout *layout = new QVBoxLayout(userFormPanel);
  layout->setContentsMargins(25, 30, 25, 30);
  layout->setSpacing(20);

  userFormTitle = new QLabel("Nouv. Utilisateur");
  userFormTitle->setStyleSheet(
      "font-size: 20px; font-weight: bold; color: #2C3E50;");
  layout->addWidget(userFormTitle);

  auto createInput = [&](QString label, QString placeholder) -> QLineEdit * {
    QLabel *lbl = new QLabel(label);
    lbl->setStyleSheet("color: #7F8C8D; font-size: 12px; font-weight: 600;");
    layout->addWidget(lbl);
    QLineEdit *inp = new QLineEdit();
    inp->setPlaceholderText(placeholder);
    inp->setStyleSheet(
        "QLineEdit { padding: 10px; border: 1px solid #E0E0E0; border-radius: "
        "6px; background: #F8F9FA; } "
        "QLineEdit:focus { border: 1px solid #2ECC71; background: white; }");
    layout->addWidget(inp);
    return inp;
  };

  userNameInput = createInput("Nom complet", "Ex: Ahmed Ben Ali");
  userEmailInput = createInput("Email", "Ex: ahmed@tuniwaste.tn");
  userPhoneInput = createInput("Téléphone", "Ex: 55 123 456");

  layout->addWidget(new QLabel("Rôle", userFormPanel));
  userRoleCombo = new QComboBox();
  userRoleCombo->addItem("Employe");
  userRoleCombo->addItem("Administrateur");
  userRoleCombo->setStyleSheet(
      "QComboBox { padding: 10px; border: 1px solid #E0E0E0; border-radius: "
      "6px; background: #F8F9FA; }");
  layout->addWidget(userRoleCombo);

  userSaveButton = new QPushButton("Enregistrer");
  userSaveButton->setCursor(Qt::PointingHandCursor);
  userSaveButton->setStyleSheet(
      "QPushButton { background: #2ECC71; color: white; border-radius: 8px; "
      "padding: 12px; font-weight: bold; } "
      "QPushButton:hover { background: #27AE60; }");
  connect(userSaveButton, &QPushButton::clicked, this,
          &MainWindow::onUserSaveClicked);

  layout->addSpacing(10);
  layout->addWidget(userSaveButton);

  QPushButton *clearBtn = new QPushButton("Annuler");
  clearBtn->setCursor(Qt::PointingHandCursor);
  clearBtn->setStyleSheet(
      "QPushButton { background: transparent; color: #7F8C8D; border: none; } "
      "QPushButton:hover { color: #34495E; }");
  connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearUserForm);
  layout->addWidget(clearBtn);

  layout->addStretch();
}

void MainWindow::createUserMainContent() {
  userMainContent = new QWidget();
  QVBoxLayout *mainLayout = new QVBoxLayout(userMainContent);
  mainLayout->setContentsMargins(30, 30, 30, 30);
  mainLayout->setSpacing(25);

  // Header
  QHBoxLayout *headerLayout = new QHBoxLayout();
  QVBoxLayout *titleBox = new QVBoxLayout();
  QLabel *mainTitle = new QLabel("Gestion des Utilisateurs");
  mainTitle->setStyleSheet(
      "font-size: 24px; font-weight: bold; color: #2C3E50;");
  titleBox->addWidget(mainTitle);
  headerLayout->addLayout(titleBox);
  headerLayout->addStretch();

  createUserChartWidget();
  headerLayout->addWidget(userChartWidget);
  mainLayout->addLayout(headerLayout);

  // Filter Bar
  QHBoxLayout *actionsLayout = new QHBoxLayout();
  userSearchInput = new QLineEdit();
  userSearchInput->setPlaceholderText("🔍 Rechercher...");
  userSearchInput->setFixedWidth(300);
  userSearchInput->setStyleSheet(
      "QLineEdit { padding: 10px 15px; border: 1px solid #E0E0E0; "
      "border-radius: 20px; background: white; }");
  connect(userSearchInput, &QLineEdit::textChanged, this,
          &MainWindow::onUserSearchChanged);

  userRoleFilter = new QComboBox();
  userRoleFilter->addItem("Tous les rôles");
  userRoleFilter->addItem("Administrateur");
  userRoleFilter->addItem("Employe");
  userRoleFilter->setFixedWidth(150);
  userRoleFilter->setStyleSheet(
      "QComboBox { padding: 8px 15px; border: 1px solid #E0E0E0; "
      "border-radius: 20px; background: white; }");
  connect(userRoleFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int) { filterAndSortUsers(); });

  QPushButton *pdfBtn = new QPushButton("📊 PDF");
  pdfBtn->setStyleSheet("QPushButton { background: white; border: 1px solid "
                        "#E0E0E0; border-radius: 20px; padding: 8px 15px; } "
                        "QPushButton:hover { background: #F5F5F5; }");
  connect(pdfBtn, &QPushButton::clicked, this, &MainWindow::onExportUsersPDF);

  actionsLayout->addWidget(userSearchInput);
  actionsLayout->addWidget(userRoleFilter);
  actionsLayout->addWidget(pdfBtn);
  actionsLayout->addStretch();
  mainLayout->addLayout(actionsLayout);

  // Table
  userTable = new QTableWidget();
  userTable->setColumnCount(5);
  userTable->setHorizontalHeaderLabels(
      {"ID", "Nom & Prénom", "Email", "Rôle", "Actions"});
  userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  userTable->verticalHeader()->setVisible(false);
  userTable->setShowGrid(false);
  userTable->setFrameShape(QFrame::NoFrame);
  userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  userTable->setSelectionMode(QAbstractItemView::SingleSelection);
  userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  userTable->setStyleSheet(
      "QTableWidget { background: white; border-radius: 12px; padding: 10px; } "
      "QHeaderView::section { background: white; border: none; font-weight: "
      "bold; color: #7F8C8D; padding: 10px; } "
      "QTableWidget::item { border-bottom: 1px solid #F0F0F0; padding: 10px; "
      "}");

  mainLayout->addWidget(userTable);
  updateUserTable();
}

void MainWindow::updateUserTable() {
  userTable->setRowCount(0);
  userTable->setRowCount(filteredUsers.size());

  for (int i = 0; i < filteredUsers.size(); ++i) {
    const User &u = filteredUsers[i];

    userTable->setItem(i, 0, new QTableWidgetItem(QString::number(u.id)));
    userTable->setItem(i, 1, new QTableWidgetItem(u.name));
    userTable->setItem(i, 2, new QTableWidgetItem(u.email));

    QLabel *roleBadge = new QLabel(u.role);
    roleBadge->setAlignment(Qt::AlignCenter);
    QString style =
        (u.role == "Administrateur")
            ? "background: #E8F8F5; color: #27AE60; border: 1px solid #27AE60;"
            : "background: #F4F6F7; color: #7F8C8D; border: 1px solid #BDC3C7;";
    roleBadge->setStyleSheet(style + "border-radius: 10px; padding: 2px 8px; "
                                     "font-size: 11px; font-weight: bold;");

    QWidget *roleW = new QWidget();
    QHBoxLayout *rl = new QHBoxLayout(roleW);
    rl->setContentsMargins(10, 5, 10, 5);
    rl->addWidget(roleBadge);
    userTable->setCellWidget(i, 3, roleW);

    QWidget *actionsW = new QWidget();
    QHBoxLayout *al = new QHBoxLayout(actionsW);
    al->setContentsMargins(5, 5, 5, 5);

    QPushButton *editBtn = new QPushButton("✏️");
    editBtn->setFixedSize(30, 30);
    editBtn->setStyleSheet(
        "border: none; background: #F4F6F6; border-radius: 5px;");
    connect(editBtn, &QPushButton::clicked, this,
            [this, i]() { onModifyUser(i); });

    QPushButton *delBtn = new QPushButton("🗑️");
    delBtn->setFixedSize(30, 30);
    delBtn->setStyleSheet("border: none; background: #FDEDEC; border-radius: "
                          "5px; color: #E74C3C;");
    connect(delBtn, &QPushButton::clicked, this,
            [this, i]() { onDeleteUser(i); });

    al->addWidget(editBtn);
    al->addWidget(delBtn);
    userTable->setCellWidget(i, 4, actionsW);
  }
}

void MainWindow::createUserChartWidget() {
  userChartWidget = new QWidget();
  userChartWidget->setFixedHeight(120);

  QHBoxLayout *layout = new QHBoxLayout(userChartWidget);
  layout->setContentsMargins(0, 0, 0, 0);

  // 1. Stats Text Data
  QWidget *statsBox = new QWidget();
  statsBox->setStyleSheet(
      "background: white; border-radius: 12px; border: 1px solid #E0E0E0;");
  statsBox->setFixedWidth(120);
  QVBoxLayout *statsLayout = new QVBoxLayout(statsBox);

  QLabel *countLbl = new QLabel(QString::number(users.size()));
  countLbl->setObjectName("countLabel");
  countLbl->setStyleSheet(
      "font-size: 24px; font-weight: bold; color: #2E7D32;");
  countLbl->setAlignment(Qt::AlignCenter);

  QLabel *descLbl = new QLabel("Utilisateurs");
  descLbl->setStyleSheet("color: #7F8C8D; font-size: 12px; font-weight: 600;");
  descLbl->setAlignment(Qt::AlignCenter);

  statsLayout->addStretch();
  statsLayout->addWidget(countLbl);
  statsLayout->addWidget(descLbl);
  statsLayout->addStretch();

  layout->addWidget(statsBox);

  // 2. Chart (Pie)
  QPieSeries *series = new QPieSeries();
  int adminCount = 0;
  int empCount = 0;
  for (const auto &u : users) {
    if (u.role == "Administrateur")
      adminCount++;
    else
      empCount++;
  }

  series->append("Admin", adminCount);
  series->append("Employé", empCount);

  if (series->slices().size() > 0) {
    series->slices().at(0)->setBrush(QColor("#2ECC71"));
    series->slices().at(0)->setLabelVisible(false);
  }
  if (series->slices().size() > 1) {
    series->slices().at(1)->setBrush(QColor("#BDC3C7"));
    series->slices().at(1)->setLabelVisible(false);
  }

  QChart *chart = new QChart();
  chart->addSeries(series);
  chart->legend()->hide();
  chart->setBackgroundRoundness(0);
  chart->setMargins(QMargins(0, 0, 0, 0));
  chart->setBackgroundVisible(false);

  QChartView *chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setStyleSheet("background: transparent;");

  layout->addWidget(chartView);
}

void MainWindow::updateUserChart() {
  if (userChartWidget) {
    QLabel *lbl = userChartWidget->findChild<QLabel *>("countLabel");
    if (lbl)
      lbl->setText(QString::number(users.size()));
  }
}

void MainWindow::onUserSaveClicked() {
  if (userNameInput->text().isEmpty() || userEmailInput->text().isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez remplir les champs obligatoires.");
    return;
  }

  if (currentUserEditingRow >= 0) {
    // Edit Mode
    if (currentUserEditingRow < filteredUsers.size()) {
      int userId = filteredUsers[currentUserEditingRow].id;
      // Find original user info
      for (auto &u : users) {
        if (u.id == userId) {
          u.name = userNameInput->text();
          u.email = userEmailInput->text();
          u.phone = userPhoneInput->text();
          u.role = userRoleCombo->currentText();
          break;
        }
      }
      QMessageBox::information(this, "Succès", "Utilisateur modifié !");
    }
  } else {
    // Add Mode
    User newUser;
    newUser.id = nextUserId++;
    newUser.name = userNameInput->text();
    newUser.email = userEmailInput->text();
    newUser.phone = userPhoneInput->text();
    newUser.role = userRoleCombo->currentText();
    users.append(newUser);
    QMessageBox::information(this, "Succès", "Utilisateur ajouté !");
  }

  clearUserForm();
  filterAndSortUsers();
  updateUserChart();
}

void MainWindow::clearUserForm() {
  userNameInput->clear();
  userEmailInput->clear();
  userPhoneInput->clear();
  userRoleCombo->setCurrentIndex(0);
  userFormTitle->setText("Nouv. Utilisateur");
  userSaveButton->setText("Enregistrer");
  currentUserEditingRow = -1;
}

void MainWindow::onUserSearchChanged(const QString &text) {
  filterAndSortUsers();
}

void MainWindow::onSearchTextChanged(const QString &text) {
  onUserSearchChanged(text);
}

void MainWindow::onModifyUser(int row) {
  if (row < 0 || row >= filteredUsers.size())
    return;

  currentUserEditingRow = row;
  const User &u = filteredUsers[row];

  userNameInput->setText(u.name);
  userEmailInput->setText(u.email);
  userPhoneInput->setText(u.phone);
  userRoleCombo->setCurrentText(u.role);

  userFormTitle->setText("Modifier Utilisateur");
  userSaveButton->setText("Mettre à jour");
}

void MainWindow::onDeleteUser(int row) {
  if (row < 0 || row >= filteredUsers.size())
    return;

  if (QMessageBox::question(this, "Confirmer", "Supprimer cet utilisateur ?") ==
      QMessageBox::Yes) {
    int idToRemove = filteredUsers[row].id;
    for (int i = 0; i < users.size(); ++i) {
      if (users[i].id == idToRemove) {
        users.removeAt(i);
        break;
      }
    }
    filterAndSortUsers();
    updateUserChart();
    clearUserForm();
  }
}

// Map old slots to new logic where appropriate
void MainWindow::onAddUserClicked() { clearUserForm(); }

void MainWindow::onUsersClicked() { contentStackedWidget->setCurrentIndex(0); }

void MainWindow::filterAndSortUsers() {
  QString search = userSearchInput ? userSearchInput->text().toLower() : "";
  QString role =
      userRoleFilter ? userRoleFilter->currentText() : "Tous les rôles";

  filteredUsers.clear();
  for (const auto &u : users) {
    bool matchSearch =
        u.name.toLower().contains(search) || u.email.toLower().contains(search);
    bool matchRole = (role == "Tous les rôles") || (u.role == role);
    if (matchSearch && matchRole)
      filteredUsers.append(u);
  }
  updateUserTable();
}

// =========================================================
// OTHER SCREENS
// =========================================================

void MainWindow::setupRecyclingScreen() {
  recyclageWidget = new Recyclage(this);
  contentStackedWidget->addWidget(recyclageWidget);
}
void MainWindow::onRecycleClicked() {
  contentStackedWidget->setCurrentWidget(recyclageWidget);
}

void MainWindow::setupZonesScreen() {
  zonesWidget = new GestionZones(this);
  contentStackedWidget->addWidget(zonesWidget);
}
void MainWindow::onZonesClicked() {
  contentStackedWidget->setCurrentWidget(zonesWidget);
}

void MainWindow::setupTrucksScreen() {
  camionsWidget = new GestionCamions(this);
  contentStackedWidget->addWidget(camionsWidget);
}
void MainWindow::onTrucksClicked() {
  contentStackedWidget->setCurrentWidget(camionsWidget);
}

void MainWindow::setupBinsScreen() {
  poubellesWidget = new GestionPoubelles();
  contentStackedWidget->addWidget(poubellesWidget);
}
void MainWindow::onBinsClicked() {
  contentStackedWidget->setCurrentWidget(poubellesWidget);
}

void MainWindow::setupDashboardScreen() {
  dashboardWidget = new Dashboard();
  contentStackedWidget->addWidget(dashboardWidget);
}
void MainWindow::onDashboardClicked() {
  contentStackedWidget->setCurrentWidget(dashboardWidget);
}

// =========================================================
// PDF EXPORT
// =========================================================

void MainWindow::onExportUsersPDF() {
  QString fileName =
      QFileDialog::getSaveFileName(this, "Exporter PDF", "", "PDF (*.pdf)");
  if (fileName.isEmpty())
    return;
  if (QFileInfo(fileName).suffix().isEmpty())
    fileName.append(".pdf");

  QPdfWriter writer(fileName);
  writer.setPageSize(QPageSize(QPageSize::A4));
  QPainter painter(&writer);

  // Header
  painter.setPen(QColor("#2E7D32"));
  painter.setFont(QFont("Arial", 20, QFont::Bold));
  painter.drawText(300, 300, "Rapport des Utilisateurs - TuniWaste");

  painter.setPen(Qt::black);
  painter.setFont(QFont("Arial", 12));
  painter.drawText(300, 600, "Date: " + QDate::currentDate().toString());

  int y = 1200;
  // Table Header
  painter.setFont(QFont("Arial", 10, QFont::Bold));
  painter.drawText(300, y, "ID");
  painter.drawText(1000, y, "Nom");
  painter.drawText(3500, y, "Email");
  painter.drawText(6500, y, "Rôle");
  y += 300;
  painter.drawLine(300, y, 9000, y);
  y += 300;

  // Data
  painter.setFont(QFont("Arial", 10));
  for (const auto &u : filteredUsers) {
    painter.drawText(300, y, QString::number(u.id));
    painter.drawText(1000, y, u.name);
    painter.drawText(3500, y, u.email);
    painter.drawText(6500, y, u.role);
    y += 400;

    if (y > 13000) {
      writer.newPage();
      y = 500;
    }
  }

  painter.end();
  QMessageBox::information(this, "Succès", "PDF exporté !");
  QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

// Unused slots required by meta-object system if invoked
void MainWindow::onSortByColumn(int column) { filterAndSortUsers(); }
void MainWindow::addStatBar(QVBoxLayout *layout, const QString &label,
                            int count, int total, const QString &color) {}

// =========================================================
// DIALOG IMPLEMENTATIONS (Legacy Support)
// =========================================================

AddUserDialog::AddUserDialog(QWidget *parent, User *editUser)
    : QDialog(parent) {
  setWindowTitle(editUser ? "Modifier l'utilisateur"
                          : "Ajouter un utilisateur");
  QVBoxLayout *layout = new QVBoxLayout(this);
  QFormLayout *formLayout = new QFormLayout();

  nameEdit = new QLineEdit(this);
  emailEdit = new QLineEdit(this);
  phoneEdit = new QLineEdit(this);
  roleCombo = new QComboBox(this);
  roleCombo->addItems({"Employe", "Administrateur"});

  if (editUser) {
    nameEdit->setText(editUser->name);
    emailEdit->setText(editUser->email);
    phoneEdit->setText(editUser->phone);
    roleCombo->setCurrentText(editUser->role);
  }

  formLayout->addRow("Nom:", nameEdit);
  formLayout->addRow("Email:", emailEdit);
  formLayout->addRow("Téléphone:", phoneEdit);
  formLayout->addRow("Rôle:", roleCombo);

  layout->addLayout(formLayout);

  QHBoxLayout *btnLayout = new QHBoxLayout();
  QPushButton *saveBtn = new QPushButton("Enregistrer", this);
  QPushButton *cancelBtn = new QPushButton("Annuler", this);
  connect(saveBtn, &QPushButton::clicked, this, &QDialog::accept);
  connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
  btnLayout->addWidget(saveBtn);
  btnLayout->addWidget(cancelBtn);
  layout->addLayout(btnLayout);
}

PasswordResetDialog::PasswordResetDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle("Réinitialisation du mot de passe");
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(
      new QLabel("Entrez votre email pour réinitialiser le mot de passe:"));

  emailEdit = new QLineEdit(this);
  emailEdit->setPlaceholderText("email@example.com");
  layout->addWidget(emailEdit);

  QHBoxLayout *btnLayout = new QHBoxLayout();
  QPushButton *resetBtn = new QPushButton("Réinitialiser", this);
  QPushButton *cancelBtn = new QPushButton("Annuler", this);
  connect(resetBtn, &QPushButton::clicked, this,
          &PasswordResetDialog::onResetClicked);
  connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
  btnLayout->addWidget(resetBtn);
  btnLayout->addWidget(cancelBtn);
  layout->addLayout(btnLayout);
}
void PasswordResetDialog::onResetClicked() { accept(); }
