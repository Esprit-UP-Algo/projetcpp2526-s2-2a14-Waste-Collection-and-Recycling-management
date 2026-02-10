#include "Dashboard.h"
#include <QDateTime>
#include <QScrollArea>

Dashboard::Dashboard(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_StyledBackground); // Crucial for QWidget stylesheets to
                                         // render background
  setupUI();
  applyStyles();
}

void Dashboard::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(35, 30, 35, 30);
  mainLayout->setSpacing(25);

  // Header Section
  QWidget *headerWidget = new QWidget();
  headerWidget->setStyleSheet("background: transparent;");
  QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
  headerLayout->setContentsMargins(0, 0, 0, 0);
  headerLayout->setSpacing(5);

  QLabel *headerLabel = new QLabel("🏠 Tableau de Bord");
  headerLabel->setObjectName("dashboardHeader");
  headerLabel->setStyleSheet(
      "font-size: 26px; font-weight: bold; color: #2D3436;");
  headerLayout->addWidget(headerLabel);

  QLabel *welcomeLabel = new QLabel(
      "Bienvenue sur TuniWaste : Système Intelligent de Gestion des Déchets");
  welcomeLabel->setStyleSheet("font-size: 14px; color: #636E72;");
  headerLayout->addWidget(welcomeLabel);

  mainLayout->addWidget(headerWidget);

  // Stats Cards Container
  statsContainer = new QWidget();
  statsContainer->setStyleSheet("background: transparent;");
  createStatsCards();
  mainLayout->addWidget(statsContainer);

  // Content Area
  QHBoxLayout *contentLayout = new QHBoxLayout();
  contentLayout->setSpacing(25);

  // Left Column: Recent Activity
  createRecentActivity();
  contentLayout->addWidget(activityContainer, 3);

  // Right Column: Quick Actions
  createQuickActions();
  contentLayout->addWidget(actionsContainer, 2);

  mainLayout->addLayout(contentLayout);
  mainLayout->addStretch();
}

void Dashboard::createStatsCards() {
  QHBoxLayout *statsLayout = new QHBoxLayout(statsContainer);
  statsLayout->setSpacing(20);
  statsLayout->setContentsMargins(0, 0, 0, 0);

  // Coordinated color palette
  statsLayout->addWidget(createStatCard("Utilisateurs", "4", "👥",
                                        "#4A90E2")); // Professional Blue
  statsLayout->addWidget(
      createStatCard("Poubelles", "3", "🗑️", "#6FA85E")); // Sidebar Green
  statsLayout->addWidget(
      createStatCard("Camions", "2", "🚛", "#E67E22")); // Energetic Orange
  statsLayout->addWidget(
      createStatCard("Zones", "5", "📍", "#9B59B6")); // Elegant Purple
}

QWidget *Dashboard::createStatCard(const QString &title, const QString &value,
                                   const QString &icon, const QString &color) {
  QFrame *card = new QFrame();
  card->setObjectName("statCard");
  card->setStyleSheet(QString("#statCard {"
                              "   background: white;"
                              "   border-radius: 16px;"
                              "   border-bottom: 4px solid %1;"
                              "   padding: 15px;"
                              "}"
                              "QLabel { background: transparent; }")
                          .arg(color));

  QVBoxLayout *cardLayout = new QVBoxLayout(card);
  cardLayout->setSpacing(5);

  QHBoxLayout *headerRow = new QHBoxLayout();
  QLabel *iconLabel = new QLabel(icon);
  iconLabel->setStyleSheet("font-size: 24px;");
  headerRow->addWidget(iconLabel);
  headerRow->addStretch();
  cardLayout->addLayout(headerRow);

  QLabel *valueLabel = new QLabel(value);
  valueLabel->setStyleSheet(
      QString("font-size: 32px; font-weight: bold; color: #2D3436;"));
  cardLayout->addWidget(valueLabel);

  QLabel *titleLabel = new QLabel(title);
  titleLabel->setStyleSheet("font-size: 12px; color: #636E72; font-weight: "
                            "bold; text-transform: uppercase;");
  cardLayout->addWidget(titleLabel);

  return card;
}

void Dashboard::createRecentActivity() {
  activityContainer = new QFrame();
  activityContainer->setObjectName("cardPanel");
  activityContainer->setStyleSheet("#cardPanel {"
                                   "   background: white;"
                                   "   border-radius: 16px;"
                                   "   padding: 25px;"
                                   "}");

  QVBoxLayout *activityLayout = new QVBoxLayout(activityContainer);
  activityLayout->setSpacing(15);

  QLabel *title = new QLabel("📊 Activité Récente");
  title->setStyleSheet("font-size: 18px; font-weight: bold; color: #2D3436;");
  activityLayout->addWidget(title);

  QStringList activities = {"Nouvelle poubelle connectée - Zone Nord",
                            "Route de collecte optimisée pour Camion TN-1234",
                            "Mise à jour des stocks de recyclage",
                            "Niveau de remplissage critique : Bin #42",
                            "Rapport hebdomadaire exporté avec succès"};

  QStringList times = {"Il y a 10 min", "Il y a 1h", "Il y a 3h", "Il y a 5h",
                       "Hier"};

  for (int i = 0; i < activities.size(); ++i) {
    QFrame *item = new QFrame();
    item->setMinimumHeight(70); // Ensure enough vertical space
    item->setStyleSheet("background: #F8F9FA; border-radius: 12px; "
                        "border-left: 4px solid #6FA85E;");

    QVBoxLayout *itemLayout = new QVBoxLayout(item);
    itemLayout->setContentsMargins(20, 15, 20, 15); // Clear margins
    itemLayout->setSpacing(5);

    QLabel *txt = new QLabel(activities[i]);
    txt->setStyleSheet(
        "font-weight: bold; color: #2D3436; font-size: 11pt;"); // Using pt for
                                                                // scaling
    txt->setWordWrap(true);

    QLabel *time = new QLabel(times[i]);
    time->setStyleSheet("color: #636E72; font-size: 9pt;");

    itemLayout->addWidget(txt);
    itemLayout->addWidget(time);
    activityLayout->addWidget(item);
  }
  activityLayout->addStretch();
}

void Dashboard::createQuickActions() {
  actionsContainer = new QFrame();
  actionsContainer->setObjectName("cardPanel");
  actionsContainer->setStyleSheet("#cardPanel {"
                                  "   background: white;"
                                  "   border-radius: 16px;"
                                  "   padding: 25px;"
                                  "}");

  QVBoxLayout *layout = new QVBoxLayout(actionsContainer);
  layout->setSpacing(15);

  QLabel *title = new QLabel("⚡ Accès Rapide");
  title->setStyleSheet(
      "font-size: 18px; font-weight: 600; color: #2D3436; margin-bottom: 8px;");
  layout->addWidget(title);

  QString btnStyle = "QPushButton {"
                     "   background: #F0F2F5;"
                     "   color: #1A1A1A;"
                     "   border-radius: 10px;"
                     "   padding: 15px;"
                     "   text-align: left;"
                     "   font-weight: 600;"
                     "   font-size: 14px;"
                     "   border: 1px solid #E0E0E0;"
                     "}"
                     "QPushButton:hover {"
                     "   background: #6FA85E;"
                     "   color: white;"
                     "   border: 1px solid #6FA85E;"
                     "}";

  QStringList icons = {"➕", "🚛", "📅", "📊"};
  QStringList labels = {"Nouvelle Poubelle", "Ajouter Camion",
                        "Planifier Collecte", "Rapport Analytique"};

  for (int i = 0; i < labels.size(); ++i) {
    QPushButton *btn = new QPushButton(icons[i] + "  " + labels[i]);
    btn->setStyleSheet(btnStyle);
    layout->addWidget(btn);
  }

  layout->addStretch();

  // Status Frame
  QFrame *status = new QFrame();
  status->setStyleSheet("background: #ECF9F1; border-radius: 12px; padding: "
                        "15px; border: 1px solid #C3E6CB;");
  QHBoxLayout *statusLayout = new QHBoxLayout(status);

  QLabel *statusIcon = new QLabel("✅");
  statusIcon->setStyleSheet("font-size: 20px;");
  statusLayout->addWidget(statusIcon);

  QVBoxLayout *statusTextLayout = new QVBoxLayout();
  QLabel *statusTitle = new QLabel("Système Connecté");
  statusTitle->setStyleSheet(
      "color: #155724; font-weight: 700; font-size: 14px;");
  QLabel *statusSub = new QLabel("Tous les capteurs sont en ligne");
  statusSub->setStyleSheet("color: #155724; font-size: 12px;");
  statusTextLayout->addWidget(statusTitle);
  statusTextLayout->addWidget(statusSub);

  statusLayout->addLayout(statusTextLayout);
  statusLayout->addStretch();

  layout->addWidget(status);
}

void Dashboard::applyStyles() {
  setStyleSheet("QWidget {"
                "   background-color: #F8F9FB;"
                "   font-family: sans-serif;"
                "}");
}
