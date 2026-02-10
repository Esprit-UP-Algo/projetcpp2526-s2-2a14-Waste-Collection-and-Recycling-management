#include "Dashboard.h"
#include <QDateTime>
#include <QScrollArea>


Dashboard::Dashboard(QWidget *parent) : QWidget(parent) {
  setupUI();
  applyStyles();
}

void Dashboard::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(30, 30, 30, 30);
  mainLayout->setSpacing(20);

  // Header
  QLabel *headerLabel = new QLabel("🏠 Tableau de Bord");
  headerLabel->setObjectName("dashboardHeader");
  headerLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: "
                             "#2C3E50; margin-bottom: 10px;");
  mainLayout->addWidget(headerLabel);

  // Welcome message
  QLabel *welcomeLabel =
      new QLabel("Bienvenue dans TuniWaste - Système de Gestion des Déchets");
  welcomeLabel->setStyleSheet(
      "font-size: 14px; color: #7F8C8D; margin-bottom: 20px;");
  mainLayout->addWidget(welcomeLabel);

  // Stats Cards
  createStatsCards();
  mainLayout->addWidget(statsContainer);

  // Two column layout for activity and actions
  QHBoxLayout *contentLayout = new QHBoxLayout();
  contentLayout->setSpacing(20);

  // Recent Activity
  createRecentActivity();
  contentLayout->addWidget(activityContainer, 2);

  // Quick Actions
  createQuickActions();
  contentLayout->addWidget(actionsContainer, 1);

  mainLayout->addLayout(contentLayout);
  mainLayout->addStretch();
}

void Dashboard::createStatsCards() {
  statsContainer = new QWidget();
  QHBoxLayout *statsLayout = new QHBoxLayout(statsContainer);
  statsLayout->setSpacing(20);
  statsLayout->setContentsMargins(0, 0, 0, 0);

  // Create stat cards
  QWidget *usersCard = createStatCard("Utilisateurs", "4", "👥", "#3498DB");
  QWidget *binsCard = createStatCard("Poubelles", "3", "🗑️", "#2ECC71");
  QWidget *trucksCard = createStatCard("Camions", "2", "🚛", "#E74C3C");
  QWidget *zonesCard = createStatCard("Zones", "5", "📍", "#F39C12");

  statsLayout->addWidget(usersCard);
  statsLayout->addWidget(binsCard);
  statsLayout->addWidget(trucksCard);
  statsLayout->addWidget(zonesCard);
}

QWidget *Dashboard::createStatCard(const QString &title, const QString &value,
                                   const QString &icon, const QString &color) {
  QFrame *card = new QFrame();
  card->setObjectName("statCard");
  card->setStyleSheet(QString("#statCard {"
                              "   background: white;"
                              "   border-radius: 12px;"
                              "   border-left: 5px solid %1;"
                              "   padding: 20px;"
                              "}"
                              "#statCard:hover {"
                              "   box-shadow: 0 4px 12px rgba(0,0,0,0.1);"
                              "}")
                          .arg(color));

  QVBoxLayout *cardLayout = new QVBoxLayout(card);
  cardLayout->setSpacing(10);

  // Icon and value row
  QHBoxLayout *topRow = new QHBoxLayout();

  QLabel *iconLabel = new QLabel(icon);
  iconLabel->setStyleSheet("font-size: 32px;");

  QLabel *valueLabel = new QLabel(value);
  valueLabel->setStyleSheet(
      QString("font-size: 36px; font-weight: bold; color: %1;").arg(color));

  topRow->addWidget(iconLabel);
  topRow->addStretch();
  topRow->addWidget(valueLabel);

  cardLayout->addLayout(topRow);

  // Title
  QLabel *titleLabel = new QLabel(title);
  titleLabel->setStyleSheet(
      "font-size: 14px; color: #7F8C8D; font-weight: 500;");
  cardLayout->addWidget(titleLabel);

  return card;
}

void Dashboard::createRecentActivity() {
  activityContainer = new QFrame();
  activityContainer->setObjectName("activityPanel");
  activityContainer->setStyleSheet("#activityPanel {"
                                   "   background: white;"
                                   "   border-radius: 12px;"
                                   "   padding: 20px;"
                                   "}");

  QVBoxLayout *activityLayout = new QVBoxLayout(activityContainer);
  activityLayout->setSpacing(15);

  // Header
  QLabel *activityHeader = new QLabel("📊 Activité Récente");
  activityHeader->setStyleSheet("font-size: 18px; font-weight: bold; color: "
                                "#2C3E50; margin-bottom: 10px;");
  activityLayout->addWidget(activityHeader);

  // Activity items
  QStringList activities = {"✅ Nouvelle poubelle ajoutée - Zone Centre-Ville",
                            "🚛 Camion TN-1234 assigné à la collecte",
                            "👤 Nouvel utilisateur enregistré",
                            "♻️ 15 kg de plastique recyclé",
                            "📍 Zone Industrielle mise à jour"};

  for (const QString &activity : activities) {
    QFrame *activityItem = new QFrame();
    activityItem->setStyleSheet("background: #F8F9FA;"
                                "border-radius: 8px;"
                                "padding: 12px;"
                                "border-left: 3px solid #6FA85E;");

    QVBoxLayout *itemLayout = new QVBoxLayout(activityItem);
    itemLayout->setContentsMargins(10, 8, 10, 8);
    itemLayout->setSpacing(5);

    QLabel *activityText = new QLabel(activity);
    activityText->setStyleSheet("font-size: 13px; color: #2C3E50;");

    QLabel *timeLabel = new QLabel("Il y a 2 heures");
    timeLabel->setStyleSheet("font-size: 11px; color: #95A5A6;");

    itemLayout->addWidget(activityText);
    itemLayout->addWidget(timeLabel);

    activityLayout->addWidget(activityItem);
  }

  activityLayout->addStretch();
}

void Dashboard::createQuickActions() {
  actionsContainer = new QFrame();
  actionsContainer->setObjectName("actionsPanel");
  actionsContainer->setStyleSheet("#actionsPanel {"
                                  "   background: white;"
                                  "   border-radius: 12px;"
                                  "   padding: 20px;"
                                  "}");

  QVBoxLayout *actionsLayout = new QVBoxLayout(actionsContainer);
  actionsLayout->setSpacing(15);

  // Header
  QLabel *actionsHeader = new QLabel("⚡ Actions Rapides");
  actionsHeader->setStyleSheet("font-size: 18px; font-weight: bold; color: "
                               "#2C3E50; margin-bottom: 10px;");
  actionsLayout->addWidget(actionsHeader);

  // Action buttons
  QString buttonStyle = "QPushButton {"
                        "   background: #6FA85E;"
                        "   color: white;"
                        "   border: none;"
                        "   border-radius: 8px;"
                        "   padding: 12px;"
                        "   font-size: 14px;"
                        "   font-weight: 500;"
                        "   text-align: left;"
                        "}"
                        "QPushButton:hover {"
                        "   background: #5D8F4D;"
                        "}";

  QPushButton *addBinBtn = new QPushButton("🗑️  Ajouter une Poubelle");
  addBinBtn->setStyleSheet(buttonStyle);

  QPushButton *addTruckBtn = new QPushButton("🚛  Ajouter un Camion");
  addTruckBtn->setStyleSheet(buttonStyle);

  QPushButton *scheduleBtn = new QPushButton("📅  Planifier une Collecte");
  scheduleBtn->setStyleSheet(buttonStyle);

  QPushButton *reportBtn = new QPushButton("📈  Générer un Rapport");
  reportBtn->setStyleSheet(
      buttonStyle.replace("#6FA85E", "#3498DB").replace("#5D8F4D", "#2980B9"));

  actionsLayout->addWidget(addBinBtn);
  actionsLayout->addWidget(addTruckBtn);
  actionsLayout->addWidget(scheduleBtn);
  actionsLayout->addWidget(reportBtn);
  actionsLayout->addStretch();

  // System status
  QFrame *statusFrame = new QFrame();
  statusFrame->setStyleSheet("background: #E8F5E9;"
                             "border-radius: 8px;"
                             "padding: 15px;"
                             "border: 1px solid #C8E6C9;");

  QVBoxLayout *statusLayout = new QVBoxLayout(statusFrame);
  statusLayout->setSpacing(5);

  QLabel *statusIcon = new QLabel("✅");
  statusIcon->setStyleSheet("font-size: 24px;");
  statusIcon->setAlignment(Qt::AlignCenter);

  QLabel *statusText = new QLabel("Système Opérationnel");
  statusText->setStyleSheet(
      "font-size: 14px; font-weight: bold; color: #2E7D32;");
  statusText->setAlignment(Qt::AlignCenter);

  QLabel *statusDetail =
      new QLabel("Tous les services fonctionnent normalement");
  statusDetail->setStyleSheet("font-size: 11px; color: #558B2F;");
  statusDetail->setAlignment(Qt::AlignCenter);
  statusDetail->setWordWrap(true);

  statusLayout->addWidget(statusIcon);
  statusLayout->addWidget(statusText);
  statusLayout->addWidget(statusDetail);

  actionsLayout->addWidget(statusFrame);
}

void Dashboard::applyStyles() {
  setStyleSheet("QWidget {"
                "   background: #F5F6FA;"
                "   font-family: 'Segoe UI', Arial, sans-serif;"
                "}");
}
