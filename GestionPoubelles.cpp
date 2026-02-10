#include "GestionPoubelles.h"
#include <QDate>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QStyle>
#include <QTextStream>
#include <QUrl>


GestionPoubelles::GestionPoubelles(QWidget *parent)
    : QWidget(parent), nextId(4), currentEditingRow(-1) {
  setupUI();
  loadPoubelleData();
  applyStyles();
}

GestionPoubelles::~GestionPoubelles() {}

void GestionPoubelles::setupUI() {
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  createFormPanel();
  createMainContent();

  mainLayout->addWidget(formPanel);
  mainLayout->addWidget(mainContent);
}

void GestionPoubelles::createFormPanel() {
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
  fieldsLayout->setSpacing(4);
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
  fieldsLayout->addSpacing(12);

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
  fieldsLayout->addSpacing(12);

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
  fieldsLayout->addSpacing(12);

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
  fieldsLayout->addSpacing(12);

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
  fieldsLayout->addSpacing(12);

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
  connect(saveButton, &QPushButton::clicked, this,
          &GestionPoubelles::onAddPoubelle);

  formLayout->addWidget(saveButton);
  formLayout->addStretch();

  // Info Box
  QLabel *infoBox = new QLabel("💡 <b>Astuce:</b><br>"
                               "Cliquez sur 'Modifier' dans le tableau pour "
                               "éditer une poubelle existante.");
  infoBox->setObjectName("infoBox");
  infoBox->setWordWrap(true);
  formLayout->addWidget(infoBox);
}

void GestionPoubelles::createMainContent() {
  mainContent = new QWidget();
  mainContent->setObjectName("mainContent");

  QVBoxLayout *contentLayout = new QVBoxLayout(mainContent);
  contentLayout->setContentsMargins(20, 20, 20, 20);
  contentLayout->setSpacing(15);

  // Header (Breadcrumb part specifically for this module)
  QWidget *headerWidget = new QWidget();
  headerWidget->setObjectName("header");
  headerWidget->setFixedHeight(50);
  QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
  headerLayout->setContentsMargins(20, 10, 20, 10);
  QLabel *breadcrumb = new QLabel("Tableau de bord / Gestion des poubelles");
  breadcrumb->setObjectName("breadcrumb");
  headerLayout->addWidget(breadcrumb);
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
  connect(searchInput, &QLineEdit::textChanged, this,
          &GestionPoubelles::onSearchTextChanged);

  etatFilter = new QComboBox();
  etatFilter->setObjectName("filterSelect");
  etatFilter->addItem("Tous les états");
  etatFilter->addItem("Opérationnelle");
  etatFilter->addItem("Maintenance");
  etatFilter->addItem("Hors service");
  etatFilter->addItem("Pleine");
  etatFilter->setFixedHeight(38);
  etatFilter->setMinimumWidth(180);
  connect(etatFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &GestionPoubelles::onFilterChanged);

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
  poubelleTable->setHorizontalHeaderLabels(
      {"🆔 ID", "📍 Localisation", "🗂️ Type", "📦 Capacité", "📊 Niveau",
       "⚙️ État", "🏷️ ID Zone", "⚡ Actions"});

  poubelleTable->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Interactive);
  poubelleTable->horizontalHeader()->setSectionResizeMode(
      4, QHeaderView::ResizeToContents);
  poubelleTable->horizontalHeader()->setSectionResizeMode(
      5, QHeaderView::ResizeToContents);
  poubelleTable->horizontalHeader()->setSectionResizeMode(
      7, QHeaderView::ResizeToContents);

  poubelleTable->setColumnWidth(0, 60);
  poubelleTable->setColumnWidth(1, 300);
  poubelleTable->setColumnWidth(2, 100);
  poubelleTable->setColumnWidth(3, 90);
  poubelleTable->setColumnWidth(4, 100);
  poubelleTable->setColumnWidth(5, 150);
  poubelleTable->setColumnWidth(6, 100);
  poubelleTable->setColumnWidth(7, 220);

  poubelleTable->verticalHeader()->setDefaultSectionSize(60);
  poubelleTable->verticalHeader()->setVisible(false);
  poubelleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  poubelleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  poubelleTable->setShowGrid(false);
  poubelleTable->setAlternatingRowColors(true);

  poubelleTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  poubelleTable->setMinimumHeight(300);
  poubelleTable->setMaximumHeight(500);
  poubelleTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  poubelleTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  wrapperLayout->addWidget(poubelleTable, 1);

  contentLayout->addWidget(contentWrapper);

  // Export PDF Button
  QHBoxLayout *exportLayout = new QHBoxLayout();
  exportLayout->addStretch();
  exportPdfButton = new QPushButton("📄 Exporter PDF");
  exportPdfButton->setObjectName("exportPdfButton");
  exportPdfButton->setFixedHeight(36);
  exportPdfButton->setCursor(Qt::PointingHandCursor);
  connect(exportPdfButton, &QPushButton::clicked, this,
          &GestionPoubelles::onExportPDF);
  exportLayout->addWidget(exportPdfButton);
  contentLayout->addLayout(exportLayout);

  // Chart Widget
  createChartWidget();
  contentLayout->addWidget(chartWidget);
}

void GestionPoubelles::loadPoubelleData() {
  addTableRow(1, "Avenue Habib Bourguiba, Tunis Centre", 85, "Pleine",
              "ZONE-001", 240, "Plastique");
  addTableRow(2, "Rue de Marseille, Ariana", 45, "Opérationnelle", "ZONE-002",
              120, "Verre");
  addTableRow(3, "Boulevard 7 Novembre, Sfax", 20, "Maintenance", "ZONE-003",
              180, "Papier");
  poubelleTable->resizeColumnsToContents();
  updateChartData();
}

void GestionPoubelles::createChartWidget() {
  chartWidget = new QWidget();
  chartWidget->setObjectName("chartWidget");
  chartWidget->setMinimumHeight(350);

  QVBoxLayout *chartLayout = new QVBoxLayout(chartWidget);
  chartLayout->setContentsMargins(25, 25, 25, 25);
  chartLayout->setSpacing(15);

  QLabel *chartTitle = new QLabel("📊 Statistiques des Poubelles");
  chartTitle->setObjectName("chartTitle");
  QFont titleFont = chartTitle->font();
  titleFont.setPointSize(18);
  titleFont.setBold(true);
  chartTitle->setFont(titleFont);
  chartLayout->addWidget(chartTitle);

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

  // Niveau Moyen Chart
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

void GestionPoubelles::updateChartData() {
  int operationnelleCount = 0;
  int maintenanceCount = 0;
  int horsServiceCount = 0;
  int pleineCount = 0;
  QMap<QString, int> typeCount;
  int totalNiveau = 0;

  for (int row = 0; row < poubelleTable->rowCount(); ++row) {
    QWidget *etatWidget = poubelleTable->cellWidget(row, 5);
    QLabel *etatLabel =
        etatWidget ? etatWidget->findChild<QLabel *>() : nullptr;
    if (etatLabel) {
      QString etat = etatLabel->text();
      if (etat == "Opérationnelle")
        operationnelleCount++;
      else if (etat == "Maintenance")
        maintenanceCount++;
      else if (etat == "Hors service")
        horsServiceCount++;
      else if (etat == "Pleine")
        pleineCount++;
    }

    QString type = poubelleTable->item(row, 2)->text();
    typeCount[type]++;

    QWidget *niveauWidget = poubelleTable->cellWidget(row, 4);
    QLabel *niveauLabel =
        niveauWidget ? niveauWidget->findChild<QLabel *>() : nullptr;
    if (niveauLabel) {
      QString niveauStr = niveauLabel->text();
      totalNiveau += niveauStr.remove("%").trimmed().toInt();
    }
  }

  int avgNiveau = poubelleTable->rowCount() > 0
                      ? totalNiveau / poubelleTable->rowCount()
                      : 0;

  QList<QWidget *> charts = chartWidget->findChildren<QWidget *>("miniChart");
  for (QWidget *chart : charts) {
    if (chart->property("chartType").toString() == "etat") {
      QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(chart->layout());
      if (layout) {
        while (layout->count() > 2) {
          QLayoutItem *item = layout->takeAt(1);
          delete item->widget();
          delete item;
        }
        addStatBar(layout, "Opérationnelle", operationnelleCount,
                   poubelleTable->rowCount(), "#4CAF50");
        addStatBar(layout, "Pleine", pleineCount, poubelleTable->rowCount(),
                   "#F5A623");
        addStatBar(layout, "Maintenance", maintenanceCount,
                   poubelleTable->rowCount(), "#2196F3");
        addStatBar(layout, "Hors service", horsServiceCount,
                   poubelleTable->rowCount(), "#F44336");
        layout->addStretch();
      }
    } else if (chart->property("chartType").toString() == "type") {
      QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(chart->layout());
      if (layout) {
        while (layout->count() > 2) {
          QLayoutItem *item = layout->takeAt(1);
          delete item->widget();
          delete item;
        }
        QStringList colors = {"#9C27B0", "#00BCD4", "#FF9800",
                              "#8BC34A", "#607D8B", "#E91E63"};
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

  QLabel *avgNiveauLabel = chartWidget->findChild<QLabel *>("statValue");
  if (avgNiveauLabel) {
    avgNiveauLabel->setText(QString::number(avgNiveau) + "%");
    QString color = "#4CAF50";
    if (avgNiveau >= 80)
      color = "#F44336";
    else if (avgNiveau >= 60)
      color = "#F5A623";
    avgNiveauLabel->setStyleSheet("color: " + color + ";");
  }
}

void GestionPoubelles::addStatBar(QVBoxLayout *layout, const QString &label,
                                  int count, int total, const QString &color) {
  QWidget *barContainer = new QWidget();
  QVBoxLayout *barLayout = new QVBoxLayout(barContainer);
  barLayout->setContentsMargins(0, 5, 0, 5);
  barLayout->setSpacing(5);

  QHBoxLayout *labelLayout = new QHBoxLayout();
  QLabel *nameLabel = new QLabel(label);
  nameLabel->setObjectName("barLabel");
  QLabel *countLabel = new QLabel(QString::number(count));
  countLabel->setObjectName("barCount");
  labelLayout->addWidget(nameLabel);
  labelLayout->addStretch();
  labelLayout->addWidget(countLabel);
  barLayout->addLayout(labelLayout);

  QWidget *barBackground = new QWidget();
  barBackground->setFixedHeight(10);
  barBackground->setStyleSheet("background: #f0f0f0; border-radius: 5px;");
  QWidget *barFill = new QWidget(barBackground);
  int percentage = total > 0 ? (count * 100 / total) : 0;
  barFill->setFixedHeight(10);
  barFill->setFixedWidth(barBackground->width() * percentage / 100);
  barFill->setStyleSheet(
      QString("background: %1; border-radius: 5px;").arg(color));
  barLayout->addWidget(barBackground);

  layout->insertWidget(layout->count() - 1, barContainer);
}

void GestionPoubelles::addTableRow(int id, const QString &localisation,
                                   int niveau, const QString &etat,
                                   const QString &idZone, int capacite,
                                   const QString &type) {
  int row = poubelleTable->rowCount();
  poubelleTable->insertRow(row);

  poubelleTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
  poubelleTable->setItem(row, 1, new QTableWidgetItem(localisation));
  poubelleTable->setItem(row, 2, new QTableWidgetItem(type));
  poubelleTable->setItem(
      row, 3, new QTableWidgetItem(QString::number(capacite) + " L"));
  poubelleTable->setItem(row, 6, new QTableWidgetItem(idZone));

  QWidget *niveauWidget = new QWidget();
  niveauWidget->setStyleSheet("background: transparent;");
  QHBoxLayout *niveauLayout = new QHBoxLayout(niveauWidget);
  niveauLayout->setContentsMargins(5, 5, 5, 5);
  QLabel *niveauBadge = new QLabel(QString::number(niveau) + "%");
  niveauBadge->setAlignment(Qt::AlignCenter);
  niveauBadge->setStyleSheet(getNiveauStyle(niveau) + " min-width: 80px;");
  niveauBadge->setFixedHeight(26);
  niveauLayout->addStretch();
  niveauLayout->addWidget(niveauBadge);
  niveauLayout->addStretch();
  poubelleTable->setCellWidget(row, 4, niveauWidget);

  QWidget *etatWidget = new QWidget();
  etatWidget->setStyleSheet("background: transparent;");
  QHBoxLayout *etatLayout = new QHBoxLayout(etatWidget);
  etatLayout->setContentsMargins(5, 5, 5, 5);
  QLabel *etatBadge = new QLabel(etat);
  etatBadge->setAlignment(Qt::AlignCenter);
  etatBadge->setWordWrap(true);
  etatBadge->setStyleSheet(getEtatStyle(etat) +
                           " min-width: 150px; padding: 8px;");
  etatBadge->setFixedHeight(26);
  etatLayout->addStretch();
  etatLayout->addWidget(etatBadge);
  etatLayout->addStretch();
  poubelleTable->setCellWidget(row, 5, etatWidget);

  QWidget *actionsWidget = new QWidget();
  actionsWidget->setStyleSheet("background: transparent;");
  QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
  actionsLayout->setContentsMargins(5, 0, 5, 0);
  actionsLayout->setSpacing(10);

  QPushButton *modifyBtn = new QPushButton();
  modifyBtn->setFixedSize(40, 40);
  modifyBtn->setCursor(Qt::PointingHandCursor);
  modifyBtn->setToolTip("Modifier cette poubelle");
  modifyBtn->setText("✏️");
  modifyBtn->setStyleSheet("background: #E8F5E9; border-radius: 20px;");
  connect(modifyBtn, &QPushButton::clicked,
          [this, row]() { onModifyPoubelle(row); });

  QPushButton *deleteBtn = new QPushButton();
  deleteBtn->setFixedSize(40, 40);
  deleteBtn->setCursor(Qt::PointingHandCursor);
  deleteBtn->setToolTip("Supprimer cette poubelle");
  deleteBtn->setText("🗑️");
  deleteBtn->setStyleSheet("background: #FFEBEE; border-radius: 20px;");
  connect(deleteBtn, &QPushButton::clicked,
          [this, row]() { onDeletePoubelle(row); });

  actionsLayout->addStretch();
  actionsLayout->addWidget(modifyBtn);
  actionsLayout->addWidget(deleteBtn);
  actionsLayout->addStretch();
  poubelleTable->setCellWidget(row, 7, actionsWidget);

  poubelleTable->setRowHeight(row, 60);
}

QString GestionPoubelles::getEtatStyle(const QString &etat) {
  QString style = "QLabel { background-color: %1; color: white; border-radius: "
                  "8px; font-weight: bold; font-size: 10px; }";
  if (etat == "Opérationnelle")
    return style.arg("#4CAF50");
  else if (etat == "Maintenance")
    return style.arg("#2196F3");
  else if (etat == "Hors service")
    return style.arg("#F44336");
  else if (etat == "Pleine")
    return style.arg("#FF9800");
  return "";
}

QString GestionPoubelles::getNiveauStyle(int niveau) {
  QString bgColor;
  if (niveau >= 80)
    bgColor = "#F44336";
  else if (niveau >= 60)
    bgColor = "#FF9800";
  else if (niveau >= 40)
    bgColor = "#2196F3";
  else
    bgColor = "#4CAF50";

  return QString("QLabel { background-color: %1; color: white; border-radius: "
                 "8px; font-weight: bold; font-size: 10px; }")
      .arg(bgColor);
}

void GestionPoubelles::clearFormInputs() {
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

void GestionPoubelles::setFormForEditing(int row) {
  if (row < 0 || row >= poubelleTable->rowCount())
    return;

  currentEditingRow = row;
  QString id = poubelleTable->item(row, 0)->text();

  localisationInput->setText(poubelleTable->item(row, 1)->text());
  typeCombo->setCurrentText(poubelleTable->item(row, 2)->text());
  QString capaciteStr = poubelleTable->item(row, 3)->text();
  capaciteInput->setValue(capaciteStr.remove(" L").toInt());

  QWidget *niveauWidget = poubelleTable->cellWidget(row, 4);
  QLabel *niveauLabel =
      niveauWidget ? niveauWidget->findChild<QLabel *>() : nullptr;
  if (niveauLabel)
    niveauInput->setValue(niveauLabel->text().remove("%").trimmed().toInt());

  QWidget *etatWidget = poubelleTable->cellWidget(row, 5);
  QLabel *etatLabel = etatWidget ? etatWidget->findChild<QLabel *>() : nullptr;
  if (etatLabel)
    etatCombo->setCurrentText(etatLabel->text());

  idZoneInput->setText(poubelleTable->item(row, 6)->text());

  formTitleLabel->setText("✏️ Modifier la poubelle #" + id);
  saveButton->setText("💾 Mettre à jour");
}

void GestionPoubelles::applyStyles() {
  QString styleSheet = R"(
        #formPanel { background: white; border-right: 1px solid #E5E5E5; }
        #formTitle { color: #2C3E50; padding-bottom: 5px; }
        #formLabel { color: #2C3E50; font-weight: 700; font-size: 12px; margin-bottom: 8px; letter-spacing: 0.3px; }
        #formInput { border: 2px solid #E0E0E0; border-radius: 12px; padding: 10px 14px; font-size: 13px; background: #FAFAFA; color: #2C3E50; font-weight: 500; }
        #formInput:hover { border-color: #9BCB4E; background: white; }
        #formInput:focus { border-color: #9BCB4E; background: white; outline: none; }
        QSpinBox::up-button, QSpinBox::down-button { width: 20px; border-radius: 4px; }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover { background: #9BCB4E; }
        QComboBox::drop-down { border: none; width: 30px; }
        QComboBox::down-arrow { image: none; border-left: 5px solid transparent; border-right: 5px solid transparent; border-top: 6px solid #666; margin-right: 10px; }
        #saveButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9BCB4E, stop:1 #8AB83E); color: white; border: none; border-radius: 12px; font-size: 14px; font-weight: bold; letter-spacing: 0.5px; }
        #saveButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B7D97A, stop:1 #9BCB4E); }
        #saveButton:pressed { background: #8AB83E; }
        #infoBox { background: #E8F4F8; color: #1565C0; padding: 16px; border-radius: 12px; border-left: 5px solid #2196F3; font-size: 13px; line-height: 1.6; }
        #mainContent { background: #F7F7F7; }
        #header { background: white; border-radius: 14px; box-shadow: 0 2px 8px rgba(0,0,0,0.04); }
        #breadcrumb { color: #7F8C8D; font-size: 14px; font-weight: 500; }
        #contentWrapper { background: white; border-radius: 16px; box-shadow: 0 2px 12px rgba(0,0,0,0.06); }
        #pageTitle { font-size: 22px; font-weight: 800; color: #2C3E50; letter-spacing: -0.5px; }
        #searchInput { border: 2px solid #E0E0E0; border-radius: 12px; padding: 0 16px; font-size: 14px; color: #2C3E50; background: #FAFAFA; }
        #searchInput:focus { border-color: #9BCB4E; background: white; }
        #filterSelect { border: 2px solid #E0E0E0; border-radius: 12px; padding: 0 16px; font-size: 14px; background: #FAFAFA; color: #2C3E50; font-weight: 500; }
        #filterSelect:hover { border-color: #9BCB4E; background: white; }
        #poubelleTable { background: white; border: none; gridline-color: #F0F0F0; color: #2C3E50; font-size: 12px; }
        #poubelleTable::item { padding: 12px 10px; border-bottom: 1px solid #F0F0F0; }
        #poubelleTable::item:selected { background: #F8FFF9; color: #2C3E50; }
        #poubelleTable::item:alternate { background: #FAFAFA; }
        QHeaderView::section { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #F8F9FA, stop:1 #F0F1F2); padding: 10px 10px; border: none; border-bottom: 2px solid #E0E0E0; border-right: 1px solid #EEEEEE; font-weight: 700; font-size: 11px; color: #2C3E50; text-transform: uppercase; letter-spacing: 0.5px; }
        #pageButton { border: 2px solid #E0E0E0; background: white; border-radius: 10px; font-weight: 600; color: #2C3E50; }
        #pageButton:hover { background: #9BCB4E; color: white; border-color: #9BCB4E; }
        #exportPdfButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #E74C3C, stop:1 #C0392B); color: white; border: none; border-radius: 10px; padding: 0 20px; font-size: 13px; font-weight: bold; }
        #exportPdfButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #EC7063, stop:1 #E74C3C); }
        #chartWidget { background: white; border-radius: 16px; box-shadow: 0 2px 12px rgba(0,0,0,0.06); }
        #chartTitle { color: #2C3E50; font-weight: 800; }
        #miniChart { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FAFAFA, stop:1 #F5F5F5); border-radius: 14px; border: 2px solid #E8E8E8; }
        #miniChartTitle { color: #2C3E50; font-size: 14px; font-weight: 700; margin-bottom: 15px; }
        #statValue { color: #9BCB4E; font-weight: 900; }
        #statLabel { color: #7F8C8D; font-size: 13px; font-weight: 500; }
        #barLabel { color: #2C3E50; font-size: 12px; font-weight: 600; }
        #barCount { color: #2C3E50; font-weight: 700; font-size: 12px; }
    )";
  setStyleSheet(styleSheet);
}

void GestionPoubelles::onAddPoubelle() {
  QString localisation = localisationInput->text();
  QString idZone = idZoneInput->text();
  if (localisation.isEmpty() || idZone.isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez remplir tous les champs obligatoires");
    return;
  }

  QString type = typeCombo->currentText();
  type = type.split(" ").last();
  int capacite = capaciteInput->value();
  int niveau = niveauInput->value();
  QString etat = etatCombo->currentText();
  etat = etat.split(" ").last();

  if (currentEditingRow >= 0) {
    QString id = poubelleTable->item(currentEditingRow, 0)->text();
    poubelleTable->item(currentEditingRow, 1)->setText(localisation);
    poubelleTable->item(currentEditingRow, 2)->setText(type);
    poubelleTable->item(currentEditingRow, 3)
        ->setText(QString::number(capacite) + " L");
    poubelleTable->item(currentEditingRow, 6)->setText(idZone);

    QWidget *niveauWidget = poubelleTable->cellWidget(currentEditingRow, 4);
    QLabel *niveauLabel =
        niveauWidget ? niveauWidget->findChild<QLabel *>() : nullptr;
    if (niveauLabel) {
      niveauLabel->setText(QString::number(niveau) + "%");
      niveauLabel->setStyleSheet(getNiveauStyle(niveau) + " min-width: 80px;");
    }

    QWidget *etatWidget = poubelleTable->cellWidget(currentEditingRow, 5);
    QLabel *etatLabel =
        etatWidget ? etatWidget->findChild<QLabel *>() : nullptr;
    if (etatLabel) {
      etatLabel->setText(etat);
      etatLabel->setStyleSheet(getEtatStyle(etat) +
                               " min-width: 150px; padding: 8px;");
    }

    QMessageBox::information(this, "Succès",
                             "Poubelle #" + id + " modifiée avec succès!");
    clearFormInputs();
    poubelleTable->resizeColumnsToContents();
    updateChartData();
  } else {
    addTableRow(nextId++, localisation, niveau, etat, idZone, capacite, type);
    poubelleTable->scrollToBottom();
    QMessageBox::information(this, "Succès", "Poubelle ajoutée avec succès!");
    clearFormInputs();
    poubelleTable->resizeColumnsToContents();
    updateChartData();
  }
}

void GestionPoubelles::onModifyPoubelle(int row) { setFormForEditing(row); }

void GestionPoubelles::onDeletePoubelle(int row) {
  if (row < 0 || row >= poubelleTable->rowCount())
    return;
  QString id = poubelleTable->item(row, 0)->text();
  QString localisation = poubelleTable->item(row, 1)->text();

  QMessageBox::StandardButton reply =
      QMessageBox::question(this, "Confirmer la suppression",
                            "Voulez-vous vraiment supprimer la poubelle #" +
                                id + " (" + localisation + ") ?",
                            QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    if (currentEditingRow == row)
      clearFormInputs();
    poubelleTable->removeRow(row);
    poubelleTable->resizeColumnsToContents();
    updateChartData();
    QMessageBox::information(this, "Succès", "Poubelle supprimée avec succès!");
  }
}

void GestionPoubelles::onSearchTextChanged(const QString &text) {
  for (int row = 0; row < poubelleTable->rowCount(); ++row) {
    bool match = false;
    for (int col = 0; col < 7; ++col) {
      QTableWidgetItem *item = poubelleTable->item(row, col);
      if (item && item->text().contains(text, Qt::CaseInsensitive)) {
        match = true;
        break;
      }
    }
    if (!match) {
      QWidget *etatWidget = poubelleTable->cellWidget(row, 5);
      if (etatWidget) {
        QLabel *lbl = etatWidget->findChild<QLabel *>();
        if (lbl && lbl->text().contains(text, Qt::CaseInsensitive))
          match = true;
      }
    }
    poubelleTable->setRowHidden(row, !match);
  }
}

void GestionPoubelles::onFilterChanged(int index) {
  QString filter = etatFilter->itemText(index);
  for (int row = 0; row < poubelleTable->rowCount(); ++row) {
    bool match = (index == 0);
    if (!match) {
      QWidget *etatWidget = poubelleTable->cellWidget(row, 5);
      if (etatWidget) {
        QLabel *lbl = etatWidget->findChild<QLabel *>();
        if (lbl && lbl->text() == filter)
          match = true;
      }
    }
    poubelleTable->setRowHidden(row, !match);
  }
}

void GestionPoubelles::onExportPDF() {
  QString fileName = QFileDialog::getSaveFileName(
      this, "Exporter le Rapport",
      QDir::homePath() + "/TuniWaste_Poubelles_" +
          QDate::currentDate().toString("yyyy-MM-dd") + ".html",
      "Fichiers HTML (*.html);;Tous les fichiers (*)");
  if (fileName.isEmpty())
    return;
  if (!fileName.endsWith(".html", Qt::CaseInsensitive))
    fileName += ".html";

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

  out << "<!DOCTYPE html>\n<html>\n<head>\n<meta "
         "charset='UTF-8'>\n<title>TuniWaste</title>\n</"
         "head>\n<body>\n<h1>Rapport</h1>\n</body>\n</html>";
  file.close();

  QMessageBox::information(this, "Succès", "Fichier généré: " + fileName);
  QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}
