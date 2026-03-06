#include "mainwindow.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QEasingCurve>
#include <QTimer>
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
#include <QValidator>
#include <QDir>
#include <QLinearGradient>
#include <QPageLayout>
#include <QPageSize>

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

    QSqlDatabase db = Connection::instance()->getDatabase();
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
    if (logo.isNull()) logo.load(QDir::currentPath() + "/magee.png");
    if (logo.isNull()) logo.load("../magee.png");
    if (logo.isNull()) logo.load("../../magee.png");
    if (!logo.isNull()) {
        leftLogoLabel->setPixmap(logo.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        leftLogoLabel->setStyleSheet("background: transparent;");
    } else {
        leftLogoLabel->setText("🤖");
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
    if (logo.isNull()) { logo.load(QDir::currentPath() + "/magee.png"); }
    if (logo.isNull()) { logo.load("../magee.png"); }
    if (logo.isNull()) { logo.load("../../magee.png"); }
    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(45, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoLabel->setStyleSheet("background: transparent;");
    } else {
        logoLabel->setText("🤖");
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
    pageTitle->setStyleSheet(
        "font-size: 28px; font-weight: bold; color: #FFFFFF; background-color: #6FA85E;"
        " border-radius: 10px; padding: 12px 30px;"
        );
    pageTitle->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(pageTitle);

    // ── Splitter : formulaire gauche | tableau droite ─────────────────────────
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setHandleWidth(10);
    splitter->setStyleSheet("QSplitter::handle { background-color: #DDDDDD; margin: 2px; }");

    // ── Formulaire ────────────────────────────────────────────────────────────
    QGroupBox *formGroupBox = new QGroupBox("");
    formGroupBox->setMinimumWidth(350);
    formGroupBox->setMaximumWidth(450);
    formGroupBox->setStyleSheet(
        "QGroupBox { background-color: #FFFFFF; border: 2px solid #6FA85E; border-radius: 8px; margin-top: 30px; padding: 15px; }"
        );

    QVBoxLayout *formGroupLayout = new QVBoxLayout(formGroupBox);
    formGroupLayout->setSpacing(10);
    formGroupLayout->setContentsMargins(10, 10, 10, 10);

    // ── Titre en haut du formulaire ───────────────────────────────────────────
    QWidget *formTitleWidget = new QWidget();
    formTitleWidget->setFixedHeight(46);
    formTitleWidget->setStyleSheet(
        "QWidget { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #3B6B35, stop:0.5 #6FA85E, stop:1 #A3D977);"
        " border-radius: 6px; }"
        );
    QHBoxLayout *formTitleLayout = new QHBoxLayout(formTitleWidget);
    formTitleLayout->setContentsMargins(12, 0, 12, 0);
    formTitleLayout->setSpacing(8);
    QLabel *fTitleIcon = new QLabel("\U0001F464");
    fTitleIcon->setStyleSheet("font-size: 18px; background: transparent;");
    formTitleLayout->addWidget(fTitleIcon);
    QLabel *fTitleText = new QLabel("Formulaire Utilisateur");
    fTitleText->setStyleSheet(
        "font-size: 14px; font-weight: bold; color: #FFFFFF; background: transparent;"
        );
    formTitleLayout->addWidget(fTitleText);
    formTitleLayout->addStretch();
    QLabel *fTitleBrand = new QLabel("TuniWaste");
    fTitleBrand->setStyleSheet(
        "font-size: 11px; color: rgba(255,255,255,0.80); background: transparent;"
        );
    formTitleLayout->addWidget(fTitleBrand);
    formGroupLayout->addWidget(formTitleWidget);

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
    formFirstNameEdit->setFixedHeight(34);
    formFieldsLayout->addRow(makeLabel("Prénom:"), formFirstNameEdit);

    formLastNameEdit = new QLineEdit();
    formLastNameEdit->setPlaceholderText("Nom de famille *");
    formLastNameEdit->setStyleSheet(lineEditStyle);
    formLastNameEdit->setFixedHeight(34);
    formFieldsLayout->addRow(makeLabel("Nom:"), formLastNameEdit);

    formEmailEdit = new QLineEdit();
    formEmailEdit->setPlaceholderText("nom@example.com *");
    formEmailEdit->setStyleSheet(lineEditStyle);
    formEmailEdit->setFixedHeight(34);
    formFieldsLayout->addRow(makeLabel("Email:"), formEmailEdit);

    formPhoneEdit = new QLineEdit();
    formPhoneEdit->setPlaceholderText("Ex: 22123456 *");
    formPhoneEdit->setStyleSheet(lineEditStyle);
    formPhoneEdit->setFixedHeight(34);
    formFieldsLayout->addRow(makeLabel("Téléphone:"), formPhoneEdit);

    formGenderCombo = new QComboBox();
    formGenderCombo->addItem("Homme");
    formGenderCombo->addItem("Femme");
    formGenderCombo->setStyleSheet(comboStyle);
    formFieldsLayout->addRow(makeLabel("Sexe:"), formGenderCombo);

    formCityEdit = new QLineEdit();
    formCityEdit->setPlaceholderText("Ville *");
    formCityEdit->setStyleSheet(lineEditStyle);
    formCityEdit->setFixedHeight(34);
    formFieldsLayout->addRow(makeLabel("Ville:"), formCityEdit);

    formPostalCodeEdit = new QLineEdit();
    formPostalCodeEdit->setPlaceholderText("1000 *");
    formPostalCodeEdit->setStyleSheet(lineEditStyle);
    formPostalCodeEdit->setFixedHeight(34);
    formFieldsLayout->addRow(makeLabel("Code Postal:"), formPostalCodeEdit);

    formPasswordEdit = new QLineEdit();
    formPasswordEdit->setPlaceholderText("min. 6 caractères *");
    formPasswordEdit->setEchoMode(QLineEdit::Password);
    formPasswordEdit->setStyleSheet(lineEditStyle);
    formPasswordEdit->setFixedHeight(34);
    formFieldsLayout->addRow(makeLabel("Mot de passe:"), formPasswordEdit);

    formRoleCombo = new QComboBox();
    formRoleCombo->addItem("Administrateur");
    formRoleCombo->addItem("Chauffeur");
    formRoleCombo->addItem("Responsable Camion");
    formRoleCombo->addItem("Responsable Recyclage");
    formRoleCombo->addItem("Responsable Zone");
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

    // (titre moved to top of form)

    // ── Tableau droite ────────────────────────────────────────────────────────
    QWidget *tableWidget = new QWidget();
    tableWidget->setStyleSheet("QWidget { background-color: transparent; }");
    QVBoxLayout *tableLayout = new QVBoxLayout(tableWidget);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(15);

    // ── Bouton chatbot en haut à droite du tableau ────────────────────────────
    QWidget *tableHeaderWidget = new QWidget();
    tableHeaderWidget->setStyleSheet("QWidget { background-color: transparent; }");
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout(tableHeaderWidget);
    tableHeaderLayout->setContentsMargins(0, 0, 0, 0);
    tableHeaderLayout->addStretch();

    QPushButton *chatBotIconBtn = new QPushButton("🤖");
    chatBotIconBtn->setFixedSize(52, 52);
    chatBotIconBtn->setCursor(Qt::PointingHandCursor);
    chatBotIconBtn->setToolTip("Assistant TuniWaste");
    chatBotIconBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #3B6B35; color: white; font-size: 22px;"
        "  border: none; border-radius: 26px; border: 3px solid #A3D977;"
        "}"
        "QPushButton:hover { background-color: #6FA85E; border: 3px solid #FFFFFF; }"
        "QPushButton:pressed { background-color: #2A4F28; }"
        );
    connect(chatBotIconBtn, &QPushButton::clicked, [this]() {
        if (chatBubble) chatBubble->triggerOpen();
    });
    tableHeaderLayout->addWidget(chatBotIconBtn);
    tableLayout->addWidget(tableHeaderWidget);

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
    roleFilter->addItem("Chauffeur");
    roleFilter->addItem("Responsable Camion");
    roleFilter->addItem("Responsable Recyclage");
    roleFilter->addItem("Responsable Zone");
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
        "QTableWidget::item:hover { background-color: transparent; }"
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

    // ── Bulle ChatBot — cachée (l'icône intégrée dans le tableau la déclenche) ─
    chatBubble = new ChatBubbleButton(mainWidget);
    chatBubble->hide();   // on masque la bulle flottante, on utilise le bouton tableau

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
        // Couleur selon le rôle — texte toujours centré
        QString badgeBase = "color: #FFFFFF; padding: 6px 8px; border-radius: 4px; font-weight: bold; font-size: 10px; qproperty-alignment: AlignCenter;";
        if (user.role == "Administrateur") {
            roleLabel->setStyleSheet("background-color: #FFA726; " + badgeBase);
        } else if (user.role == "Chauffeur") {
            roleLabel->setStyleSheet("background-color: #5DADE2; " + badgeBase);
        } else if (user.role == "Responsable Camion") {
            roleLabel->setStyleSheet("background-color: #8E44AD; " + badgeBase);
        } else if (user.role == "Responsable Recyclage") {
            roleLabel->setStyleSheet("background-color: #27AE60; " + badgeBase);
        } else {
            roleLabel->setStyleSheet("background-color: #E74C3C; " + badgeBase);
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
//  onLoginClicked() — VALIDATION email + mot de passe (sans OTP)
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::onLoginClicked()
{
    QString email    = loginEmailEdit->text().trimmed();
    QString password = loginPasswordEdit->text();

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

    QRegularExpression emailRegex(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Email invalide",
                             "L'adresse email n'est pas valide.\nExemple : nom@example.com");
        loginEmailEdit->setFocus();
        return;
    }

    QSqlDatabase db = Connection::instance()->getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT ID_UTILISATEUR FROM UTILISATEUR WHERE EMAIL = :email AND MOT_DE_PASSE = :password");
    query.bindValue(":email",    email);
    query.bindValue(":password", password);

    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "Connexion refusée",
                             "Email ou mot de passe incorrect.\nVérifiez vos identifiants.");
        loginPasswordEdit->clear();
        loginPasswordEdit->setFocus();
        return;
    }

    // Connexion directe sans OTP
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
    QRegularExpression nomRegex(R"(^[A-Za-zÀ-ÿ\s]+$)");
    if (!nomRegex.match(prenom).hasMatch()) {
        QMessageBox::warning(this, "Prénom invalide",
                             "Le prénom doit contenir uniquement des lettres.\n\n"
                             "❌  Chiffres et symboles non autorisés.\n\n"
                             "Exemples valides :\n"
                             "  Mohamed\n"
                             "  Ben Ali\n"
                             "  Fatma");
        formFirstNameEdit->setFocus();
        return;
    }
    if (nom.isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "Le nom est obligatoire.");
        formLastNameEdit->setFocus();
        return;
    }
    if (!nomRegex.match(nom).hasMatch()) {
        QMessageBox::warning(this, "Nom invalide",
                             "Le nom doit contenir uniquement des lettres.\n\n"
                             "❌  Chiffres et symboles non autorisés.\n\n"
                             "Exemples valides :\n"
                             "  Trabelsi\n"
                             "  Ben Salah\n"
                             "  Chaabane");
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
    QRegularExpression phoneRegex(R"(^[24579]\d{7}$)");
    if (!phoneRegex.match(telephone).hasMatch()) {
        QMessageBox::warning(this, "Téléphone invalide",
                             "Le numéro doit contenir exactement 8 chiffres\n"
                             "et commencer par l'un de ces chiffres :\n\n"
                             "  2X XXX XXX  →  Ooredoo\n"
                             "  4X XXX XXX  →  Lycamobile\n"
                             "  5X XXX XXX  →  Orange\n"
                             "  7X XXX XXX  →  Fixe\n"
                             "  9X XXX XXX  →  Tunisie Telecom");
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
        // AJOUTER — récupérer le prochain ID disponible
        int nextId = 1;
        QSqlQuery idQuery;
        if (idQuery.exec("SELECT MAX(ID_UTILISATEUR) FROM UTILISATEUR") && idQuery.next())
            nextId = idQuery.value(0).toInt() + 1;
        Utmp = Utilisateur(nextId, nom, prenom, email, telephone, password, role, ville, codePostal, sexe, formPhotoPath);
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
//  Export PDF  — professional table layout, DPI-aware, paginated
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::onExportPdfClicked()
{
    // ── File save dialog ──────────────────────────────────────────────────────
    QString defaultFileName = "Liste_Utilisateurs_" +
                              QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".pdf";
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF",
                                                    documentsPath + "/" + defaultFileName,
                                                    "Fichiers PDF (*.pdf)");
    if (fileName.isEmpty()) return;

    // ─────────────────────────────────────────────────────────────────────────
    //  KEY FIX: QPrinter::ScreenResolution (96 dpi) keeps font metrics in sync
    //  with layout metrics.  HighResolution (1200 dpi) causes the infamous
    //  character-stacking / text-overflow bug on Windows.
    // ─────────────────────────────────────────────────────────────────────────
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize::A4);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);

    QPainter p;
    if (!p.begin(&printer)) {
        QMessageBox::critical(this, "Erreur", "Impossible de creer le fichier PDF.");
        return;
    }

    // ── Page geometry (device pixels at 96 dpi) ───────────────────────────────
    const QRectF pr  = printer.pageRect(QPrinter::DevicePixel);
    const qreal  W   = pr.width();
    const qreal  H   = pr.height();
    const qreal  mL  = 28, mR = 28, mT = 28, mB = 32;
    const qreal  CW  = W - mL - mR;   // usable content width

    // ── Colours ───────────────────────────────────────────────────────────────
    const QColor cGreenDark ("#2E5D28");
    const QColor cGreenMid  ("#3B6B35");
    const QColor cGreenLight("#6FA85E");
    const QColor cGreenXL   ("#A3D977");
    const QColor cBorder    ("#C5DCB8");
    const QColor cAlt       ("#F2F9EE");
    const QColor cWhite     (Qt::white);
    const QColor cText      ("#1A2E17");
    const QColor cGray      ("#888888");

    // ── Fonts — use point sizes (device-independent) ──────────────────────────
    QFont fTitle("Arial", 13, QFont::Bold);
    QFont fDate ("Arial",  7);
    QFont fStat ("Arial",  7, QFont::Bold);
    QFont fHdr  ("Arial",  8, QFont::Bold);
    QFont fCell ("Arial",  8);
    QFont fFoot ("Arial",  7);

    // ── Column layout ─────────────────────────────────────────────────────────
    //   ID | Nom Complet | Telephone | Email | Sexe | Role | Ville
    struct Col { QString label; qreal w; };
    QVector<Col> cols = {
        {"ID",           0.055 * CW},
        {"Nom Complet",  0.160 * CW},
        {"Telephone",    0.120 * CW},
        {"Email",        0.235 * CW},
        {"Sexe",         0.060 * CW},
        {"Role",         0.185 * CW},
        {"Ville",        0.185 * CW}
    };
    // snap last column so total == CW exactly
    {
        qreal used = 0;
        for (int i = 0; i < cols.size()-1; i++) used += cols[i].w;
        cols.last().w = CW - used;
    }

    const qreal rowH  = 20;   // data row height (pt/px at 96 dpi)
    const qreal hdrH  = 22;   // column header height
    const qreal padX  =  5;   // horizontal text padding

    // ── helpers ───────────────────────────────────────────────────────────────
    // Draw a filled rectangle with bottom+right border line, then draw text
    auto drawCell = [&](qreal x, qreal y, qreal w, qreal h,
                        const QString &txt, const QFont &f,
                        Qt::Alignment align, QColor bg, QColor fg)
    {
        p.setPen(Qt::NoPen);
        p.setBrush(bg);
        p.drawRect(QRectF(x, y, w, h));

        p.setPen(QPen(cBorder, 0.5));
        p.setBrush(Qt::NoBrush);
        p.drawLine(QPointF(x+w, y), QPointF(x+w, y+h));
        p.drawLine(QPointF(x,   y+h), QPointF(x+w, y+h));

        p.setFont(f);
        p.setPen(fg);
        QFontMetrics fm(f);
        QString elided = fm.elidedText(txt, Qt::ElideRight, int(w - 2*padX));
        p.drawText(QRectF(x+padX, y, w-2*padX, h), align | Qt::AlignVCenter, elided);
    };

    // Draw full column-header row
    auto drawColHeaders = [&](qreal y) {
        qreal x = mL;
        // top border of header strip
        p.setPen(QPen(cBorder, 0.5));
        p.drawLine(QPointF(mL, y), QPointF(mL+CW, y));
        // left border
        p.drawLine(QPointF(mL, y), QPointF(mL, y+hdrH));
        for (const Col &c : cols) {
            drawCell(x, y, c.w, hdrH, c.label, fHdr, Qt::AlignCenter, cGreenMid, cWhite);
            x += c.w;
        }
    };

    // ── Statistics ────────────────────────────────────────────────────────────
    int nbAdmin=0, nbChauf=0, nbCamion=0, nbRecyc=0, nbZone=0;
    for (const UserRow &u : std::as_const(filteredUsers)) {
        if      (u.role == "Administrateur")        nbAdmin++;
        else if (u.role == "Chauffeur")             nbChauf++;
        else if (u.role == "Responsable Camion")    nbCamion++;
        else if (u.role == "Responsable Recyclage") nbRecyc++;
        else                                         nbZone++;
    }

    // ── Draw page header (banner + optional stats bar + col headers) ──────────
    int pageNum = 1;
    auto drawPageHeader = [&](bool firstPage) -> qreal
    {
        qreal y = mT;

        // — Green gradient banner —
        const qreal bannerH = 36;
        QLinearGradient grad(mL, y, mL+CW, y);
        grad.setColorAt(0.0,  cGreenDark);
        grad.setColorAt(0.5,  cGreenMid);
        grad.setColorAt(1.0,  cGreenLight);
        p.setPen(Qt::NoPen);
        p.setBrush(grad);
        p.drawRoundedRect(QRectF(mL, y, CW, bannerH), 4, 4);

        // title (left)
        p.setFont(fTitle);
        p.setPen(cWhite);
        p.drawText(QRectF(mL+10, y, CW*0.6, bannerH),
                   Qt::AlignVCenter | Qt::AlignLeft,
                   "TuniWaste  —  Liste des Utilisateurs");

        // date (right)
        p.setFont(fDate);
        p.setPen(QColor(210, 240, 200));
        p.drawText(QRectF(mL, y, CW-10, bannerH),
                   Qt::AlignVCenter | Qt::AlignRight,
                   "Genere le : " + QDateTime::currentDateTime().toString("dd/MM/yyyy  hh:mm"));
        y += bannerH + 4;

        // — Stats bar (first page only) —
        if (firstPage) {
            const qreal sH = 18;
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(242, 249, 238));
            p.drawRoundedRect(QRectF(mL, y, CW, sH), 3, 3);
            p.setPen(QPen(cBorder, 0.5));
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(QRectF(mL, y, CW, sH), 3, 3);

            QStringList items = {
                QString("Total : %1").arg(filteredUsers.size()),
                QString("Admins : %1").arg(nbAdmin),
                QString("Chauffeurs : %1").arg(nbChauf),
                QString("Camion : %1").arg(nbCamion),
                QString("Recyclage : %1").arg(nbRecyc),
                QString("Zone : %1").arg(nbZone)
            };
            qreal sw = CW / items.size();
            for (int i = 0; i < items.size(); i++) {
                p.setFont(fStat);
                p.setPen(i == 0 ? cGreenLight : cGreenMid);
                p.drawText(QRectF(mL + i*sw, y, sw, sH), Qt::AlignCenter, items[i]);
            }
            y += sH + 4;
        }

        drawColHeaders(y);
        return y + hdrH;
    };

    qreal yPos  = drawPageHeader(true);
    bool  altRow = false;

    // ── Data rows ─────────────────────────────────────────────────────────────
    for (const UserRow &user : std::as_const(filteredUsers)) {

        // New page if needed
        if (yPos + rowH > H - mB) {
            // footer
            p.setFont(fFoot);
            p.setPen(cGray);
            p.drawText(QRectF(mL, H-mB, CW, mB), Qt::AlignBottom | Qt::AlignCenter,
                       QString("TuniWaste (c) %1  -  Systeme de Gestion des Dechets   |   Page %2")
                           .arg(QDate::currentDate().year()).arg(pageNum));
            printer.newPage();
            pageNum++;
            altRow = false;
            yPos   = drawPageHeader(false);
        }

        // left border of row
        p.setPen(QPen(cBorder, 0.5));
        p.drawLine(QPointF(mL, yPos), QPointF(mL, yPos+rowH));

        QColor rowBg = altRow ? cAlt : cWhite;
        qreal x = mL;

        QStringList vals = {
            QString::number(user.id),
            (user.prenom + " " + user.nom).trimmed(),
            user.telephone,
            user.email,
            user.sexe,
            user.role,
            user.ville
        };

        for (int i = 0; i < vals.size(); i++) {
            QColor bg = rowBg, fg = cText;
            if (i == 5) {  // Role column — colour coded
                if      (user.role == "Administrateur")        { bg = QColor(255,243,224); fg = QColor(160,80,0);   }
                else if (user.role == "Chauffeur")             { bg = QColor(224,240,255); fg = QColor(0,80,160);   }
                else if (user.role == "Responsable Camion")    { bg = QColor(240,224,255); fg = QColor(90,0,150);   }
                else if (user.role == "Responsable Recyclage") { bg = QColor(224,245,230); fg = QColor(20,110,50);  }
                else                                            { bg = QColor(255,228,228); fg = QColor(160,20,20); }
            }
            drawCell(x, yPos, cols[i].w, rowH, vals[i], fCell,
                     i == 0 ? Qt::AlignCenter : Qt::AlignLeft, bg, fg);
            x += cols[i].w;
        }

        yPos  += rowH;
        altRow = !altRow;
    }

    // ── Footer last page ──────────────────────────────────────────────────────
    p.setFont(fFoot);
    p.setPen(cGray);
    p.drawText(QRectF(mL, H-mB, CW, mB), Qt::AlignBottom | Qt::AlignCenter,
               QString("TuniWaste (c) %1  -  Systeme de Gestion des Dechets   |   Page %2")
                   .arg(QDate::currentDate().year()).arg(pageNum));

    p.end();

    QMessageBox::information(this, "Succes",
                             "PDF exporte avec succes !\n\nEmplacement : " + fileName);
}


// ══════════════════════════════════════════════════════════════════════════════
//  PasswordResetDialog
// ══════════════════════════════════════════════════════════════════════════════
PasswordResetDialog::PasswordResetDialog(QWidget *parent) : QDialog(parent)
{
    setModal(true);
    setFixedSize(480, 400);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    // ── Card ─────────────────────────────────────────────────────────────────
    QWidget *card = new QWidget();
    card->setObjectName("pwdCard");
    card->setStyleSheet(
        "QWidget#pwdCard {"
        "  background-color: #FFFFFF;"
        "  border-radius: 20px;"
        "  border: 1.5px solid #C8E6C0;"
        "}"
        );
    outerLayout->addWidget(card);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    // ── Drag handle / title bar ───────────────────────────────────────────────
    QWidget *topBar = new QWidget();
    topBar->setFixedHeight(58);
    topBar->setCursor(Qt::OpenHandCursor);
    topBar->setStyleSheet(
        "QWidget {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "    stop:0 #1B4D18, stop:0.4 #2E5D28, stop:0.7 #3B6B35, stop:1 #5C9A4E);"
        "  border-top-left-radius: 20px; border-top-right-radius: 20px;"
        "}"
        );
    QHBoxLayout *topL = new QHBoxLayout(topBar);
    topL->setContentsMargins(18, 0, 14, 0);

    QLabel *lockIcon = new QLabel("🔑");
    lockIcon->setStyleSheet("font-size: 20px; background: transparent;");
    topL->addWidget(lockIcon);

    QWidget *topInfo = new QWidget();
    topInfo->setStyleSheet("background: transparent;");
    QVBoxLayout *tiL = new QVBoxLayout(topInfo);
    tiL->setSpacing(1); tiL->setContentsMargins(0,0,0,0);
    QLabel *tiTitle = new QLabel("Mot de passe oublie ?");
    tiTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #FFFFFF; background: transparent;");
    QLabel *tiSub   = new QLabel("TuniWaste — Recuperation de compte");
    tiSub->setStyleSheet("font-size: 10px; color: #A3D977; background: transparent;");
    tiL->addWidget(tiTitle); tiL->addWidget(tiSub);
    topL->addWidget(topInfo);
    topL->addStretch();

    QPushButton *closeBtn = new QPushButton("x");
    closeBtn->setFixedSize(28, 28);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { background-color: rgba(255,255,255,0.18); color: white;"
        "  font-size: 13px; font-weight: bold; border: none; border-radius: 14px; }"
        "QPushButton:hover { background-color: rgba(255,80,80,0.85); }"
        );
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    topL->addWidget(closeBtn);
    cardLayout->addWidget(topBar);

    // ── Inner content ─────────────────────────────────────────────────────────
    QWidget *inner = new QWidget();
    inner->setStyleSheet("background: transparent;");
    QVBoxLayout *innerL = new QVBoxLayout(inner);
    innerL->setContentsMargins(40, 24, 40, 28);
    innerL->setSpacing(14);

    QLabel *instrLabel = new QLabel(
        "Entrez votre adresse email.\nVotre mot de passe vous sera envoye immediatement."
        );
    instrLabel->setStyleSheet("font-size: 13px; color: #5A7A55; background: transparent;");
    instrLabel->setAlignment(Qt::AlignCenter);
    instrLabel->setWordWrap(true);
    innerL->addWidget(instrLabel);

    QLabel *emailLabel = new QLabel("Adresse email");
    emailLabel->setStyleSheet("font-size: 12px; font-weight: bold; color: #3B6B35; background: transparent;");
    innerL->addWidget(emailLabel);

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("nom@example.com");
    emailEdit->setFixedHeight(48);
    emailEdit->setStyleSheet(
        "QLineEdit { padding: 12px 16px; border: 2px solid #D4EDDA; border-radius: 10px;"
        " background-color: #F8FCF6; color: #2C3E25; font-size: 14px; }"
        "QLineEdit:focus { border: 2px solid #6FA85E; background-color: #FFFFFF; }"
        );
    connect(emailEdit, &QLineEdit::returnPressed, this, &PasswordResetDialog::onResetClicked);
    innerL->addWidget(emailEdit);

    // Inline status label (hidden initially)
    statusLabel = new QLabel();
    statusLabel->setWordWrap(true);
    statusLabel->setFixedHeight(32);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("background: transparent; font-size: 12px;");
    statusLabel->setVisible(false);
    innerL->addWidget(statusLabel);

    // Buttons row
    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(10);

    QPushButton *cancelBtn = new QPushButton("Annuler");
    cancelBtn->setFixedHeight(44);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(
        "QPushButton { background-color: #F2F2F2; color: #555; padding: 10px 20px;"
        " border: 2px solid #DDD; border-radius: 10px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background-color: #E8E8E8; }"
        );
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton *resetBtn = new QPushButton("Envoyer le mot de passe");
    resetBtn->setFixedHeight(44);
    resetBtn->setCursor(Qt::PointingHandCursor);
    resetBtn->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
        "    stop:0 #6FA85E, stop:1 #4A8F42);"
        "  color: #FFFFFF; padding: 10px 20px; border: none;"
        "  border-radius: 10px; font-weight: bold; font-size: 13px;"
        "}"
        "QPushButton:hover { background-color: #5A9048; }"
        "QPushButton:pressed { background-color: #3B6B35; }"
        );
    connect(resetBtn, &QPushButton::clicked, this, &PasswordResetDialog::onResetClicked);

    btnRow->addWidget(cancelBtn, 1);
    btnRow->addWidget(resetBtn, 2);
    innerL->addLayout(btnRow);
    cardLayout->addWidget(inner);

    // Fade-in animation
    QGraphicsOpacityEffect *fx = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(fx);
    QPropertyAnimation *anim = new QPropertyAnimation(fx, "opacity", this);
    anim->setDuration(200);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    emailEdit->setFocus();
}

// ── PasswordResetDialog — drag support ───────────────────────────────────────
void PasswordResetDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->pos().y() <= 58) {
        m_dragging   = true;
        m_dragOffset = event->globalPosition().toPoint() - frameGeometry().topLeft();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QDialog::mousePressEvent(event);
}

void PasswordResetDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragOffset);
        event->accept();
        return;
    }
    QDialog::mouseMoveEvent(event);
}

void PasswordResetDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_dragging) {
        m_dragging = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QDialog::mouseReleaseEvent(event);
}

// ── showStatus() — inline feedback instead of QMessageBox ────────────────────
void PasswordResetDialog::showStatus(const QString &msg, bool success)
{
    statusLabel->setText(msg);
    if (success)
        statusLabel->setStyleSheet(
            "font-size: 12px; border-radius: 6px; padding: 4px 8px;"
            " background-color: #E8F5E3; color: #2E7D32; border: 1px solid #A5D6A7;"
            );
    else
        statusLabel->setStyleSheet(
            "font-size: 12px; border-radius: 6px; padding: 4px 8px;"
            " background-color: #FFEBEE; color: #C62828; border: 1px solid #EF9A9A;"
            );
    statusLabel->setVisible(true);
}

void PasswordResetDialog::onResetClicked()
{
    QString email = emailEdit->text().trimmed();

    if (email.isEmpty()) {
        showStatus("Veuillez entrer votre adresse email.", false);
        emailEdit->setFocus();
        return;
    }

    QRegularExpression emailRegex(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)");
    if (!emailRegex.match(email).hasMatch()) {
        showStatus("Email invalide. Exemple : nom@example.com", false);
        emailEdit->setFocus();
        return;
    }

    QSqlDatabase db = Connection::instance()->getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT MOT_DE_PASSE FROM UTILISATEUR WHERE EMAIL = :email");
    query.bindValue(":email", email);

    if (!query.exec() || !query.next()) {
        showStatus("Aucun compte associe a cet email.", false);
        emailEdit->setFocus();
        return;
    }

    QString motDePasse = query.value(0).toString();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool sent = OtpManager::getInstance().sendPasswordEmail(email, motDePasse);
    QApplication::restoreOverrideCursor();

    if (sent) {
        showStatus("Mot de passe envoye ! Verifiez votre boite mail.", true);
        QTimer::singleShot(2500, this, &QDialog::accept);
    } else {
        showStatus("Envoi echoue. Verifiez votre connexion internet.", false);
    }
}
