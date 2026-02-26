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
#include <QSqlQuery>
#include <QRegularExpression>

// ══════════════════════════════════════════════════════════════════════════════
//  Constructeur — charge les données depuis la BD dès l'ouverture
// ══════════════════════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    currentSortColumn(-1),
    currentSortOrder(Qt::AscendingOrder),
    editingUserId(-1),
    loginEmailEdit(nullptr),
    loginPasswordEdit(nullptr)
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setupLoginScreen();
    setupUserManagementScreen();

    stackedWidget->setCurrentIndex(0);

    // Charger les données depuis la BD
    refreshTable();
}

MainWindow::~MainWindow() {}

// ══════════════════════════════════════════════════════════════════════════════
//  refreshTable() — recharge le cache depuis Oracle et réaffiche le tableau
//  Appelée après chaque CREATE / UPDATE / DELETE (actualisation)
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::refreshTable()
{
    users.clear();

    QSqlDatabase db = Database::getInstance().getDatabase();
    QSqlQuery query(db);
    query.exec(
        "SELECT ID_UTILISATEUR, NOM, PRENOM, EMAIL, NUM_TELEPHONE, ROLE, VILLE, CODE_POSTAL, SEXE, PHOTO, MOT_DE_PASSE "
        "FROM UTILISATEUR ORDER BY ID_UTILISATEUR"
        );

    while (query.next()) {
        UserRow u;
        u.id          = query.value(0).toInt();
        u.nom         = query.value(1).toString();
        u.prenom      = query.value(2).toString();
        u.email       = query.value(3).toString();
        u.telephone   = query.value(4).toString();
        u.role        = query.value(5).toString();
        u.ville       = query.value(6).toString();
        u.codePostal  = query.value(7).toString();
        u.sexe        = query.value(8).toString();
        u.photo       = query.value(9).toString();
        u.motDePasse  = query.value(10).toString();
        users.append(u);
    }

    filterAndSortUsers();
}

// ══════════════════════════════════════════════════════════════════════════════
//  setupLoginScreen()
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::setupLoginScreen()
{
    QWidget *loginWidget = new QWidget();
    loginWidget->setStyleSheet("QWidget { background-color: #F0F7EC; }");

    QHBoxLayout *mainLayout = new QHBoxLayout(loginWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ── Panneau gauche ────────────────────────────────────────────────────────
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(480);
    leftPanel->setStyleSheet(
        "QWidget { background-color: #3B6B35; border-right: 4px solid #6FA85E; }"
        );

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(50, 60, 50, 60);
    leftLayout->setSpacing(0);
    leftLayout->setAlignment(Qt::AlignVCenter);

    QLabel *leftLogoLabel = new QLabel();
    QPixmap logo("magee.png");
    if (logo.isNull()) logo.load(":/magee.png");
    if (logo.isNull()) logo.load(QCoreApplication::applicationDirPath() + "/magee.png");
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
        "font-size: 42px; font-weight: bold; color: #FFFFFF; background: transparent; letter-spacing: 2px;"
        );
    brandName->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(brandName);
    leftLayout->addSpacing(16);

    QFrame *separatorLine = new QFrame();
    separatorLine->setFixedHeight(3);
    separatorLine->setStyleSheet("background-color: #A3D977; border-radius: 2px;");
    leftLayout->addWidget(separatorLine);
    leftLayout->addSpacing(24);

    QLabel *tagline = new QLabel("Système de Gestion\ndes Déchets & Recyclage");
    tagline->setStyleSheet(
        "font-size: 18px; color: #C8E6C9; background: transparent; line-height: 1.6;"
        );
    tagline->setAlignment(Qt::AlignCenter);
    tagline->setWordWrap(true);
    leftLayout->addWidget(tagline);
    leftLayout->addSpacing(50);

    QWidget *statsWidget = new QWidget();
    statsWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsWidget);
    statsLayout->setContentsMargins(0, 0, 0, 0);
    statsLayout->setSpacing(20);

    auto makeStatBox = [](const QString &num, const QString &label) -> QWidget* {
        QWidget *box = new QWidget();
        box->setStyleSheet(
            "QWidget { background-color: rgba(163,217,119,0.15); border: 1px solid rgba(163,217,119,0.4); border-radius: 10px; padding: 10px; }"
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

    QLabel *footerLabel = new QLabel("© 2025 TuniWaste — Tous droits réservés");
    footerLabel->setStyleSheet("font-size: 11px; color: rgba(200,230,200,0.6); background: transparent;");
    footerLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(footerLabel);

    mainLayout->addWidget(leftPanel);

    // ── Panneau droit (formulaire de connexion) ───────────────────────────────
    QWidget *rightPanel = new QWidget();
    rightPanel->setStyleSheet("QWidget { background-color: #F0F7EC; }");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setAlignment(Qt::AlignCenter);
    rightLayout->setContentsMargins(40, 40, 40, 40);

    QWidget *card = new QWidget();
    card->setFixedWidth(520);
    card->setObjectName("loginCard");
    card->setStyleSheet(
        "QWidget#loginCard { background-color: #FFFFFF; border-radius: 20px; border: 1px solid #D4EDDA; }"
        );

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(0);
    cardLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *cardTopBar = new QWidget();
    cardTopBar->setFixedHeight(8);
    cardTopBar->setStyleSheet(
        "QWidget { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #3B6B35,stop:0.5 #6FA85E,stop:1 #A3D977); border-radius: 20px; }"
        );
    cardLayout->addWidget(cardTopBar);

    QWidget *cardInner = new QWidget();
    cardInner->setStyleSheet("QWidget { background: transparent; }");
    QVBoxLayout *innerLayout = new QVBoxLayout(cardInner);
    innerLayout->setSpacing(22);
    innerLayout->setContentsMargins(55, 45, 55, 50);

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

    QString fieldStyle =
        "QLineEdit { padding: 14px 16px; border: 2px solid #E0EDD8; border-radius: 10px; font-size: 14px; background-color: #F8FCF6; color: #2C3E25; }"
        "QLineEdit:focus { border: 2px solid #6FA85E; background-color: #FFFFFF; }";
    QString labelStyle = "font-size: 13px; font-weight: bold; color: #3B6B35; background: transparent;";

    // ── Email login ───────────────────────────────────────────────────────────
    QLabel *emailLabel = new QLabel("📧  Adresse Email");
    emailLabel->setStyleSheet(labelStyle);
    innerLayout->addWidget(emailLabel);

    loginEmailEdit = new QLineEdit();
    loginEmailEdit->setPlaceholderText("nom@example.com");
    loginEmailEdit->setFixedHeight(56);
    loginEmailEdit->setStyleSheet(fieldStyle);
    innerLayout->addWidget(loginEmailEdit);

    // ── Mot de passe login ────────────────────────────────────────────────────
    QLabel *passwordLabel = new QLabel("🔒  Mot de passe");
    passwordLabel->setStyleSheet(labelStyle);
    innerLayout->addWidget(passwordLabel);

    loginPasswordEdit = new QLineEdit();
    loginPasswordEdit->setPlaceholderText("••••••••••••");
    loginPasswordEdit->setEchoMode(QLineEdit::Password);
    loginPasswordEdit->setFixedHeight(56);
    loginPasswordEdit->setStyleSheet(fieldStyle);
    innerLayout->addWidget(loginPasswordEdit);

    innerLayout->addSpacing(6);

    QPushButton *loginButton = new QPushButton("  Se connecter  →");
    loginButton->setFixedHeight(58);
    loginButton->setCursor(Qt::PointingHandCursor);
    loginButton->setStyleSheet(
        "QPushButton { background-color: #6FA85E; color: #FFFFFF; font-size: 16px; font-weight: bold; border: none; border-radius: 10px; letter-spacing: 1px; }"
        "QPushButton:hover { background-color: #5A9048; }"
        "QPushButton:pressed { background-color: #3B6B35; }"
        );
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    innerLayout->addWidget(loginButton);

    QPushButton *forgotButton = new QPushButton("Mot de passe oublié ?");
    forgotButton->setStyleSheet(
        "QPushButton { color: #6FA85E; font-size: 13px; background: transparent; border: none; text-decoration: underline; }"
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

// ══════════════════════════════════════════════════════════════════════════════
//  createSidebar()
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::createSidebar()
{
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(250);
    sidebar->setStyleSheet("QWidget { background-color: #6FA85E; }");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    QWidget *logoContainer = new QWidget();
    logoContainer->setStyleSheet("QWidget { background-color: #6FA85E; }");
    QHBoxLayout *logoLayout = new QHBoxLayout(logoContainer);
    logoLayout->setContentsMargins(15, 15, 15, 15);
    logoLayout->setSpacing(10);
    logoLayout->setAlignment(Qt::AlignLeft);

    QLabel *logoLabel = new QLabel();
    QPixmap logo("magee.png");
    if (logo.isNull()) { logo.load(":/magee.png"); }
    if (logo.isNull()) { logo.load(QCoreApplication::applicationDirPath() + "/magee.png"); }
    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(45, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoLabel->setStyleSheet("background: transparent;");
    } else {
        logoLabel->setText("🗑️");
        logoLabel->setStyleSheet("font-size: 35px; background: transparent;");
    }
    logoLayout->addWidget(logoLabel);

    QLabel *appTitleLabel = new QLabel("TuniWaste");
    appTitleLabel->setStyleSheet("color: #FFFFFF; font-size: 20px; font-weight: bold; background: transparent;");
    appTitleLabel->setAlignment(Qt::AlignVCenter);
    logoLayout->addWidget(appTitleLabel);
    logoLayout->addStretch();
    sidebarLayout->addWidget(logoContainer);

    QString buttonStyle =
        "QPushButton { background-color: #6FA85E; color: #FFFFFF; text-align: left; padding: 14px 15px; border: none; font-size: 14px; margin-bottom: 4px; }"
        "QPushButton:hover { background-color: #7DB86D; }";

    sidebarLayout->addWidget([&]{ auto b = new QPushButton("🏠  Tableau de bord"); b->setStyleSheet(buttonStyle); return b; }());
    sidebarLayout->addWidget([&]{ auto b = new QPushButton("👥  Gestion utilisateurs"); b->setStyleSheet(buttonStyle); return b; }());
    sidebarLayout->addWidget([&]{ auto b = new QPushButton("🚛  Gestion camions"); b->setStyleSheet(buttonStyle); return b; }());
    sidebarLayout->addWidget([&]{ auto b = new QPushButton("🗑️  Gestion poubelles"); b->setStyleSheet(buttonStyle); return b; }());
    sidebarLayout->addWidget([&]{ auto b = new QPushButton("📍  Gestion zones"); b->setStyleSheet(buttonStyle); return b; }());
    sidebarLayout->addWidget([&]{ auto b = new QPushButton("♻️  Recyclage"); b->setStyleSheet(buttonStyle); return b; }());
    sidebarLayout->addWidget([&]{ auto b = new QPushButton("📋  Suivi collectes"); b->setStyleSheet(buttonStyle); return b; }());
    sidebarLayout->addWidget([&]{ auto b = new QPushButton("📊  Rapports"); b->setStyleSheet(buttonStyle); return b; }());
    sidebarLayout->addStretch();
    sidebarLayout->addWidget([&]{ auto b = new QPushButton("⚙️  Parametres"); b->setStyleSheet(buttonStyle); return b; }());

    return sidebar;
}

// ══════════════════════════════════════════════════════════════════════════════
//  setupUserManagementScreen()
// ══════════════════════════════════════════════════════════════════════════════
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

    // Titre
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
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #6FA85E; background: transparent;");
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();

    QLabel *breadcrumbLabel = new QLabel("[HOME] Tableau de bord");
    breadcrumbLabel->setStyleSheet(
        "font-size: 14px; color: #999999; background-color: #FFFFFF; padding: 8px 15px; border: 1px solid #DDDDDD; border-radius: 4px;"
        );
    titleLayout->addWidget(breadcrumbLabel);
    topLayout->addWidget(titleWidget);

    QLabel *pathLabel = new QLabel("Tableau de bord / Gestion des utilisateurs");
    pathLabel->setStyleSheet("font-size: 13px; color: #999999; background: transparent;");
    topLayout->addWidget(pathLabel);
    contentLayout->addWidget(topWidget);

    QLabel *pageTitle = new QLabel("Gestion des utilisateurs");
    pageTitle->setStyleSheet("font-size: 28px; font-weight: bold; color: #000000; background: transparent;");
    contentLayout->addWidget(pageTitle);

    // ── Splitter : formulaire gauche | tableau droite ─────────────────────────
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setHandleWidth(10);
    splitter->setStyleSheet("QSplitter::handle { background-color: #DDDDDD; margin: 2px; }");

    // ── Formulaire ────────────────────────────────────────────────────────────
    QGroupBox *formGroupBox = new QGroupBox("Formulaire Utilisateur");
    formGroupBox->setMinimumWidth(350);
    formGroupBox->setMaximumWidth(450);
    formGroupBox->setStyleSheet(
        "QGroupBox { background-color: #FFFFFF; border: 2px solid #6FA85E; border-radius: 8px; margin-top: 10px; padding: 15px; font-size: 16px; font-weight: bold; color: #6FA85E; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 5px 10px; background-color: #FFFFFF; }"
        );

    QVBoxLayout *formGroupLayout = new QVBoxLayout(formGroupBox);
    formGroupLayout->setSpacing(10);
    formGroupLayout->setContentsMargins(10, 10, 10, 10);

    // Photo
    QHBoxLayout *photoTopLayout = new QHBoxLayout();
    photoTopLayout->setAlignment(Qt::AlignCenter);

    formPhotoLabel = new QLabel();
    formPhotoLabel->setFixedSize(80, 80);
    formPhotoLabel->setStyleSheet(
        "QLabel { border: 2px solid #DDDDDD; border-radius: 40px; background-color: #F5F5F5; font-size: 35px; }"
        );
    formPhotoLabel->setAlignment(Qt::AlignCenter);
    formPhotoLabel->setText("👤");
    photoTopLayout->addWidget(formPhotoLabel);
    formGroupLayout->addLayout(photoTopLayout);

    QPushButton *browsePhotoBtn = new QPushButton("📷 Photo");
    browsePhotoBtn->setStyleSheet(
        "QPushButton { background-color: #6FA85E; color: #FFFFFF; padding: 6px 12px; border: none; border-radius: 5px; font-weight: bold; font-size: 12px; }"
        "QPushButton:hover { background-color: #5A8F47; }"
        );
    browsePhotoBtn->setCursor(Qt::PointingHandCursor);
    connect(browsePhotoBtn, &QPushButton::clicked, this, &MainWindow::onBrowsePhotoClicked);
    formGroupLayout->addWidget(browsePhotoBtn);
    formGroupLayout->addSpacing(5);

    // Champs
    QFormLayout *formFieldsLayout = new QFormLayout();
    formFieldsLayout->setSpacing(8);
    formFieldsLayout->setLabelAlignment(Qt::AlignRight);

    QString labelStyle = "font-size: 12px; font-weight: bold; color: #000000; background: transparent;";
    QString lineEditStyle =
        "QLineEdit { padding: 6px; border: 2px solid #CCCCCC; border-radius: 5px; background-color: #FFFFFF; color: #000000; font-size: 12px; }"
        "QLineEdit:focus { border: 2px solid #6FA85E; }";
    QString comboStyle =
        "QComboBox { padding: 6px; border: 2px solid #CCCCCC; border-radius: 5px; background-color: #FFFFFF; color: #000000; font-size: 12px; font-weight: normal; }"
        "QComboBox:focus { border: 2px solid #6FA85E; }"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; border-right: 5px solid transparent; border-top: 5px solid #666666; width: 0; height: 0; }"
        "QComboBox QAbstractItemView { background-color: #FFFFFF; color: #000000; selection-background-color: #6FA85E; selection-color: #FFFFFF; border: 1px solid #CCCCCC; padding: 5px; }";

    auto makeLabel = [&](const QString &text) {
        QLabel *l = new QLabel(text);
        l->setStyleSheet(labelStyle);
        return l;
    };

    formFirstNameEdit = new QLineEdit();
    formFirstNameEdit->setPlaceholderText("Prénom *");
    formFirstNameEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(makeLabel("Prénom:"), formFirstNameEdit);

    formLastNameEdit = new QLineEdit();
    formLastNameEdit->setPlaceholderText("Nom de famille *");
    formLastNameEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(makeLabel("Nom:"), formLastNameEdit);

    formEmailEdit = new QLineEdit();
    formEmailEdit->setPlaceholderText("nom@example.com *");
    formEmailEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(makeLabel("Email:"), formEmailEdit);

    formPhoneEdit = new QLineEdit();
    formPhoneEdit->setPlaceholderText("+216 XX XXX XXX *");
    formPhoneEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(makeLabel("Téléphone:"), formPhoneEdit);

    formGenderCombo = new QComboBox();
    formGenderCombo->addItem("Homme");
    formGenderCombo->addItem("Femme");
    formGenderCombo->setStyleSheet(comboStyle);
    formFieldsLayout->addRow(makeLabel("Sexe:"), formGenderCombo);

    formCityEdit = new QLineEdit();
    formCityEdit->setPlaceholderText("Ville *");
    formCityEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(makeLabel("Ville:"), formCityEdit);

    formPostalCodeEdit = new QLineEdit();
    formPostalCodeEdit->setPlaceholderText("1000 *");
    formPostalCodeEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(makeLabel("Code Postal:"), formPostalCodeEdit);

    formPasswordEdit = new QLineEdit();
    formPasswordEdit->setPlaceholderText("min. 6 caractères *");
    formPasswordEdit->setEchoMode(QLineEdit::Password);
    formPasswordEdit->setStyleSheet(lineEditStyle);
    formFieldsLayout->addRow(makeLabel("Mot de passe:"), formPasswordEdit);

    formRoleCombo = new QComboBox();
    formRoleCombo->addItem("Administrateur");
    formRoleCombo->addItem("Employe");
    formRoleCombo->setStyleSheet(comboStyle);
    formFieldsLayout->addRow(makeLabel("Rôle:"), formRoleCombo);

    formGroupLayout->addLayout(formFieldsLayout);
    formGroupLayout->addSpacing(10);

    // Boutons
    QHBoxLayout *formButtonsLayout = new QHBoxLayout();
    formButtonsLayout->setSpacing(10);

    QPushButton *clearBtn = new QPushButton("🔄 Nouveau");
    clearBtn->setStyleSheet(
        "QPushButton { background-color: #999999; color: #FFFFFF; padding: 10px 20px; border: none; border-radius: 5px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background-color: #777777; }"
        );
    clearBtn->setCursor(Qt::PointingHandCursor);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearFormClicked);
    formButtonsLayout->addWidget(clearBtn);

    QPushButton *saveBtn = new QPushButton("💾 Enregistrer");
    saveBtn->setStyleSheet(
        "QPushButton { background-color: #A3D977; color: #FFFFFF; padding: 10px 20px; border: none; border-radius: 5px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background-color: #8FC65E; }"
        );
    saveBtn->setCursor(Qt::PointingHandCursor);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveUserClicked);
    formButtonsLayout->addWidget(saveBtn);

    formGroupLayout->addLayout(formButtonsLayout);

    // ── Tableau droite ────────────────────────────────────────────────────────
    QWidget *tableWidget = new QWidget();
    tableWidget->setStyleSheet("QWidget { background-color: transparent; }");
    QVBoxLayout *tableLayout = new QVBoxLayout(tableWidget);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(15);

    QWidget *searchFilterWidget = new QWidget();
    searchFilterWidget->setStyleSheet(
        "QWidget { background-color: #FFFFFF; border: 1px solid #E0E0E0; border-radius: 6px; padding: 15px; }"
        );
    QHBoxLayout *searchFilterLayout = new QHBoxLayout(searchFilterWidget);
    searchFilterLayout->setSpacing(15);

    QLineEdit *searchNameEdit = new QLineEdit();
    searchNameEdit->setPlaceholderText("🔍 Rechercher (nom, email, ville...)");
    searchNameEdit->setFixedWidth(250);
    searchNameEdit->setStyleSheet(
        "QLineEdit { padding: 10px 15px; border: 2px solid #DDDDDD; border-radius: 6px; background-color: #FFFFFF; color: #000000; font-size: 14px; }"
        "QLineEdit:focus { border: 2px solid #6FA85E; }"
        );
    searchFilterLayout->addWidget(searchNameEdit);
    searchFilterLayout->addStretch();

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
        "QComboBox { padding: 10px 15px; border: 1px solid #DDDDDD; border-radius: 4px; background-color: #FFFFFF; color: #000000; font-size: 13px; font-weight: bold; }"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; border-right: 5px solid transparent; border-top: 5px solid #666666; }"
        "QComboBox QAbstractItemView { background-color: #FFFFFF; color: #000000; selection-background-color: #6FA85E; selection-color: #FFFFFF; border: 1px solid #DDDDDD; }"
        );
    connect(sortCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [this](int index) {
                switch(index) {
                case 0: currentSortColumn = 0; currentSortOrder = Qt::DescendingOrder; break;
                case 1: currentSortColumn = 0; currentSortOrder = Qt::AscendingOrder;  break;
                case 2: currentSortColumn = 2; currentSortOrder = Qt::AscendingOrder;  break;
                case 3: currentSortColumn = 3; currentSortOrder = Qt::AscendingOrder;  break;
                }
                filterAndSortUsers();
            });
    searchFilterLayout->addWidget(sortCombo);

    roleFilter = new QComboBox();
    roleFilter->addItem("Tous les rôles");
    roleFilter->addItem("Administrateur");
    roleFilter->addItem("Employe");
    roleFilter->setFixedWidth(180);
    roleFilter->setStyleSheet(
        "QComboBox { padding: 10px 15px; border: 1px solid #DDDDDD; border-radius: 4px; background-color: #FFFFFF; color: #000000; font-size: 14px; }"
        "QComboBox:focus { border: 1px solid #629952; }"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; border-right: 5px solid transparent; border-top: 5px solid #666666; }"
        "QComboBox QAbstractItemView { background-color: #FFFFFF; color: #000000; selection-background-color: #629952; selection-color: #FFFFFF; border: 1px solid #DDDDDD; }"
        );
    connect(roleFilter, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [this](int) { filterAndSortUsers(); });
    searchFilterLayout->addWidget(roleFilter);

    QPushButton *exportPdfBtn = new QPushButton("📄 PDF");
    exportPdfBtn->setStyleSheet(
        "QPushButton { background-color: #FF8C42; color: #FFFFFF; padding: 10px 20px; border: none; border-radius: 4px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background-color: #FF7A29; }"
        );
    exportPdfBtn->setCursor(Qt::PointingHandCursor);
    connect(exportPdfBtn, &QPushButton::clicked, this, &MainWindow::onExportPdfClicked);
    searchFilterLayout->addWidget(exportPdfBtn);
    tableLayout->addWidget(searchFilterWidget);

    // champ de recherche caché (pour le backend)
    searchEdit = new QLineEdit();
    searchEdit->setVisible(false);
    connect(searchNameEdit, &QLineEdit::textChanged, [this](const QString &text) {
        searchEdit->setText(text);
        filterAndSortUsers();
    });

    // Tableau
    userTable = new QTableWidget();
    userTable->setColumnCount(8);
    userTable->setHorizontalHeaderLabels({"ID", "Prénom", "Nom", "Email", "Sexe", "Ville", "Rôle", "Actions"});
    userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    userTable->horizontalHeader()->setStretchLastSection(false);
    userTable->setColumnWidth(0, 40);
    userTable->setColumnWidth(1, 90);
    userTable->setColumnWidth(2, 90);
    userTable->setColumnWidth(3, 160);
    userTable->setColumnWidth(4, 90);
    userTable->setColumnWidth(5, 90);
    userTable->setColumnWidth(6, 165);
    userTable->setColumnWidth(7, 210);
    for (int i = 0; i < 7; i++)
        userTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    userTable->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    userTable->verticalHeader()->setVisible(false);
    userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTable->setSelectionMode(QAbstractItemView::SingleSelection);
    userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    userTable->setAlternatingRowColors(true);
    userTable->setSortingEnabled(true);
    userTable->setShowGrid(true);
    userTable->setMinimumWidth(850);
    userTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    userTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    userTable->setStyleSheet(
        "QTableWidget { background-color: #FFFFFF; border: 1px solid #E0E0E0; gridline-color: #E5E5E5; color: #000000; font-size: 14px; }"
        "QTableWidget::item { padding: 10px 8px; border-bottom: 1px solid #E5E5E5; }"
        "QTableWidget::item:selected { background-color: #F0F7ED; color: #000000; }"
        "QHeaderView::section { background-color: #F8F8F8; color: #000000; padding: 12px 8px; border: none; border-bottom: 2px solid #E0E0E0; border-right: 1px solid #E5E5E5; font-weight: bold; font-size: 14px; }"
        "QTableWidget::item:alternate { background-color: #FAFAFA; }"
        );
    tableLayout->addWidget(userTable);

    splitter->addWidget(formGroupBox);
    splitter->addWidget(tableWidget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    contentLayout->addWidget(splitter);
    mainLayout->addWidget(contentArea);

    stackedWidget->addWidget(mainWidget);
}

// ══════════════════════════════════════════════════════════════════════════════
//  updateUserTable() — remplit le QTableWidget depuis filteredUsers
// ══════════════════════════════════════════════════════════════════════════════
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
        const UserRow &user = filteredUsers[i];

        auto makeItem = [&](const QString &text) {
            QTableWidgetItem *item = new QTableWidgetItem(text);
            item->setTextAlignment(Qt::AlignCenter);
            item->setFont(cellFont);
            return item;
        };

        userTable->setItem(i, 0, makeItem(QString::number(user.id)));
        userTable->setItem(i, 1, makeItem(user.prenom));
        userTable->setItem(i, 2, makeItem(user.nom));
        userTable->setItem(i, 3, makeItem(user.email));
        userTable->setItem(i, 4, makeItem(user.sexe));
        userTable->setItem(i, 5, makeItem(user.ville));

        // Badge rôle coloré
        QWidget *roleWidget = new QWidget();
        roleWidget->setStyleSheet("background-color: transparent;");
        QHBoxLayout *roleLayout = new QHBoxLayout(roleWidget);
        roleLayout->setContentsMargins(5, 5, 5, 5);
        roleLayout->setAlignment(Qt::AlignCenter);
        QLabel *roleLabel = new QLabel(user.role);
        roleLabel->setAlignment(Qt::AlignCenter);
        if (user.role == "Administrateur") {
            roleLabel->setStyleSheet(
                "background-color: #FFA726; color: #FFFFFF; padding: 6px 6px; border-radius: 4px; font-weight: bold; font-size: 10px; min-width: 140px; max-width: 140px;"
                );
        } else {
            roleLabel->setStyleSheet(
                "background-color: #5DADE2; color: #FFFFFF; padding: 6px 6px; border-radius: 4px; font-weight: bold; font-size: 10px; min-width: 90px; max-width: 90px;"
                );
        }
        roleLayout->addWidget(roleLabel);
        QTableWidgetItem *roleSortItem = new QTableWidgetItem(user.role);
        roleSortItem->setForeground(QBrush(Qt::transparent));
        userTable->setItem(i, 6, roleSortItem);
        userTable->setCellWidget(i, 6, roleWidget);

        // Boutons actions
        QWidget *actionWidget = new QWidget();
        QVBoxLayout *actionLayout = new QVBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 8, 5, 8);
        actionLayout->setSpacing(12);
        actionLayout->setAlignment(Qt::AlignCenter);

        QPushButton *editBtn = new QPushButton("Modifier");
        editBtn->setCursor(Qt::PointingHandCursor);
        editBtn->setFixedSize(165, 30);
        editBtn->setStyleSheet(
            "QPushButton { background-color: #4CAF50; color: #FFFFFF; padding: 6px 12px; border: none; border-radius: 5px; font-size: 12px; font-weight: bold; }"
            "QPushButton:hover { background-color: #45A049; }"
            );
        connect(editBtn, &QPushButton::clicked, [this, i]() { onModifyUser(i); });

        QPushButton *deleteBtn = new QPushButton("Supprimer");
        deleteBtn->setCursor(Qt::PointingHandCursor);
        deleteBtn->setFixedSize(165, 30);
        deleteBtn->setStyleSheet(
            "QPushButton { background-color: #F44336; color: #FFFFFF; padding: 6px 12px; border: none; border-radius: 5px; font-size: 12px; font-weight: bold; }"
            "QPushButton:hover { background-color: #DA190B; }"
            );
        connect(deleteBtn, &QPushButton::clicked, [this, i]() { onDeleteUser(i); });

        actionLayout->addWidget(editBtn);
        actionLayout->addWidget(deleteBtn);
        userTable->setCellWidget(i, 7, actionWidget);
    }

    for (int i = 0; i < userTable->rowCount(); ++i)
        userTable->setRowHeight(i, 85);

    userTable->setSortingEnabled(true);
}

// ══════════════════════════════════════════════════════════════════════════════
//  clearForm()
// ══════════════════════════════════════════════════════════════════════════════
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
        "QLabel { border: 2px solid #DDDDDD; border-radius: 40px; background-color: #F5F5F5; font-size: 35px; }"
        );
    editingUserId = -1;
}

// ══════════════════════════════════════════════════════════════════════════════
//  loadUserToForm() — remplit le formulaire depuis une ligne du tableau
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::loadUserToForm(const UserRow &user)
{
    formFirstNameEdit->setText(user.prenom);
    formLastNameEdit->setText(user.nom);
    formEmailEdit->setText(user.email);
    formPhoneEdit->setText(user.telephone);
    formGenderCombo->setCurrentText(user.sexe);
    formCityEdit->setText(user.ville);
    formPostalCodeEdit->setText(user.codePostal);
    formRoleCombo->setCurrentText(user.role);
    formPhotoPath = user.photo;
    formPasswordEdit->setText(user.motDePasse);

    if (!user.photo.isEmpty() && QFile::exists(user.photo)) {
        QPixmap photo(user.photo);
        formPhotoLabel->setPixmap(photo.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        formPhotoLabel->setStyleSheet("QLabel { border: 2px solid #DDDDDD; border-radius: 40px; }");
    } else {
        formPhotoLabel->setText("👤");
        formPhotoLabel->setStyleSheet(
            "QLabel { border: 2px solid #DDDDDD; border-radius: 40px; background-color: #F5F5F5; font-size: 35px; }"
            );
    }
    editingUserId = user.id;
}

// ══════════════════════════════════════════════════════════════════════════════
//  filterAndSortUsers()
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::filterAndSortUsers()
{
    QString searchText  = searchEdit->text().trimmed().toLower();
    QString selectedRole = roleFilter->currentText();

    filteredUsers.clear();
    for (const UserRow &user : std::as_const(users)) {
        bool matchesSearch = searchText.isEmpty() ||
                             QString::number(user.id).contains(searchText) ||
                             user.prenom.toLower().contains(searchText) ||
                             user.nom.toLower().contains(searchText) ||
                             user.email.toLower().contains(searchText) ||
                             user.telephone.contains(searchText) ||
                             user.sexe.toLower().contains(searchText) ||
                             user.ville.toLower().contains(searchText) ||
                             user.codePostal.contains(searchText) ||
                             user.role.toLower().contains(searchText);

        bool matchesRole = (selectedRole == "Tous les rôles") || (user.role == selectedRole);

        if (matchesSearch && matchesRole)
            filteredUsers.append(user);
    }

    if (currentSortColumn >= 0) {
        std::sort(filteredUsers.begin(), filteredUsers.end(),
                  [this](const UserRow &a, const UserRow &b) {
                      switch (currentSortColumn) {
                      case 0: return (currentSortOrder == Qt::AscendingOrder) ? (a.id < b.id) : (a.id > b.id);
                      case 1: return (currentSortOrder == Qt::AscendingOrder) ? (a.prenom.toLower() < b.prenom.toLower()) : (a.prenom.toLower() > b.prenom.toLower());
                      case 2: return (currentSortOrder == Qt::AscendingOrder) ? (a.nom.toLower() < b.nom.toLower()) : (a.nom.toLower() > b.nom.toLower());
                      case 3: return (currentSortOrder == Qt::AscendingOrder) ? (a.email.toLower() < b.email.toLower()) : (a.email.toLower() > b.email.toLower());
                      default: return false;
                      }
                  });
    }

    updateUserTable();
}

// ══════════════════════════════════════════════════════════════════════════════
//  onLoginClicked() — VALIDATION email + mot de passe avant connexion
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::onLoginClicked()
{
    QString email    = loginEmailEdit->text().trimmed();
    QString password = loginPasswordEdit->text();

    // ── Champs vides ──────────────────────────────────────────────────────────
    if (email.isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "Veuillez entrer votre adresse email.");
        loginEmailEdit->setFocus();
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "Veuillez entrer votre mot de passe.");
        loginPasswordEdit->setFocus();
        return;
    }

    // ── Format email ──────────────────────────────────────────────────────────
    QRegularExpression emailRegex(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Email invalide",
                             "L'adresse email n'est pas valide.\nExemple : nom@example.com");
        loginEmailEdit->setFocus();
        return;
    }

    // ── Vérification dans la BD ───────────────────────────────────────────────
    QSqlDatabase db = Database::getInstance().getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT ID_UTILISATEUR FROM UTILISATEUR WHERE EMAIL = :email AND MOT_DE_PASSE = :password");
    query.bindValue(":email",    email);
    query.bindValue(":password", password);

    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "Connexion refusée",
                             "Email ou mot de passe incorrect.\nVérifiez vos identifiants.");
        loginPasswordEdit->clear();
        loginEmailEdit->setFocus();
        return;
    }

    // ── Connexion réussie ─────────────────────────────────────────────────────
    stackedWidget->setCurrentIndex(1);
}

// ══════════════════════════════════════════════════════════════════════════════
//  onSaveUserClicked() — VALIDATION complète + ajouter() ou modifier()
//  Aucune requête SQL ici — tout est dans utilisateur.cpp
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::onSaveUserClicked()
{
    QString prenom     = formFirstNameEdit->text().trimmed();
    QString nom        = formLastNameEdit->text().trimmed();
    QString email      = formEmailEdit->text().trimmed();
    QString telephone  = formPhoneEdit->text().trimmed();
    QString sexe       = formGenderCombo->currentText();
    QString ville      = formCityEdit->text().trimmed();
    QString codePostal = formPostalCodeEdit->text().trimmed();
    QString role       = formRoleCombo->currentText();
    QString password   = formPasswordEdit->text().trimmed();

    // ── Champs obligatoires ───────────────────────────────────────────────────
    if (prenom.isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "Le prénom est obligatoire.");
        formFirstNameEdit->setFocus();
        return;
    }
    if (nom.isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "Le nom est obligatoire.");
        formLastNameEdit->setFocus();
        return;
    }
    if (email.isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "L'email est obligatoire.");
        formEmailEdit->setFocus();
        return;
    }

    // ── Format email ──────────────────────────────────────────────────────────
    QRegularExpression emailRegex(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Email invalide",
                             "L'adresse email n'est pas valide.\nExemple : nom@example.com");
        formEmailEdit->setFocus();
        return;
    }

    // ── Téléphone ─────────────────────────────────────────────────────────────
    if (telephone.isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "Le téléphone est obligatoire.");
        formPhoneEdit->setFocus();
        return;
    }
    QRegularExpression phoneRegex(R"(^\+?[\d\s\-]{7,20}$)");
    if (!phoneRegex.match(telephone).hasMatch()) {
        QMessageBox::warning(this, "Téléphone invalide",
                             "Le numéro de téléphone n'est pas valide.\nExemple : +216 22 123 456");
        formPhoneEdit->setFocus();
        return;
    }

    // ── Ville ─────────────────────────────────────────────────────────────────
    if (ville.isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "La ville est obligatoire.");
        formCityEdit->setFocus();
        return;
    }

    // ── Code postal ───────────────────────────────────────────────────────────
    if (codePostal.isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "Le code postal est obligatoire.");
        formPostalCodeEdit->setFocus();
        return;
    }
    QRegularExpression postalRegex(R"(^\d{4,6}$)");
    if (!postalRegex.match(codePostal).hasMatch()) {
        QMessageBox::warning(this, "Code postal invalide",
                             "Le code postal doit contenir 4 à 6 chiffres.\nExemple : 1000");
        formPostalCodeEdit->setFocus();
        return;
    }

    // ── Mot de passe ──────────────────────────────────────────────────────────
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "Le mot de passe est obligatoire.");
        formPasswordEdit->setFocus();
        return;
    }
    if (password.length() < 6) {
        QMessageBox::warning(this, "Mot de passe trop court",
                             "Le mot de passe doit contenir au moins 6 caractères.");
        formPasswordEdit->setFocus();
        return;
    }

    // ── Appel Modèle — pas de SQL dans ce slot ────────────────────────────────
    if (editingUserId == -1) {
        // AJOUTER
        Utmp = Utilisateur(0, nom, prenom, email, telephone, password, role, ville, codePostal, sexe, formPhotoPath);
        if (Utmp.ajouter()) {
            QMessageBox::information(this, "Succès", "Utilisateur ajouté avec succès !");
            clearForm();
            refreshTable();   // actualisation après CREATE
        } else {
            QMessageBox::critical(this, "Erreur BD",
                                  "Impossible d'ajouter l'utilisateur.\n"
                                  "Vérifiez que l'email n'est pas déjà utilisé.");
        }
    } else {
        // MODIFIER
        Utmp = Utilisateur(editingUserId, nom, prenom, email, telephone, password, role, ville, codePostal, sexe, formPhotoPath);
        if (Utmp.modifier()) {
            QMessageBox::information(this, "Succès", "Utilisateur modifié avec succès !");
            clearForm();
            refreshTable();   // actualisation après UPDATE
        } else {
            QMessageBox::critical(this, "Erreur BD",
                                  "Impossible de modifier l'utilisateur.");
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  onModifyUser() — charge les données dans le formulaire
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::onModifyUser(int row)
{
    if (row < 0 || row >= filteredUsers.size()) return;
    loadUserToForm(filteredUsers[row]);
}

// ══════════════════════════════════════════════════════════════════════════════
//  onDeleteUser() — supprime via Utmp.supprimer() puis actualise
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::onDeleteUser(int row)
{
    if (row < 0 || row >= filteredUsers.size()) return;

    const UserRow &user = filteredUsers[row];

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmation",
        "Êtes-vous sûr de vouloir supprimer '" + user.prenom + " " + user.nom + "' ?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (Utmp.supprimer(user.id)) {
            if (editingUserId == user.id) clearForm();
            refreshTable();   // actualisation après DELETE
            QMessageBox::information(this, "Succès", "Utilisateur supprimé avec succès !");
        } else {
            QMessageBox::critical(this, "Erreur BD", "Impossible de supprimer cet utilisateur.");
        }
    }
}

void MainWindow::onBrowsePhotoClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "Choisir une photo de profil", QDir::homePath(),
        "Images (*.png *.jpg *.jpeg *.bmp)"
        );
    if (!fileName.isEmpty()) {
        formPhotoPath = fileName;
        QPixmap photo(fileName);
        formPhotoLabel->setPixmap(photo.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        formPhotoLabel->setStyleSheet("QLabel { border: 2px solid #DDDDDD; border-radius: 40px; }");
    }
}

void MainWindow::onClearFormClicked()  { clearForm(); }
void MainWindow::onSearchTextChanged(const QString &) { filterAndSortUsers(); }
void MainWindow::onSortByColumn(int column)
{
    if (currentSortColumn == column)
        currentSortOrder = (currentSortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
    else { currentSortColumn = column; currentSortOrder = Qt::AscendingOrder; }
    filterAndSortUsers();
}

void MainWindow::onForgotPasswordClicked()
{
    PasswordResetDialog dialog(this);
    dialog.exec();
}

// ══════════════════════════════════════════════════════════════════════════════
//  Export PDF
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::onExportPdfClicked()
{
    QString defaultFileName = "Liste_Utilisateurs_" +
                              QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".pdf";

    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF",
                                                    documentsPath + "/" + defaultFileName,
                                                    "Fichiers PDF (*.pdf)");
    if (fileName.isEmpty()) return;

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

    int pageWidth  = printer.pageRect(QPrinter::DevicePixel).width();
    int pageHeight = printer.pageRect(QPrinter::DevicePixel).height();
    int margin = 50;
    int yPos   = margin;

    painter.setPen(QColor(111, 168, 94));
    painter.setFont(titleFont);
    painter.drawText(margin, yPos, "TuniWaste - Liste des Utilisateurs");
    yPos += 60;

    painter.setFont(smallFont);
    painter.setPen(Qt::black);
    painter.drawText(margin, yPos,
                     "Généré le : " + QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm:ss"));
    yPos += 40;

    painter.setPen(QPen(QColor(224, 224, 224), 2));
    painter.drawLine(margin, yPos, pageWidth - margin, yPos);
    yPos += 30;

    painter.setFont(normalFont);
    painter.setPen(Qt::black);
    int nbAdmin = 0, nbEmp = 0;
    for (const UserRow &u : std::as_const(filteredUsers)) {
        if (u.role == "Administrateur") nbAdmin++;
        else nbEmp++;
    }
    painter.drawText(margin, yPos,
                     QString("Nombre total : %1  |  Administrateurs : %2  |  Employés : %3")
                         .arg(filteredUsers.size()).arg(nbAdmin).arg(nbEmp));
    yPos += 50;

    int colWidths[] = {50, 130, 130, 210, 80, 110, 100};
    int totalWidth  = 0;
    for (int w : colWidths) totalWidth += w;
    double scaleFactor = (double)(pageWidth - 2 * margin) / totalWidth;
    for (int i = 0; i < 7; i++) colWidths[i] = (int)(colWidths[i] * scaleFactor);

    painter.setFont(headerFont);
    painter.setPen(Qt::white);
    painter.setBrush(QColor(111, 168, 94));
    int headerHeight = 40;
    painter.drawRect(margin, yPos, pageWidth - 2 * margin, headerHeight);

    int xPos = margin;
    QStringList headers = {"ID", "Prénom", "Nom", "Email", "Sexe", "Ville", "Rôle"};
    for (int i = 0; i < headers.size(); i++) {
        painter.drawText(QRect(xPos + 10, yPos, colWidths[i] - 10, headerHeight),
                         Qt::AlignVCenter | Qt::AlignLeft, headers[i]);
        xPos += colWidths[i];
    }
    yPos += headerHeight;

    painter.setFont(normalFont);
    int rowHeight = 35;
    bool alternate = false;

    for (const UserRow &user : std::as_const(filteredUsers)) {
        if (yPos + rowHeight > pageHeight - margin) {
            printer.newPage();
            yPos = margin;
            painter.setPen(Qt::white);
            painter.setBrush(QColor(111, 168, 94));
            painter.setFont(headerFont);
            painter.drawRect(margin, yPos, pageWidth - 2 * margin, headerHeight);
            xPos = margin;
            for (int i = 0; i < headers.size(); i++) {
                painter.drawText(QRect(xPos + 10, yPos, colWidths[i] - 10, headerHeight),
                                 Qt::AlignVCenter | Qt::AlignLeft, headers[i]);
                xPos += colWidths[i];
            }
            yPos += headerHeight;
            painter.setFont(normalFont);
            alternate = false;
        }

        painter.setBrush(alternate ? QColor(249, 249, 249) : Qt::white);
        painter.setPen(QColor(224, 224, 224));
        painter.drawRect(margin, yPos, pageWidth - 2 * margin, rowHeight);
        painter.setPen(Qt::black);
        xPos = margin;

        QStringList rowData = {
            QString::number(user.id), user.prenom, user.nom,
            user.email, user.sexe, user.ville, user.role
        };
        for (int i = 0; i < rowData.size(); i++) {
            QString text = painter.fontMetrics().elidedText(rowData[i], Qt::ElideRight, colWidths[i] - 20);
            painter.drawText(QRect(xPos + 10, yPos, colWidths[i] - 15, rowHeight),
                             Qt::AlignVCenter | Qt::AlignLeft, text);
            xPos += colWidths[i];
        }
        yPos += rowHeight;
        alternate = !alternate;
    }

    yPos = pageHeight - margin + 20;
    painter.setFont(smallFont);
    painter.setPen(QColor(153, 153, 153));
    painter.drawText(margin, yPos, pageWidth - 2 * margin, 20, Qt::AlignCenter,
                     "TuniWaste © " + QString::number(QDateTime::currentDateTime().date().year()) +
                         " - Système de Gestion des Déchets");
    painter.end();

    QMessageBox::information(this, "Succès",
                             "PDF exporté avec succès !\n\nEmplacement : " + fileName);
}

// ══════════════════════════════════════════════════════════════════════════════
//  PasswordResetDialog
// ══════════════════════════════════════════════════════════════════════════════
PasswordResetDialog::PasswordResetDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Réinitialisation du mot de passe");
    setModal(true);
    setFixedSize(500, 280);
    setStyleSheet("QDialog { background-color: #FFFFFF; } QLabel { color: #000000; background: transparent; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Réinitialiser le mot de passe");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #000000; background: transparent;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *instructionLabel = new QLabel(
        "Entrez votre adresse email et nous vous enverrons un lien\npour réinitialiser votre mot de passe."
        );
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
        "QLineEdit { padding: 12px; border: 2px solid #CCCCCC; border-radius: 6px; background-color: #FFFFFF; color: #000000; font-size: 14px; }"
        "QLineEdit:focus { border: 2px solid #A3C651; }"
        );
    mainLayout->addWidget(emailEdit);
    mainLayout->addSpacing(10);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    QPushButton *cancelBtn = new QPushButton("Annuler");
    cancelBtn->setStyleSheet(
        "QPushButton { background-color: #F5F5F5; color: #000000; padding: 12px 24px; border: 2px solid #CCCCCC; border-radius: 6px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background-color: #F5F5F5; border: 2px solid #999999; }"
        );
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton *resetBtn = new QPushButton("Envoyer");
    resetBtn->setStyleSheet(
        "QPushButton { background-color: #A3C651; color: #FFFFFF; padding: 12px 24px; border: none; border-radius: 6px; font-weight: bold; font-size: 14px; }"
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
        QMessageBox::warning(this, "Champ requis", "Veuillez entrer votre adresse email.");
        emailEdit->setFocus();
        return;
    }

    QRegularExpression emailRegex(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Email invalide",
                             "L'adresse email n'est pas valide.\nExemple : nom@example.com");
        emailEdit->setFocus();
        return;
    }

    QMessageBox::information(this, "Succès",
                             "Un email de réinitialisation a été envoyé à " + email +
                                 "\n\nVeuillez vérifier votre boîte de réception.");
    accept();
}
