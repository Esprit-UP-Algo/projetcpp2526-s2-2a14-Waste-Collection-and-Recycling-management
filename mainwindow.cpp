#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QFormLayout>
#include <QScrollArea>
#include <QCoreApplication>
#include <algorithm>
#include <utility>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QDateTime>
#include <QStandardPaths>
#include <QGroupBox>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), nextUserId(5), currentSortColumn(-1),
    currentSortOrder(Qt::AscendingOrder), editingUserId(-1)
{
    // Initialize sample users with firstName and lastName
    users.append({1, "Ahmed", "Ben Ali", "ahmed.benali@example.com", "+216 98 123 456",
                  "Administrateur", "Homme", "Tunis", "1000", "", "admin123"});
    users.append({2, "Sami", "Trabelsi", "sami.trabelsi@example.com", "+216 22 456 789",
                  "Employe", "Homme", "Sfax", "3000", "", "sami123"});
    users.append({3, "Ali", "Haddad", "ali.haddad@example.com", "+216 55 789 123",
                  "Employe", "Homme", "Sousse", "4000", "", "ali123"});
    users.append({4, "Nadia", "Ayari", "nadia.ayari@example.com", "+216 20 321 654",
                  "Employe", "Femme", "Nabeul", "8000", "", "nadia123"});

    filteredUsers = users;

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setupLoginScreen();
    setupUserManagementScreen();

    stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupLoginScreen()
{
    // ======================================================
    // MAIN CONTAINER - Split layout: left panel + right form
    // ======================================================
    QWidget *loginWidget = new QWidget();
    loginWidget->setStyleSheet("QWidget { background-color: #F0F7EC; }");

    QHBoxLayout *mainLayout = new QHBoxLayout(loginWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ======================================================
    // LEFT PANEL - Dark green branding side
    // ======================================================
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(480);
    leftPanel->setStyleSheet(
        "QWidget { "
        "   background-color: #3B6B35; "
        "   border-right: 4px solid #6FA85E; "
        "}"
        );

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(50, 60, 50, 60);
    leftLayout->setSpacing(0);
    leftLayout->setAlignment(Qt::AlignVCenter);

    // Logo on left panel
    QLabel *leftLogoLabel = new QLabel();
    QPixmap logo("magee.png");
    if (logo.isNull()) { logo.load(":/magee.png"); }
    if (logo.isNull()) { logo.load(QCoreApplication::applicationDirPath() + "/magee.png"); }
    if (!logo.isNull()) {
        leftLogoLabel->setPixmap(logo.scaled(110, 110, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        leftLogoLabel->setStyleSheet("background: transparent;");
    } else {
        leftLogoLabel->setText("♻️");
        leftLogoLabel->setStyleSheet("font-size: 80px; background: transparent;");
    }
    leftLogoLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(leftLogoLabel);

    leftLayout->addSpacing(30);

    QLabel *brandName = new QLabel("TuniWaste");
    brandName->setStyleSheet(
        "font-size: 42px; "
        "font-weight: bold; "
        "color: #FFFFFF; "
        "background: transparent; "
        "letter-spacing: 2px;"
        );
    brandName->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(brandName);

    leftLayout->addSpacing(16);

    // Decorative separator line
    QFrame *separatorLine = new QFrame();
    separatorLine->setFixedHeight(3);
    separatorLine->setStyleSheet("background-color: #A3D977; border-radius: 2px;");
    leftLayout->addWidget(separatorLine);

    leftLayout->addSpacing(24);

    QLabel *tagline = new QLabel("Système de Gestion\ndes Déchets & Recyclage");
    tagline->setStyleSheet(
        "font-size: 18px; "
        "color: #C8E6C9; "
        "background: transparent; "
        "line-height: 1.6;"
        );
    tagline->setAlignment(Qt::AlignCenter);
    tagline->setWordWrap(true);
    leftLayout->addWidget(tagline);

    leftLayout->addSpacing(50);

    // Stats row
    QWidget *statsWidget = new QWidget();
    statsWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsWidget);
    statsLayout->setContentsMargins(0, 0, 0, 0);
    statsLayout->setSpacing(20);

    auto makeStatBox = [](const QString &num, const QString &label) -> QWidget* {
        QWidget *box = new QWidget();
        box->setStyleSheet(
            "QWidget { "
            "   background-color: rgba(163, 217, 119, 0.15); "
            "   border: 1px solid rgba(163, 217, 119, 0.4); "
            "   border-radius: 10px; "
            "   padding: 10px; "
            "}"
            );
        QVBoxLayout *bl = new QVBoxLayout(box);
        bl->setSpacing(4);
        bl->setContentsMargins(12, 10, 12, 10);

        QLabel *numLabel = new QLabel(num);
        numLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #A3D977; background: transparent;");
        numLabel->setAlignment(Qt::AlignCenter);

        QLabel *txtLabel = new QLabel(label);
        txtLabel->setStyleSheet("font-size: 11px; color: #C8E6C9; background: transparent;");
        txtLabel->setAlignment(Qt::AlignCenter);

        bl->addWidget(numLabel);
        bl->addWidget(txtLabel);
        return box;
    };

    statsLayout->addWidget(makeStatBox("500+", "Camions"));
    statsLayout->addWidget(makeStatBox("24/7", "Service"));
    statsLayout->addWidget(makeStatBox("100%", "Recyclage"));
    leftLayout->addWidget(statsWidget);

    leftLayout->addStretch();

    // Footer text
    QLabel *footerLabel = new QLabel("© 2025 TuniWaste — Tous droits réservés");
    footerLabel->setStyleSheet("font-size: 11px; color: rgba(200,230,200,0.6); background: transparent;");
    footerLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(footerLabel);

    mainLayout->addWidget(leftPanel);

    // ======================================================
    // RIGHT PANEL - Login form
    // ======================================================
    QWidget *rightPanel = new QWidget();
    rightPanel->setStyleSheet("QWidget { background-color: #F0F7EC; }");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setAlignment(Qt::AlignCenter);
    rightLayout->setContentsMargins(80, 60, 80, 60);

    // Form card
    QWidget *card = new QWidget();
    card->setFixedWidth(420);
    card->setObjectName("loginCard");
    card->setStyleSheet(
        "QWidget#loginCard { "
        "   background-color: #FFFFFF; "
        "   border-radius: 20px; "
        "   border: 1px solid #D4EDDA; "
        "}"
        );

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(0);
    cardLayout->setContentsMargins(0, 0, 0, 0);

    // Card top green accent bar
    QWidget *cardTopBar = new QWidget();
    cardTopBar->setFixedHeight(8);
    cardTopBar->setStyleSheet(
        "QWidget { "
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "       stop:0 #3B6B35, stop:0.5 #6FA85E, stop:1 #A3D977); "
        "   border-radius: 20px; "
        "}"
        );
    cardLayout->addWidget(cardTopBar);

    QWidget *cardInner = new QWidget();
    cardInner->setStyleSheet("QWidget { background: transparent; }");
    QVBoxLayout *innerLayout = new QVBoxLayout(cardInner);
    innerLayout->setSpacing(18);
    innerLayout->setContentsMargins(40, 35, 40, 40);

    // Welcome text
    QLabel *welcomeLabel = new QLabel("Bon retour 👋");
    welcomeLabel->setStyleSheet(
        "font-size: 26px; font-weight: bold; color: #2E5D28; background: transparent;"
        );
    welcomeLabel->setAlignment(Qt::AlignLeft);
    innerLayout->addWidget(welcomeLabel);

    QLabel *subLabel = new QLabel("Connectez-vous à votre espace de gestion");
    subLabel->setStyleSheet("font-size: 13px; color: #7A9E76; background: transparent;");
    innerLayout->addWidget(subLabel);

    innerLayout->addSpacing(8);

    // Field style
    QString fieldStyle =
        "QLineEdit { "
        "   padding: 14px 16px; "
        "   border: 2px solid #E0EDD8; "
        "   border-radius: 10px; "
        "   font-size: 14px; "
        "   background-color: #F8FCF6; "
        "   color: #2C3E25; "
        "}"
        "QLineEdit:focus { "
        "   border: 2px solid #6FA85E; "
        "   background-color: #FFFFFF; "
        "}";

    QString labelStyle =
        "font-size: 13px; font-weight: bold; color: #3B6B35; background: transparent;";

    // Email field
    QLabel *emailLabel = new QLabel("📧  Adresse Email");
    emailLabel->setStyleSheet(labelStyle);
    innerLayout->addWidget(emailLabel);

    QLineEdit *emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("nom@example.com");
    emailEdit->setFixedHeight(50);
    emailEdit->setStyleSheet(fieldStyle);
    innerLayout->addWidget(emailEdit);

    // Password field
    QLabel *passwordLabel = new QLabel("🔒  Mot de passe");
    passwordLabel->setStyleSheet(labelStyle);
    innerLayout->addWidget(passwordLabel);

    QLineEdit *passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("••••••••••••");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedHeight(50);
    passwordEdit->setStyleSheet(fieldStyle);
    innerLayout->addWidget(passwordEdit);

    innerLayout->addSpacing(6);

    // Login button
    QPushButton *loginButton = new QPushButton("  Se connecter  →");
    loginButton->setFixedHeight(52);
    loginButton->setCursor(Qt::PointingHandCursor);
    loginButton->setStyleSheet(
        "QPushButton { "
        "   background-color: #6FA85E; "
        "   color: #FFFFFF; "
        "   font-size: 16px; "
        "   font-weight: bold; "
        "   border: none; "
        "   border-radius: 10px; "
        "   letter-spacing: 1px; "
        "}"
        "QPushButton:hover { background-color: #5A9048; }"
        "QPushButton:pressed { background-color: #3B6B35; }"
        );
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    innerLayout->addWidget(loginButton);

    // Forgot password
    QPushButton *forgotButton = new QPushButton("Mot de passe oublié ?");
    forgotButton->setStyleSheet(
        "QPushButton { "
        "   color: #6FA85E; "
        "   font-size: 13px; "
        "   background: transparent; "
        "   border: none; "
        "   text-decoration: underline; "
        "}"
        "QPushButton:hover { color: #3B6B35; }"
        );
    forgotButton->setCursor(Qt::PointingHandCursor);
    connect(forgotButton, &QPushButton::clicked, this, &MainWindow::onForgotPasswordClicked);

    QHBoxLayout *forgotLayout = new QHBoxLayout();
    forgotLayout->addStretch();
    forgotLayout->addWidget(forgotButton);
    forgotLayout->addStretch();
    innerLayout->addLayout(forgotLayout);

    cardLayout->addWidget(cardInner);

    rightLayout->addWidget(card, 0, Qt::AlignCenter);
    mainLayout->addWidget(rightPanel);

    stackedWidget->addWidget(loginWidget);
}

QWidget* MainWindow::createSidebar()
{
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(250);  // Réduit de 343px à 250px
    sidebar->setStyleSheet("QWidget { background-color: #6FA85E; }");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    QWidget *logoContainer = new QWidget();
    logoContainer->setStyleSheet("QWidget { background-color: #6FA85E; }");
    QHBoxLayout *logoLayout = new QHBoxLayout(logoContainer);
    logoLayout->setContentsMargins(15, 15, 15, 15);  // Réduit les marges
    logoLayout->setSpacing(10);  // Réduit l'espacement
    logoLayout->setAlignment(Qt::AlignLeft);

    QLabel *logoLabel = new QLabel();
    QPixmap logo("magee.png");

    if (logo.isNull()) {
        logo.load(":/magee.png");
        if (logo.isNull()) {
            logo.load("./magee.png");
        }
        if (logo.isNull()) {
            logo.load(QCoreApplication::applicationDirPath() + "/magee.png");
        }
    }

    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(45, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation));  // Réduit de 60 à 45
        logoLabel->setStyleSheet("background: transparent;");
    } else {
        logoLabel->setText("🗑️");
        logoLabel->setStyleSheet("font-size: 35px; background: transparent;");  // Réduit de 40 à 35
    }
    logoLayout->addWidget(logoLabel);

    QLabel *appTitleLabel = new QLabel("TuniWaste");
    appTitleLabel->setStyleSheet("color: #FFFFFF; font-size: 20px; font-weight: bold; background: transparent;");  // Réduit de 26 à 20
    appTitleLabel->setAlignment(Qt::AlignVCenter);
    logoLayout->addWidget(appTitleLabel);

    logoLayout->addStretch();

    sidebarLayout->addWidget(logoContainer);

    QString buttonStyle =
        "QPushButton { "
        "   background-color: #6FA85E; "
        "   color: #FFFFFF; "
        "   text-align: left; "
        "   padding: 14px 15px; "  // Augmenté
        "   border: none; "
        "   font-size: 14px; "  // Augmenté
        "   margin-bottom: 4px; "  // Espacement entre boutons
        "}"
        "QPushButton:hover { background-color: #7DB86D; }";

    QPushButton *dashboardBtn = new QPushButton("🏠  Tableau de bord");
    dashboardBtn->setStyleSheet(buttonStyle);
    sidebarLayout->addWidget(dashboardBtn);

    QPushButton *usersBtn = new QPushButton("👥  Gestion utilisateurs");
    usersBtn->setStyleSheet(buttonStyle);
    sidebarLayout->addWidget(usersBtn);

    QPushButton *trucksBtn = new QPushButton("🚛  Gestion camions");
    trucksBtn->setStyleSheet(buttonStyle);
    sidebarLayout->addWidget(trucksBtn);

    QPushButton *binsBtn = new QPushButton("🗑️  Gestion poubelles");
    binsBtn->setStyleSheet(buttonStyle);
    sidebarLayout->addWidget(binsBtn);

    QPushButton *zonesBtn = new QPushButton("📍  Gestion zones");
    zonesBtn->setStyleSheet(buttonStyle);
    sidebarLayout->addWidget(zonesBtn);

    QPushButton *recycleBtn = new QPushButton("♻️  Recyclage");
    recycleBtn->setStyleSheet(buttonStyle);
    sidebarLayout->addWidget(recycleBtn);

    QPushButton *collectBtn = new QPushButton("📋  Suivi collectes");
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

void MainWindow::setupUserManagementScreen()
{
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
    titleLabel->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #6FA85E; background: transparent;"
        );
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();

    QLabel *breadcrumbLabel = new QLabel("[HOME] Tableau de bord");
    breadcrumbLabel->setStyleSheet(
        "font-size: 14px; color: #999999; background-color: #FFFFFF; "
        "padding: 8px 15px; border: 1px solid #DDDDDD; border-radius: 4px;"
        );
    titleLayout->addWidget(breadcrumbLabel);

    topLayout->addWidget(titleWidget);

    QLabel *pathLabel = new QLabel("Tableau de bord / Gestion des utilisateurs");
    pathLabel->setStyleSheet("font-size: 13px; color: #999999; background: transparent;");
    topLayout->addWidget(pathLabel);

    contentLayout->addWidget(topWidget);

    QLabel *pageTitle = new QLabel("Gestion des utilisateurs");
    pageTitle->setStyleSheet(
        "font-size: 28px; font-weight: bold; color: #000000; background: transparent;"
        );
    contentLayout->addWidget(pageTitle);

    // ===========================================
    // NEW LAYOUT: FORM ON LEFT, TABLE ON RIGHT
    // ===========================================

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setHandleWidth(10);
    splitter->setStyleSheet(
        "QSplitter::handle { "
        "   background-color: #DDDDDD; "
        "   margin: 2px; "
        "}"
        );

    // ===========================================
    // LEFT SIDE: FORM
    // ===========================================
    QGroupBox *formGroupBox = new QGroupBox("Formulaire Utilisateur");
    formGroupBox->setMinimumWidth(350);  // Réduit de 380
    formGroupBox->setMaximumWidth(450);  // Réduit de 480
    formGroupBox->setStyleSheet(
        "QGroupBox { "
        "   background-color: #FFFFFF; "
        "   border: 2px solid #6FA85E; "
        "   border-radius: 8px; "
        "   margin-top: 10px; "
        "   padding: 15px; "
        "   font-size: 16px; "
        "   font-weight: bold; "
        "   color: #6FA85E; "
        "}"
        "QGroupBox::title { "
        "   subcontrol-origin: margin; "
        "   subcontrol-position: top left; "
        "   padding: 5px 10px; "
        "   background-color: #FFFFFF; "
        "}"
        );

    QVBoxLayout *formGroupLayout = new QVBoxLayout(formGroupBox);
    formGroupLayout->setSpacing(10);  // Réduit de 15 à 10
    formGroupLayout->setContentsMargins(10, 10, 10, 10);  // Réduit les marges

    // Photo section at top
    QHBoxLayout *photoTopLayout = new QHBoxLayout();
    photoTopLayout->setAlignment(Qt::AlignCenter);

    formPhotoLabel = new QLabel();
    formPhotoLabel->setFixedSize(80, 80);  // Réduit de 100 à 80
    formPhotoLabel->setStyleSheet(
        "QLabel { "
        "   border: 2px solid #DDDDDD; "
        "   border-radius: 40px; "  // Réduit de 50 à 40
        "   background-color: #F5F5F5; "
        "   font-size: 35px; "  // Réduit de 45 à 35
        "}"
        );
    formPhotoLabel->setAlignment(Qt::AlignCenter);
    formPhotoLabel->setText("👤");
    photoTopLayout->addWidget(formPhotoLabel);

    formGroupLayout->addLayout(photoTopLayout);

    QPushButton *browsePhotoBtn = new QPushButton("📷 Photo");
    browsePhotoBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #6FA85E; "
        "   color: #FFFFFF; "
        "   padding: 6px 12px; "  // Réduit
        "   border: none; "
        "   border-radius: 5px; "
        "   font-weight: bold; "
        "   font-size: 12px; "
        "}"
        "QPushButton:hover { background-color: #5A8F47; }"
        );
    browsePhotoBtn->setCursor(Qt::PointingHandCursor);
    connect(browsePhotoBtn, &QPushButton::clicked, this, &MainWindow::onBrowsePhotoClicked);
    formGroupLayout->addWidget(browsePhotoBtn);

    formGroupLayout->addSpacing(5);  // Réduit de 10 à 5

    // Form fields - vertical layout
    QFormLayout *formFieldsLayout = new QFormLayout();
    formFieldsLayout->setSpacing(8);  // Réduit de 12 à 8
    formFieldsLayout->setLabelAlignment(Qt::AlignRight);

    QString labelStyle = "font-size: 12px; font-weight: bold; color: #000000; background: transparent;";
    QString lineEditStyle =
        "QLineEdit { "
        "   padding: 6px; "  // Réduit de 8px à 6px
        "   border: 2px solid #CCCCCC; "
        "   border-radius: 5px; "
        "   background-color: #FFFFFF; "
        "   color: #000000; "
        "   font-size: 12px; "  // Réduit de 13px à 12px
        "}"
        "QLineEdit:focus { border: 2px solid #6FA85E; }";
    QString comboStyle =
        "QComboBox { "
        "   padding: 6px; "  // Réduit de 8px à 6px
        "   border: 2px solid #CCCCCC; "
        "   border-radius: 5px; "
        "   background-color: #FFFFFF; "
        "   color: #000000; "
        "   font-size: 12px; "  // Réduit de 13px à 12px
        "   font-weight: normal; "
        "}"
        "QComboBox:focus { "
        "   border: 2px solid #6FA85E; "
        "}"
        "QComboBox::drop-down { "
        "   border: none; "
        "   width: 30px; "
        "}"
        "QComboBox::down-arrow { "
        "   image: none; "
        "   border-left: 5px solid transparent; "
        "   border-right: 5px solid transparent; "
        "   border-top: 5px solid #666666; "
        "   width: 0; "
        "   height: 0; "
        "}"
        "QComboBox QAbstractItemView { "
        "   background-color: #FFFFFF; "
        "   color: #000000; "
        "   selection-background-color: #6FA85E; "
        "   selection-color: #FFFFFF; "
        "   border: 1px solid #CCCCCC; "
        "   padding: 5px; "
        "}";

    // Prénom
    QLabel *firstNameLabel = new QLabel("Prénom:");
    firstNameLabel->setStyleSheet(labelStyle);
    formFirstNameEdit = new QLineEdit();
    formFirstNameEdit->setPlaceholderText("Prénom");
    formFirstNameEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(firstNameLabel, formFirstNameEdit);

    // Nom
    QLabel *lastNameLabel = new QLabel("Nom:");
    lastNameLabel->setStyleSheet(labelStyle);
    formLastNameEdit = new QLineEdit();
    formLastNameEdit->setPlaceholderText("Nom de famille");
    formLastNameEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(lastNameLabel, formLastNameEdit);

    // Email
    QLabel *emailLabel = new QLabel("Email:");
    emailLabel->setStyleSheet(labelStyle);
    formEmailEdit = new QLineEdit();
    formEmailEdit->setPlaceholderText("nom@example.com");
    formEmailEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(emailLabel, formEmailEdit);

    // Téléphone
    QLabel *phoneLabel = new QLabel("Téléphone:");
    phoneLabel->setStyleSheet(labelStyle);
    formPhoneEdit = new QLineEdit();
    formPhoneEdit->setPlaceholderText("+216 XX XXX XXX");
    formPhoneEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(phoneLabel, formPhoneEdit);

    // Sexe
    QLabel *genderLabel = new QLabel("Sexe:");
    genderLabel->setStyleSheet(labelStyle);
    formGenderCombo = new QComboBox();
    formGenderCombo->addItem("Homme");
    formGenderCombo->addItem("Femme");
    formGenderCombo->setStyleSheet(comboStyle);
    formFieldsLayout->addRow(genderLabel, formGenderCombo);

    // Ville
    QLabel *cityLabel = new QLabel("Ville:");
    cityLabel->setStyleSheet(labelStyle);
    formCityEdit = new QLineEdit();
    formCityEdit->setPlaceholderText("Ville");
    formCityEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(cityLabel, formCityEdit);

    // Code Postal
    QLabel *postalCodeLabel = new QLabel("Code Postal:");
    postalCodeLabel->setStyleSheet(labelStyle);
    formPostalCodeEdit = new QLineEdit();
    formPostalCodeEdit->setPlaceholderText("1000");
    formPostalCodeEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(postalCodeLabel, formPostalCodeEdit);

    // Mot de passe
    QLabel *passwordFormLabel = new QLabel("Mot de passe:");
    passwordFormLabel->setStyleSheet(labelStyle);
    formPasswordEdit = new QLineEdit();
    formPasswordEdit->setPlaceholderText("••••••••");
    formPasswordEdit->setEchoMode(QLineEdit::Password);
    formPasswordEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(passwordFormLabel, formPasswordEdit);

    // Rôle
    QLabel *roleLabel = new QLabel("Rôle:");
    roleLabel->setStyleSheet(labelStyle);
    formRoleCombo = new QComboBox();
    formRoleCombo->addItem("Administrateur");
    formRoleCombo->addItem("Employe");
    formRoleCombo->setStyleSheet(comboStyle);
    formFieldsLayout->addRow(roleLabel, formRoleCombo);

    formGroupLayout->addLayout(formFieldsLayout);

    formGroupLayout->addSpacing(10);

    // Buttons
    QHBoxLayout *formButtonsLayout = new QHBoxLayout();
    formButtonsLayout->setSpacing(10);

    QPushButton *clearBtn = new QPushButton("🔄 Nouveau");
    clearBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #999999; "
        "   color: #FFFFFF; "
        "   padding: 10px 20px; "
        "   border: none; "
        "   border-radius: 5px; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #777777; }"
        );
    clearBtn->setCursor(Qt::PointingHandCursor);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearFormClicked);
    formButtonsLayout->addWidget(clearBtn);

    QPushButton *saveBtn = new QPushButton("💾 Enregistrer");
    saveBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #A3D977; "
        "   color: #FFFFFF; "
        "   padding: 10px 20px; "
        "   border: none; "
        "   border-radius: 5px; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #8FC65E; }"
        );
    saveBtn->setCursor(Qt::PointingHandCursor);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveUserClicked);
    formButtonsLayout->addWidget(saveBtn);

    formGroupLayout->addLayout(formButtonsLayout);

    // ===========================================
    // RIGHT SIDE: TABLE WITH FILTERS
    // ===========================================
    QWidget *tableWidget = new QWidget();
    tableWidget->setStyleSheet("QWidget { background-color: transparent; }");
    QVBoxLayout *tableLayout = new QVBoxLayout(tableWidget);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(15);

    // Search and Filter Section
    QWidget *searchFilterWidget = new QWidget();
    searchFilterWidget->setStyleSheet(
        "QWidget { "
        "   background-color: #FFFFFF; "
        "   border: 1px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 15px; "
        "}"
        );
    QHBoxLayout *searchFilterLayout = new QHBoxLayout(searchFilterWidget);
    searchFilterLayout->setSpacing(15);

    // Search by Name
    QLineEdit *searchNameEdit = new QLineEdit();
    searchNameEdit->setPlaceholderText("🔍 Rechercher (nom, email, ville...)");
    searchNameEdit->setFixedWidth(250);
    searchNameEdit->setStyleSheet(
        "QLineEdit { "
        "   padding: 10px 15px; "
        "   border: 2px solid #DDDDDD; "
        "   border-radius: 6px; "
        "   background-color: #FFFFFF; "
        "   color: #000000; "
        "   font-size: 14px; "
        "}"
        "QLineEdit:focus { border: 2px solid #6FA85E; }"
        );
    searchFilterLayout->addWidget(searchNameEdit);

    searchFilterLayout->addStretch();

    // Sort by dropdown
    QLabel *sortLabel = new QLabel("Trier:");
    sortLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #000000; background: transparent;");
    searchFilterLayout->addWidget(sortLabel);

    QComboBox *sortCombo = new QComboBox();
    sortCombo->addItem("Plus récent (ID ↓)");
    sortCombo->addItem("Plus ancien (ID ↑)");
    sortCombo->addItem("Nom (A-Z)");
    sortCombo->addItem("Email (A-Z)");
    sortCombo->setFixedWidth(180);
    sortCombo->setStyleSheet(
        "QComboBox { "
        "   padding: 10px 15px; "
        "   border: 1px solid #DDDDDD; "
        "   border-radius: 4px; "
        "   background-color: #FFFFFF; "
        "   color: #000000; "
        "   font-size: 13px; "
        "   font-weight: bold; "
        "}"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QComboBox::down-arrow { "
        "   image: none; "
        "   border-left: 5px solid transparent; "
        "   border-right: 5px solid transparent; "
        "   border-top: 5px solid #666666; "
        "}"
        "QComboBox QAbstractItemView { "
        "   background-color: #FFFFFF; "
        "   color: #000000; "
        "   selection-background-color: #6FA85E; "
        "   selection-color: #FFFFFF; "
        "   border: 1px solid #DDDDDD; "
        "}"
        );
    connect(sortCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [this](int index) {
                switch(index) {
                case 0: // Plus récent (ID descendant)
                    currentSortColumn = 0;
                    currentSortOrder = Qt::DescendingOrder;
                    break;
                case 1: // Plus ancien (ID ascendant)
                    currentSortColumn = 0;
                    currentSortOrder = Qt::AscendingOrder;
                    break;
                case 2: // Nom A-Z
                    currentSortColumn = 2;
                    currentSortOrder = Qt::AscendingOrder;
                    break;
                case 3: // Email A-Z
                    currentSortColumn = 3;
                    currentSortOrder = Qt::AscendingOrder;
                    break;
                }
                filterAndSortUsers();
            });
    searchFilterLayout->addWidget(sortCombo);

    // Filter Dropdown
    roleFilter = new QComboBox();
    roleFilter->addItem("Tous les rôles");
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
        "   font-weight: normal; "
        "}"
        "QComboBox:focus { "
        "   border: 1px solid #629952; "
        "}"
        "QComboBox::drop-down { "
        "   border: none; "
        "   width: 30px; "
        "}"
        "QComboBox::down-arrow { "
        "   image: none; "
        "   border-left: 5px solid transparent; "
        "   border-right: 5px solid transparent; "
        "   border-top: 5px solid #666666; "
        "}"
        "QComboBox QAbstractItemView { "
        "   background-color: #FFFFFF; "
        "   color: #000000; "
        "   selection-background-color: #629952; "
        "   selection-color: #FFFFFF; "
        "   border: 1px solid #DDDDDD; "
        "}"
        );
    connect(roleFilter, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [this](int) { filterAndSortUsers(); });
    searchFilterLayout->addWidget(roleFilter);

    QPushButton *exportPdfBtn = new QPushButton("📄 PDF");
    exportPdfBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #FF8C42; "
        "   color: #FFFFFF; "
        "   padding: 10px 20px; "
        "   border: none; "
        "   border-radius: 4px; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #FF7A29; }"
        );
    exportPdfBtn->setCursor(Qt::PointingHandCursor);
    connect(exportPdfBtn, &QPushButton::clicked, this, &MainWindow::onExportPdfClicked);
    searchFilterLayout->addWidget(exportPdfBtn);

    tableLayout->addWidget(searchFilterWidget);

    // Hidden search edit for backend
    searchEdit = new QLineEdit();
    searchEdit->setVisible(false);
    connect(searchNameEdit, &QLineEdit::textChanged, [this](const QString &text) {
        searchEdit->setText(text);
        filterAndSortUsers();
    });

    // User Table
    userTable = new QTableWidget();
    userTable->setColumnCount(8);
    userTable->setHorizontalHeaderLabels({"ID", "Prénom", "Nom", "Email", "Sexe", "Ville", "Rôle", "Actions"});

    // Configure column widths for better visibility
    userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    userTable->horizontalHeader()->setStretchLastSection(false);

    // Set minimum widths for each column
    userTable->setColumnWidth(0, 40);   // ID
    userTable->setColumnWidth(1, 90);   // Prénom
    userTable->setColumnWidth(2, 90);   // Nom
    userTable->setColumnWidth(3, 160);  // Email
    userTable->setColumnWidth(4, 90);   // Sexe
    userTable->setColumnWidth(5, 90);   // Ville
    userTable->setColumnWidth(6, 165);  // Rôle
    userTable->setColumnWidth(7, 210);  // Actions

    // Allow resizing columns except Actions
    for (int i = 0; i < 7; i++) {
        userTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    }
    userTable->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    userTable->verticalHeader()->setVisible(false);
    userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTable->setSelectionMode(QAbstractItemView::SingleSelection);
    userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    userTable->setAlternatingRowColors(true);
    userTable->setSortingEnabled(true);
    userTable->setShowGrid(true);
    userTable->setMinimumWidth(850);  // Largeur minimale pour voir toutes les colonnes
    userTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    userTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    userTable->setStyleSheet(
        "QTableWidget { "
        "   background-color: #FFFFFF; "
        "   border: 1px solid #E0E0E0; "
        "   gridline-color: #E5E5E5; "
        "   color: #000000; "
        "   font-size: 14px; "
        "}"
        "QTableWidget::item { "
        "   padding: 10px 8px; "
        "   border-bottom: 1px solid #E5E5E5; "
        "}"
        "QTableWidget::item:selected { background-color: #F0F7ED; color: #000000; }"
        "QHeaderView::section { "
        "   background-color: #F8F8F8; "
        "   color: #000000; "
        "   padding: 12px 8px; "
        "   border: none; "
        "   border-bottom: 2px solid #E0E0E0; "
        "   border-right: 1px solid #E5E5E5; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}"
        "QTableWidget::item:alternate { background-color: #FAFAFA; }"
        );

    connect(userTable->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::onSortByColumn);

    tableLayout->addWidget(userTable);

    // Add to splitter
    splitter->addWidget(formGroupBox);
    splitter->addWidget(tableWidget);
    splitter->setStretchFactor(0, 0);  // Form doesn't stretch
    splitter->setStretchFactor(1, 1);  // Table stretches

    contentLayout->addWidget(splitter);

    mainLayout->addWidget(contentArea);

    updateUserTable();

    stackedWidget->addWidget(mainWidget);
}

void MainWindow::updateUserTable()
{
    userTable->setRowCount(0);
    userTable->setSortingEnabled(false);
    userTable->setRowCount(filteredUsers.size());

    QFont cellFont;
    cellFont.setPointSize(11);
    cellFont.setFamily("Segoe UI");
    cellFont.setBold(true);

    for (int i = 0; i < filteredUsers.size(); ++i) {
        const User &user = filteredUsers[i];

        // ID
        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(user.id));
        idItem->setTextAlignment(Qt::AlignCenter);
        idItem->setFont(cellFont);
        userTable->setItem(i, 0, idItem);

        // Prénom
        QTableWidgetItem *firstNameItem = new QTableWidgetItem(user.firstName);
        firstNameItem->setFont(cellFont);
        firstNameItem->setTextAlignment(Qt::AlignCenter);
        userTable->setItem(i, 1, firstNameItem);

        // Nom
        QTableWidgetItem *lastNameItem = new QTableWidgetItem(user.lastName);
        lastNameItem->setFont(cellFont);
        lastNameItem->setTextAlignment(Qt::AlignCenter);
        userTable->setItem(i, 2, lastNameItem);

        // Email
        QTableWidgetItem *emailItem = new QTableWidgetItem(user.email);
        emailItem->setFont(cellFont);
        emailItem->setTextAlignment(Qt::AlignCenter);
        userTable->setItem(i, 3, emailItem);

        // Sexe
        QTableWidgetItem *genderItem = new QTableWidgetItem(user.gender);
        genderItem->setTextAlignment(Qt::AlignCenter);
        genderItem->setFont(cellFont);
        userTable->setItem(i, 4, genderItem);

        // Ville
        QTableWidgetItem *cityItem = new QTableWidgetItem(user.city);
        cityItem->setFont(cellFont);
        cityItem->setTextAlignment(Qt::AlignCenter);
        userTable->setItem(i, 5, cityItem);

        // Role with colored badge
        QWidget *roleWidget = new QWidget();
        roleWidget->setStyleSheet("background-color: transparent;");  // Fond transparent
        QHBoxLayout *roleLayout = new QHBoxLayout(roleWidget);
        roleLayout->setContentsMargins(5, 5, 5, 5);
        roleLayout->setAlignment(Qt::AlignCenter);

        QLabel *roleLabel = new QLabel(user.role);
        roleLabel->setAlignment(Qt::AlignCenter);
        if (user.role == "Administrateur") {
            roleLabel->setStyleSheet(
                "background-color: #FFA726; "
                "color: #FFFFFF; "
                "padding: 6px 6px; "
                "border-radius: 4px; "
                "font-weight: bold; "
                "font-size: 10px; "
                "min-width: 140px; "
                "max-width: 140px; "
                );
            roleLabel->setAlignment(Qt::AlignCenter);
        } else {
            roleLabel->setStyleSheet(
                "background-color: #5DADE2; "
                "color: #FFFFFF; "
                "padding: 6px 6px; "
                "border-radius: 4px; "
                "font-weight: bold; "
                "font-size: 10px; "
                "min-width: 90px; "
                "max-width: 90px; "
                );
            roleLabel->setAlignment(Qt::AlignCenter);
        }
        roleLayout->addWidget(roleLabel);

        // Create empty item for proper sorting (no text shown)
        QTableWidgetItem *roleSortItem = new QTableWidgetItem(user.role);
        roleSortItem->setForeground(QBrush(Qt::transparent));  // Texte invisible pour le tri seulement
        userTable->setItem(i, 6, roleSortItem);
        userTable->setCellWidget(i, 6, roleWidget);

        // Actions with styled buttons
        QWidget *actionWidget = new QWidget();
        QVBoxLayout *actionLayout = new QVBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 8, 5, 8);
        actionLayout->setSpacing(12);
        actionLayout->setAlignment(Qt::AlignCenter);

        QPushButton *editBtn = new QPushButton("Modifier");
        editBtn->setCursor(Qt::PointingHandCursor);
        editBtn->setFixedSize(165, 30);
        editBtn->setStyleSheet(
            "QPushButton { "
            "   background-color: #4CAF50; "
            "   color: #FFFFFF; "
            "   padding: 6px 12px; "
            "   border: none; "
            "   border-radius: 5px; "
            "   font-size: 12px; "
            "   font-weight: bold; "
            "}"
            "QPushButton:hover { background-color: #45A049; }"
            );
        connect(editBtn, &QPushButton::clicked, [this, i]() { onModifyUser(i); });

        QPushButton *deleteBtn = new QPushButton("Supprimer");
        deleteBtn->setCursor(Qt::PointingHandCursor);
        deleteBtn->setFixedSize(165, 30);
        deleteBtn->setStyleSheet(
            "QPushButton { "
            "   background-color: #F44336; "
            "   color: #FFFFFF; "
            "   padding: 6px 12px; "
            "   border: none; "
            "   border-radius: 5px; "
            "   font-size: 12px; "
            "   font-weight: bold; "
            "}"
            "QPushButton:hover { background-color: #DA190B; }"
            );
        connect(deleteBtn, &QPushButton::clicked, [this, i]() { onDeleteUser(i); });

        actionLayout->addWidget(editBtn);
        actionLayout->addWidget(deleteBtn);

        userTable->setCellWidget(i, 7, actionWidget);
    }

    // Set row heights
    for (int i = 0; i < userTable->rowCount(); ++i) {
        userTable->setRowHeight(i, 85);
    }

    userTable->setSortingEnabled(true);
}

void MainWindow::clearForm()
{
    formFirstNameEdit->clear();
    formLastNameEdit->clear();
    formEmailEdit->clear();
    formPhoneEdit->clear();
    formGenderCombo->setCurrentIndex(0);
    formCityEdit->clear();
    formPostalCodeEdit->clear();
    formRoleCombo->setCurrentIndex(0);
    formPhotoPath.clear();
    formPasswordEdit->clear();
    formPhotoLabel->setText("👤");
    formPhotoLabel->setStyleSheet(
        "QLabel { "
        "   border: 2px solid #DDDDDD; "
        "   border-radius: 40px; "
        "   background-color: #F5F5F5; "
        "   font-size: 35px; "
        "}"
        );
    editingUserId = -1;
}

void MainWindow::loadUserToForm(const User &user)
{
    formFirstNameEdit->setText(user.firstName);
    formLastNameEdit->setText(user.lastName);
    formEmailEdit->setText(user.email);
    formPhoneEdit->setText(user.phone);
    formGenderCombo->setCurrentText(user.gender);
    formCityEdit->setText(user.city);
    formPostalCodeEdit->setText(user.postalCode);
    formRoleCombo->setCurrentText(user.role);
    formPhotoPath = user.photoPath;
    formPasswordEdit->setText(user.password);

    if (!user.photoPath.isEmpty() && QFile::exists(user.photoPath)) {
        QPixmap photo(user.photoPath);
        formPhotoLabel->setPixmap(photo.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        formPhotoLabel->setStyleSheet(
            "QLabel { "
            "   border: 2px solid #DDDDDD; "
            "   border-radius: 40px; "
            "}"
            );
    } else {
        formPhotoLabel->setText("👤");
        formPhotoLabel->setStyleSheet(
            "QLabel { "
            "   border: 2px solid #DDDDDD; "
            "   border-radius: 40px; "
            "   background-color: #F5F5F5; "
            "   font-size: 35px; "
            "}"
            );
    }

    editingUserId = user.id;
}

void MainWindow::onSearchTextChanged(const QString &/*text*/)
{
    filterAndSortUsers();
}

void MainWindow::onSortByColumn(int column)
{
    if (currentSortColumn == column) {
        currentSortOrder = (currentSortOrder == Qt::AscendingOrder) ?
                               Qt::DescendingOrder : Qt::AscendingOrder;
    } else {
        currentSortColumn = column;
        currentSortOrder = Qt::AscendingOrder;
    }
    filterAndSortUsers();
}

void MainWindow::filterAndSortUsers()
{
    QString searchText = searchEdit->text().trimmed().toLower();
    QString selectedRole = roleFilter->currentText();

    filteredUsers.clear();

    for (const User &user : std::as_const(users)) {
        bool matchesSearch = searchText.isEmpty() ||
                             QString::number(user.id).contains(searchText) ||
                             user.firstName.toLower().contains(searchText) ||
                             user.lastName.toLower().contains(searchText) ||
                             user.email.toLower().contains(searchText) ||
                             user.phone.contains(searchText) ||
                             user.gender.toLower().contains(searchText) ||
                             user.city.toLower().contains(searchText) ||
                             user.postalCode.contains(searchText) ||
                             user.role.toLower().contains(searchText);

        bool matchesRole = (selectedRole == "Tous les rôles") || (user.role == selectedRole);

        if (matchesSearch && matchesRole) {
            filteredUsers.append(user);
        }
    }

    // Sort filtered users
    if (currentSortColumn >= 0 && currentSortColumn < 7) {
        std::sort(filteredUsers.begin(), filteredUsers.end(),
                  [this](const User &a, const User &b) {
                      QString valA, valB;
                      switch (currentSortColumn) {
                      case 0:
                          return (currentSortOrder == Qt::AscendingOrder) ? (a.id < b.id) : (a.id > b.id);
                      case 1: valA = a.firstName.toLower(); valB = b.firstName.toLower(); break;
                      case 2: valA = a.lastName.toLower(); valB = b.lastName.toLower(); break;
                      case 3: valA = a.email.toLower(); valB = b.email.toLower(); break;
                      case 4: valA = a.gender; valB = b.gender; break;
                      case 5: valA = a.city.toLower(); valB = b.city.toLower(); break;
                      case 6: valA = a.role; valB = b.role; break;
                      }
                      if (currentSortColumn != 0) {
                          return (currentSortOrder == Qt::AscendingOrder) ? (valA < valB) : (valA > valB);
                      }
                      return false;
                  });
    }

    updateUserTable();
}

void MainWindow::onLoginClicked()
{
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::onSaveUserClicked()
{
    // Validate inputs
    QString firstName = formFirstNameEdit->text().trimmed();
    QString lastName = formLastNameEdit->text().trimmed();
    QString email = formEmailEdit->text().trimmed();
    QString phone = formPhoneEdit->text().trimmed();
    QString gender = formGenderCombo->currentText();
    QString city = formCityEdit->text().trimmed();
    QString postalCode = formPostalCodeEdit->text().trimmed();
    QString role = formRoleCombo->currentText();
    QString password = formPasswordEdit->text().trimmed();

    if (firstName.isEmpty() || lastName.isEmpty() || email.isEmpty() || phone.isEmpty() || city.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs obligatoires !");
        return;
    }

    if (!email.contains("@") || !email.contains(".")) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une adresse email valide !");
        return;
    }

    if (editingUserId == -1) {
        // Adding new user
        User newUser;
        newUser.id = nextUserId++;
        newUser.firstName = firstName;
        newUser.lastName = lastName;
        newUser.email = email;
        newUser.phone = phone;
        newUser.role = role;
        newUser.gender = gender;
        newUser.city = city;
        newUser.postalCode = postalCode;
        newUser.photoPath = formPhotoPath;
        newUser.password = password;

        users.append(newUser);
        filterAndSortUsers();

        QMessageBox::information(this, "Succès", "Utilisateur ajouté avec succès !");
        clearForm();
    } else {
        // Editing existing user
        int actualIndex = -1;
        for (int i = 0; i < users.size(); ++i) {
            if (users[i].id == editingUserId) {
                actualIndex = i;
                break;
            }
        }

        if (actualIndex != -1) {
            users[actualIndex].firstName = firstName;
            users[actualIndex].lastName = lastName;
            users[actualIndex].email = email;
            users[actualIndex].phone = phone;
            users[actualIndex].role = role;
            users[actualIndex].gender = gender;
            users[actualIndex].city = city;
            users[actualIndex].postalCode = postalCode;
            users[actualIndex].photoPath = formPhotoPath;
            users[actualIndex].password = password;

            filterAndSortUsers();

            QMessageBox::information(this, "Succès", "Utilisateur modifié avec succès !");
            clearForm();
        }
    }
}

void MainWindow::onModifyUser(int row)
{
    if (row < 0 || row >= filteredUsers.size()) return;

    User &user = filteredUsers[row];

    // Find the actual user in the main users list
    int actualIndex = -1;
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].id == user.id) {
            actualIndex = i;
            break;
        }
    }

    if (actualIndex == -1) return;

    // Load user data into the form
    loadUserToForm(users[actualIndex]);
}

void MainWindow::onDeleteUser(int row)
{
    if (row < 0 || row >= filteredUsers.size()) return;

    User &user = filteredUsers[row];

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation",
                                  "Êtes-vous sûr de vouloir supprimer l'utilisateur '" +
                                      user.firstName + " " + user.lastName + "' ?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        for (int i = 0; i < users.size(); ++i) {
            if (users[i].id == user.id) {
                users.removeAt(i);
                break;
            }
        }

        filterAndSortUsers();

        QMessageBox::information(this, "Succès", "Utilisateur supprimé avec succès !");

        // Clear form if we were editing this user
        if (editingUserId == user.id) {
            clearForm();
        }
    }
}

void MainWindow::onBrowsePhotoClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Choisir une photo de profil",
                                                    QDir::homePath(),
                                                    "Images (*.png *.jpg *.jpeg *.bmp)");

    if (!fileName.isEmpty()) {
        formPhotoPath = fileName;
        QPixmap photo(fileName);
        formPhotoLabel->setPixmap(photo.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        formPhotoLabel->setStyleSheet(
            "QLabel { "
            "   border: 2px solid #DDDDDD; "
            "   border-radius: 40px; "
            "}"
            );
    }
}

void MainWindow::onClearFormClicked()
{
    clearForm();
}

void MainWindow::onForgotPasswordClicked()
{
    PasswordResetDialog dialog(this);
    dialog.exec();
}

// PasswordResetDialog implementation
PasswordResetDialog::PasswordResetDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Réinitialisation du mot de passe");
    setModal(true);
    setFixedSize(500, 280);

    setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel { color: #000000; background: transparent; }"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Réinitialiser le mot de passe");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #000000; background: transparent;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *instructionLabel = new QLabel(
        "Entrez votre adresse email et nous vous enverrons un lien\n"
        "pour réinitialiser votre mot de passe.");
    instructionLabel->setStyleSheet("font-size: 14px; color: #666666; background: transparent;");
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setWordWrap(true);
    mainLayout->addWidget(instructionLabel);

    mainLayout->addSpacing(10);

    QLabel *emailLabel = new QLabel("Adresse email :");
    emailLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #000000; background: transparent;");
    mainLayout->addWidget(emailLabel);

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("nom@example.com");
    emailEdit->setStyleSheet(
        "QLineEdit { "
        "   padding: 12px; "
        "   border: 2px solid #CCCCCC; "
        "   border-radius: 6px; "
        "   background-color: #FFFFFF; "
        "   color: #000000; "
        "   font-size: 14px; "
        "}"
        "QLineEdit:focus { border: 2px solid #A3C651; }"
        );
    mainLayout->addWidget(emailEdit);

    mainLayout->addSpacing(10);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    QPushButton *cancelBtn = new QPushButton("Annuler");
    cancelBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #F5F5F5; "
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
        "}"
        );
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton *resetBtn = new QPushButton("Envoyer");
    resetBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #A3C651; "
        "   color: #FFFFFF; "
        "   padding: 12px 24px; "
        "   border: none; "
        "   border-radius: 6px; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #8FB544; }"
        );
    connect(resetBtn, &QPushButton::clicked, this, &PasswordResetDialog::onResetClicked);

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(resetBtn);

    mainLayout->addLayout(buttonLayout);
}

void PasswordResetDialog::onResetClicked()
{
    QString email = emailEdit->text().trimmed();

    if (email.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer votre adresse email !");
        return;
    }

    if (!email.contains("@") || !email.contains(".")) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une adresse email valide !");
        return;
    }

    QMessageBox::information(this, "Succès",
                             "Un email de réinitialisation a été envoyé à " + email +
                                 "\n\nVeuillez vérifier votre boîte de réception.");

    accept();
}

void MainWindow::onExportPdfClicked()
{
    QString defaultFileName = "Liste_Utilisateurs_" +
                              QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".pdf";

    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Exporter en PDF",
                                                    documentsPath + "/" + defaultFileName,
                                                    "Fichiers PDF (*.pdf)");

    if (fileName.isEmpty()) {
        return;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize::A4);
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    QPainter painter;
    if (!painter.begin(&printer)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier PDF.");
        return;
    }

    QFont titleFont("Arial", 20, QFont::Bold);
    QFont headerFont("Arial", 12, QFont::Bold);
    QFont normalFont("Arial", 10);
    QFont smallFont("Arial", 8);

    int pageWidth = printer.pageRect(QPrinter::DevicePixel).width();
    int pageHeight = printer.pageRect(QPrinter::DevicePixel).height();
    int margin = 50;
    int yPos = margin;

    painter.setPen(QColor(111, 168, 94));
    painter.setFont(titleFont);
    QString title = "TuniWaste - Liste des Utilisateurs";
    painter.drawText(margin, yPos, title);
    yPos += 60;

    painter.setFont(smallFont);
    painter.setPen(Qt::black);
    QString dateTime = "Généré le : " + QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm:ss");
    painter.drawText(margin, yPos, dateTime);
    yPos += 40;

    painter.setPen(QPen(QColor(224, 224, 224), 2));
    painter.drawLine(margin, yPos, pageWidth - margin, yPos);
    yPos += 30;

    painter.setFont(normalFont);
    painter.setPen(Qt::black);
    QString stats = QString("Nombre total d'utilisateurs : %1  |  Administrateurs : %2  |  Employés : %3")
                        .arg(filteredUsers.size())
                        .arg(std::count_if(filteredUsers.begin(), filteredUsers.end(),
                                           [](const User& u) { return u.role == "Administrateur"; }))
                        .arg(std::count_if(filteredUsers.begin(), filteredUsers.end(),
                                           [](const User& u) { return u.role == "Employe"; }))  ;
    painter.drawText(margin, yPos, stats);
    yPos += 50;

    int colWidths[] = {50, 130, 130, 210, 80, 110, 100};
    int totalWidth = 0;
    for (int w : colWidths) totalWidth += w;

    double scaleFactor = (double)(pageWidth - 2 * margin) / totalWidth;
    for (int i = 0; i < 7; i++) {
        colWidths[i] = (int)(colWidths[i] * scaleFactor);
    }

    painter.setFont(headerFont);
    painter.setPen(Qt::white);
    painter.setBrush(QColor(111, 168, 94));

    int headerHeight = 40;
    painter.drawRect(margin, yPos, pageWidth - 2 * margin, headerHeight);

    int xPos = margin;
    QStringList headers = {"ID", "Prénom", "Nom", "Email", "Sexe", "Ville", "Rôle"};

    for (int i = 0; i < headers.size(); i++) {
        QRect headerRect(xPos + 10, yPos, colWidths[i] - 10, headerHeight);
        painter.drawText(headerRect, Qt::AlignVCenter | Qt::AlignLeft, headers[i]);
        xPos += colWidths[i];
    }

    yPos += headerHeight;

    painter.setFont(normalFont);
    int rowHeight = 35;
    bool alternate = false;

    for (const User& user : std::as_const(filteredUsers)) {
        if (yPos + rowHeight > pageHeight - margin) {
            printer.newPage();
            yPos = margin;

            painter.setPen(Qt::white);
            painter.setBrush(QColor(111, 168, 94));
            painter.setFont(headerFont);
            painter.drawRect(margin, yPos, pageWidth - 2 * margin, headerHeight);

            xPos = margin;
            for (int i = 0; i < headers.size(); i++) {
                QRect headerRect(xPos + 10, yPos, colWidths[i] - 10, headerHeight);
                painter.drawText(headerRect, Qt::AlignVCenter | Qt::AlignLeft, headers[i]);
                xPos += colWidths[i];
            }
            yPos += headerHeight;
            painter.setFont(normalFont);
            alternate = false;
        }

        if (alternate) {
            painter.setBrush(QColor(249, 249, 249));
        } else {
            painter.setBrush(Qt::white);
        }
        painter.setPen(QColor(224, 224, 224));
        painter.drawRect(margin, yPos, pageWidth - 2 * margin, rowHeight);

        painter.setPen(Qt::black);
        xPos = margin;

        QStringList rowData = {
            QString::number(user.id),
            user.firstName,
            user.lastName,
            user.email,
            user.gender,
            user.city,
            user.role
        };

        for (int i = 0; i < rowData.size(); i++) {
            QRect cellRect(xPos + 10, yPos, colWidths[i] - 15, rowHeight);
            QString text = painter.fontMetrics().elidedText(rowData[i], Qt::ElideRight, colWidths[i] - 20);
            painter.drawText(cellRect, Qt::AlignVCenter | Qt::AlignLeft, text);
            xPos += colWidths[i];
        }

        yPos += rowHeight;
        alternate = !alternate;
    }

    yPos = pageHeight - margin + 20;
    painter.setFont(smallFont);
    painter.setPen(QColor(153, 153, 153));
    QString footer = "TuniWaste © " + QString::number(QDateTime::currentDateTime().date().year()) +
                     " - Système de Gestion des Déchets";
    painter.drawText(margin, yPos, pageWidth - 2 * margin, 20, Qt::AlignCenter, footer);

    painter.end();

    QMessageBox::information(this, "Succès",
                             "Le fichier PDF a été exporté avec succès !\n\nEmplacement : " + fileName);
}
