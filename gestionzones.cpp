#include "gestionzones.h"

#include <QCoreApplication>
#include <QDate>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPainter>
#include <QPdfWriter>
#include <QStyle>
#include <QUrl>

GestionZones::GestionZones(QWidget *parent)
    : QWidget(parent), currentEditingRow(-1) {
  setupUI();
  loadZoneData();
  applyStyles();
}

GestionZones::~GestionZones() {}

void GestionZones::setupUI() {
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  createFormPanel();
  createMainContent();

  mainLayout->addWidget(formPanel);
  mainLayout->addWidget(mainContent);
}

void GestionZones::createFormPanel() {
  formPanel = new QWidget();
  formPanel->setFixedWidth(400);
  formPanel->setObjectName("formPanel");

  QVBoxLayout *formLayout = new QVBoxLayout(formPanel);
  formLayout->setContentsMargins(25, 30, 25, 30);
  formLayout->setSpacing(20);

  formTitleLabel = new QLabel("📝 Ajouter une zone");
  formTitleLabel->setObjectName("formTitle");
  QFont titleFont = formTitleLabel->font();
  titleFont.setPointSize(16);
  titleFont.setBold(true);
  formTitleLabel->setFont(titleFont);
  formLayout->addWidget(formTitleLabel);

  QFrame *separator = new QFrame();
  separator->setFrameShape(QFrame::HLine);
  separator->setStyleSheet("background: #ddd; max-height: 1px;");
  formLayout->addWidget(separator);

  formLayout->addSpacing(10);

  QVBoxLayout *fieldsLayout = new QVBoxLayout();
  fieldsLayout->setSpacing(4);
  fieldsLayout->setContentsMargins(0, 0, 0, 0);

  // ID
  QLabel *lblId = new QLabel("🆔 ID Zone");
  lblId->setObjectName("formLabel");
  idEdit = new QLineEdit();
  idEdit->setObjectName("formInput");
  idEdit->setPlaceholderText("Ex: ZONE-001");
  idEdit->setFixedHeight(40);
  fieldsLayout->addWidget(lblId);
  fieldsLayout->addWidget(idEdit);
  fieldsLayout->addSpacing(12);

  // Nom
  QLabel *lblNom = new QLabel("📛 Nom");
  lblNom->setObjectName("formLabel");
  nomEdit = new QLineEdit();
  nomEdit->setObjectName("formInput");
  nomEdit->setPlaceholderText("Nom de la zone");
  nomEdit->setFixedHeight(40);
  fieldsLayout->addWidget(lblNom);
  fieldsLayout->addWidget(nomEdit);
  fieldsLayout->addSpacing(12);

  // Localisation
  QLabel *lblLoc = new QLabel("📍 Localisation");
  lblLoc->setObjectName("formLabel");
  localisationEdit = new QLineEdit();
  localisationEdit->setObjectName("formInput");
  localisationEdit->setPlaceholderText("Localisation géo.");
  localisationEdit->setFixedHeight(40);
  fieldsLayout->addWidget(lblLoc);
  fieldsLayout->addWidget(localisationEdit);
  fieldsLayout->addSpacing(12);

  // Population
  QLabel *lblPop = new QLabel("👥 Population");
  lblPop->setObjectName("formLabel");
  populationEdit = new QLineEdit();
  populationEdit->setObjectName("formInput");
  populationEdit->setPlaceholderText("Estimation habitants");
  populationEdit->setFixedHeight(40);
  fieldsLayout->addWidget(lblPop);
  fieldsLayout->addWidget(populationEdit);
  fieldsLayout->addSpacing(12);

  // Surface
  QLabel *lblSurf = new QLabel("📐 Surface (km²)");
  lblSurf->setObjectName("formLabel");
  surfaceEdit = new QLineEdit();
  surfaceEdit->setObjectName("formInput");
  surfaceEdit->setPlaceholderText("Surface en km²");
  surfaceEdit->setFixedHeight(40);
  fieldsLayout->addWidget(lblSurf);
  fieldsLayout->addWidget(surfaceEdit);
  fieldsLayout->addSpacing(12);

  // Fréquence
  QLabel *lblFreq = new QLabel("⏰ Fréquence de collecte");
  lblFreq->setObjectName("formLabel");
  frequenceEdit = new QComboBox();
  frequenceEdit->setObjectName("formInput");
  frequenceEdit->addItem("Quotidienne");
  frequenceEdit->addItem("Hebdomadaire");
  frequenceEdit->addItem("Mensuelle");
  frequenceEdit->setFixedHeight(40);
  fieldsLayout->addWidget(lblFreq);
  fieldsLayout->addWidget(frequenceEdit);

  formLayout->addLayout(fieldsLayout);
  formLayout->addSpacing(20);

  saveButton = new QPushButton("💾 Enregistrer");
  saveButton->setObjectName("saveButton");
  saveButton->setFixedHeight(42);
  saveButton->setCursor(Qt::PointingHandCursor);
  connect(saveButton, &QPushButton::clicked, this, &GestionZones::onAddZone);

  formLayout->addWidget(saveButton);
  formLayout->addStretch();

  QLabel *infoBox = new QLabel(
      "💡 <b>Astuce:</b><br>"
      "Cliquez sur 'Modifier' dans le tableau pour éditer une zone existante.");
  infoBox->setObjectName("infoBox");
  infoBox->setWordWrap(true);
  formLayout->addWidget(infoBox);
}

void GestionZones::createMainContent() {
  mainContent = new QWidget();
  mainContent->setObjectName("mainContent");

  QVBoxLayout *contentLayout = new QVBoxLayout(mainContent);
  contentLayout->setContentsMargins(20, 20, 20, 20);
  contentLayout->setSpacing(15);

  QWidget *headerWidget = new QWidget();
  headerWidget->setObjectName("header");
  headerWidget->setFixedHeight(50);
  QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
  headerLayout->setContentsMargins(20, 10, 20, 10);
  QLabel *breadcrumb = new QLabel("Tableau de bord / Gestion des zones");
  breadcrumb->setObjectName("breadcrumb");
  headerLayout->addWidget(breadcrumb);
  contentLayout->addWidget(headerWidget);

  QWidget *contentWrapper = new QWidget();
  contentWrapper->setObjectName("contentWrapper");

  QVBoxLayout *wrapperLayout = new QVBoxLayout(contentWrapper);
  wrapperLayout->setContentsMargins(20, 20, 20, 20);
  wrapperLayout->setSpacing(15);

  QHBoxLayout *titleLayout = new QHBoxLayout();
  QLabel *pageTitle = new QLabel("Liste des zones");
  pageTitle->setObjectName("pageTitle");
  titleLayout->addWidget(pageTitle);
  titleLayout->addStretch();
  wrapperLayout->addLayout(titleLayout);

  QHBoxLayout *filtersLayout = new QHBoxLayout();
  filtersLayout->setSpacing(15);

  searchNom = new QLineEdit();
  searchNom->setObjectName("searchInput");
  searchNom->setPlaceholderText("🔍 Rechercher par ID ou Nom...");
  searchNom->setFixedHeight(38);
  searchNom->setMinimumWidth(300);
  connect(searchNom, &QLineEdit::textChanged, this,
          &GestionZones::onSearchTextChanged);

  filtersLayout->addWidget(searchNom);
  filtersLayout->addStretch();

  wrapperLayout->addLayout(filtersLayout);

  tableZones = new QTableWidget();
  tableZones->setObjectName("tableZones");
  tableZones->setColumnCount(7);
  tableZones->setHorizontalHeaderLabels({"🆔 ID", "📛 Nom", "📍 Localisation",
                                         "👥 Pop.", "📐 Surf.", "⏰ Fréq.",
                                         "⚡ Actions"});
  tableZones->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  tableZones->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  tableZones->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
  tableZones->setColumnWidth(6, 250);
  tableZones->verticalHeader()->setVisible(false);
  tableZones->verticalHeader()->setDefaultSectionSize(60);
  tableZones->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableZones->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableZones->setShowGrid(false);
  tableZones->setAlternatingRowColors(true);

  tableZones->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  tableZones->setMinimumHeight(400);

  wrapperLayout->addWidget(tableZones, 1);
  contentLayout->addWidget(contentWrapper);

  // Export PDF Button
  QHBoxLayout *exportLayout = new QHBoxLayout();
  exportLayout->addStretch();
  btnExportPDF = new QPushButton("📄 Exporter PDF");
  btnExportPDF->setObjectName("exportPdfButton");
  btnExportPDF->setFixedHeight(36);
  btnExportPDF->setCursor(Qt::PointingHandCursor);
  connect(btnExportPDF, &QPushButton::clicked, this,
          &GestionZones::onExportPDF);
  exportLayout->addWidget(btnExportPDF);
  contentLayout->addLayout(exportLayout);

  createChartWidget();
  contentLayout->addWidget(chartWidget);
}

void GestionZones::applyStyles() {
  // Reuse style sheet for consistency
  QString styleSheet = R"(
        #formPanel { background: white; border-right: 1px solid #E5E5E5; }
        #formTitle { color: #2C3E50; padding-bottom: 5px; }
        #formLabel { color: #2C3E50; font-weight: 700; font-size: 12px; margin-bottom: 8px; letter-spacing: 0.3px; }
        #formInput { border: 2px solid #E0E0E0; border-radius: 12px; padding: 10px 14px; font-size: 13px; background: #FAFAFA; color: #2C3E50; font-weight: 500; }
        #formInput:hover { border-color: #9BCB4E; background: white; }
        #formInput:focus { border-color: #9BCB4E; background: white; outline: none; }
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
        #tableZones { background: white; border: none; gridline-color: #F0F0F0; color: #2C3E50; font-size: 12px; }
        #tableZones::item { padding: 12px 10px; border-bottom: 1px solid #F0F0F0; }
        #tableZones::item:selected { background: #F8FFF9; color: #2C3E50; }
        #tableZones::item:alternate { background: #FAFAFA; }
        QHeaderView::section { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #F8F9FA, stop:1 #F0F1F2); padding: 10px 10px; border: none; border-bottom: 2px solid #E0E0E0; border-right: 1px solid #EEEEEE; font-weight: 700; font-size: 11px; color: #2C3E50; text-transform: uppercase; letter-spacing: 0.5px; }
        #exportPdfButton { background: #FF9800; color: white; border: none; border-radius: 10px; padding: 0 20px; font-size: 13px; font-weight: bold; }
        #exportPdfButton:hover { background: #F57C00; }
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

void GestionZones::loadZoneData() {
  addTableRow("ZONE-001", "Centre Ville", "Tunis", "25000", "5.2",
              "Quotidienne");
  addTableRow("ZONE-002", "Jardin Carthage", "Carthage", "12000", "3.5",
              "Hebdomadaire");
  addTableRow("ZONE-003", "Marsa Plage", "La Marsa", "18000", "4.0",
              "Quotidienne");
  updateChartData();
}

void GestionZones::createChartWidget() {
  chartWidget = new QWidget();
  chartWidget->setObjectName("chartWidget");
  chartWidget->setMinimumHeight(350);

  QVBoxLayout *chartLayout = new QVBoxLayout(chartWidget);
  chartLayout->setContentsMargins(25, 25, 25, 25);
  chartLayout->setSpacing(15);

  QLabel *chartTitle = new QLabel("📊 Statistiques des Zones");
  chartTitle->setObjectName("chartTitle");
  QFont titleFont = chartTitle->font();
  titleFont.setPointSize(18);
  titleFont.setBold(true);
  chartTitle->setFont(titleFont);
  chartLayout->addWidget(chartTitle);

  QHBoxLayout *chartsContainer = new QHBoxLayout();
  chartsContainer->setSpacing(20);

  // Frequency Chart
  QWidget *freqChartWidget = new QWidget();
  freqChartWidget->setObjectName("miniChart");
  freqChartWidget->setProperty("chartType", "freq");
  QVBoxLayout *freqChartLayout = new QVBoxLayout(freqChartWidget);
  freqChartLayout->setContentsMargins(20, 20, 20, 20);
  QLabel *freqChartTitle = new QLabel("Fréquence Collecte");
  freqChartTitle->setObjectName("miniChartTitle");
  freqChartLayout->addWidget(freqChartTitle);
  freqChartLayout->addStretch();

  // Total Population Widget
  QWidget *popWidget = new QWidget();
  popWidget->setObjectName("miniChart");
  QVBoxLayout *popLayout = new QVBoxLayout(popWidget);
  popLayout->setContentsMargins(20, 20, 20, 20);
  QLabel *popTitle = new QLabel("Population Totale");
  popTitle->setObjectName("miniChartTitle");
  popLayout->addWidget(popTitle);

  QLabel *popValue = new QLabel("0");
  popValue->setObjectName("statValue");
  QFont statFont = popValue->font();
  statFont.setPointSize(36);
  statFont.setBold(true);
  popValue->setFont(statFont);
  popValue->setAlignment(Qt::AlignCenter);
  popLayout->addWidget(popValue);

  QLabel *popDesc = new QLabel("Habitants couverts");
  popDesc->setObjectName("statLabel");
  popDesc->setAlignment(Qt::AlignCenter);
  popLayout->addWidget(popDesc);
  popLayout->addStretch();

  chartsContainer->addWidget(freqChartWidget);
  chartsContainer->addWidget(popWidget);

  chartLayout->addLayout(chartsContainer);
}

void GestionZones::updateChartData() {
  int quotidien = 0;
  int hebdo = 0;
  int mensuel = 0;
  int totalPop = 0;

  for (int row = 0; row < tableZones->rowCount(); ++row) {
    if (tableZones->isRowHidden(row))
      continue;

    QString freq = tableZones->item(row, 5)->text();
    if (freq == "Quotidienne")
      quotidien++;
    else if (freq == "Hebdomadaire")
      hebdo++;
    else if (freq == "Mensuelle")
      mensuel++;

    QString popStr = tableZones->item(row, 3)->text();
    totalPop += popStr.toInt();
  }

  // Update Pop
  QLabel *popLabel = chartWidget->findChild<QLabel *>("statValue");
  if (popLabel) {
    popLabel->setText(QString::number(totalPop));
  }

  // Update Bars
  QList<QWidget *> charts = chartWidget->findChildren<QWidget *>("miniChart");
  for (QWidget *chart : charts) {
    if (chart->property("chartType").toString() == "freq") {
      QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(chart->layout());
      if (layout) {
        while (layout->count() > 2) {
          QLayoutItem *item = layout->takeAt(1);
          delete item->widget();
          delete item;
        }

        int total = quotidien + hebdo + mensuel;
        addStatBar(layout, "Quotidienne", quotidien, total, "#4CAF50");
        addStatBar(layout, "Hebdomadaire", hebdo, total, "#2196F3");
        addStatBar(layout, "Mensuelle", mensuel, total, "#FF9800");

        layout->addStretch();
      }
    }
  }
}

void GestionZones::addStatBar(QVBoxLayout *layout, const QString &label,
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

  int percentage = total > 0 ? (count * 100 / total) : 0;

  QWidget *barFill = new QWidget(barBackground);
  barFill->setFixedHeight(10);
  barFill->setFixedWidth(200 * percentage / 100);
  barFill->setStyleSheet(
      QString("background: %1; border-radius: 5px;").arg(color));

  barLayout->addWidget(barBackground);
  layout->insertWidget(layout->count() - 1, barContainer);
}

void GestionZones::addTableRow(const QString &id, const QString &nom,
                               const QString &loc, const QString &pop,
                               const QString &surf, const QString &freq) {
  int row = tableZones->rowCount();
  tableZones->insertRow(row);

  tableZones->setItem(row, 0, new QTableWidgetItem(id));
  tableZones->setItem(row, 1, new QTableWidgetItem(nom));
  tableZones->setItem(row, 2, new QTableWidgetItem(loc));
  tableZones->setItem(row, 3, new QTableWidgetItem(pop));
  tableZones->setItem(row, 4, new QTableWidgetItem(surf));
  tableZones->setItem(row, 5, new QTableWidgetItem(freq));

  QWidget *actionsWidget = new QWidget();
  actionsWidget->setStyleSheet("background: transparent;");
  QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
  actionsLayout->setContentsMargins(5, 0, 5, 0);
  actionsLayout->setSpacing(10);

  QPushButton *modifyBtn = new QPushButton("Modifier");
  modifyBtn->setFixedWidth(100);
  modifyBtn->setFixedHeight(36);
  modifyBtn->setCursor(Qt::PointingHandCursor);
  modifyBtn->setToolTip("Modifier");
  modifyBtn->setStyleSheet("background: #5CB85C; color: white; border-radius: "
                           "8px; font-weight: bold; font-size: 13px;");
  connect(modifyBtn, &QPushButton::clicked, this, [this, modifyBtn]() {
    QPoint pos = modifyBtn->parentWidget()->mapTo(tableZones, QPoint(0, 0));
    int r = tableZones->indexAt(pos).row();
    if (r >= 0)
      onModifyZone(r);
  });

  QPushButton *deleteBtn = new QPushButton("Supprimer");
  deleteBtn->setFixedWidth(100);
  deleteBtn->setFixedHeight(36);
  deleteBtn->setCursor(Qt::PointingHandCursor);
  deleteBtn->setToolTip("Supprimer");
  deleteBtn->setStyleSheet("background: #D9534F; color: white; border-radius: "
                           "8px; font-weight: bold; font-size: 13px;");
  connect(deleteBtn, &QPushButton::clicked, this, [this, deleteBtn]() {
    QPoint pos = deleteBtn->parentWidget()->mapTo(tableZones, QPoint(0, 0));
    int r = tableZones->indexAt(pos).row();
    if (r >= 0)
      onDeleteZone(r);
  });

  actionsLayout->addStretch();
  actionsLayout->addWidget(modifyBtn);
  actionsLayout->addWidget(deleteBtn);
  actionsLayout->addStretch();
  tableZones->setCellWidget(row, 6, actionsWidget);

  tableZones->setRowHeight(row, 80);
}

void GestionZones::onAddZone() {
  QString id = idEdit->text();
  QString nom = nomEdit->text();
  QString loc = localisationEdit->text();
  if (id.isEmpty() || nom.isEmpty() || loc.isEmpty()) {
    QMessageBox::warning(
        this, "Erreur",
        "Veuillez remplir les champs obligatoires (ID, Nom, Localisation)");
    return;
  }

  QString pop = populationEdit->text();
  QString surf = surfaceEdit->text();
  QString freq = frequenceEdit->currentText();

  if (currentEditingRow >= 0) {
    tableZones->item(currentEditingRow, 0)->setText(id);
    tableZones->item(currentEditingRow, 1)->setText(nom);
    tableZones->item(currentEditingRow, 2)->setText(loc);
    tableZones->item(currentEditingRow, 3)->setText(pop);
    tableZones->item(currentEditingRow, 4)->setText(surf);
    tableZones->item(currentEditingRow, 5)->setText(freq);

    QMessageBox::information(this, "Succès", "Zone modifiée avec succès!");
    clearFormInputs();
    tableZones->resizeColumnsToContents();
    updateChartData();
  } else {
    addTableRow(id, nom, loc, pop, surf, freq);
    QMessageBox::information(this, "Succès", "Zone ajoutée avec succès!");
    clearFormInputs();
    updateChartData();
  }
}

void GestionZones::onModifyZone(int row) { setFormForEditing(row); }

void GestionZones::onDeleteZone(int row) {
  if (row < 0 || row >= tableZones->rowCount())
    return;

  QMessageBox::StandardButton reply = QMessageBox::question(
      this, "Confirmer", "Voulez-vous vraiment supprimer cette zone ?",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    if (currentEditingRow == row)
      clearFormInputs();
    tableZones->removeRow(row);
    updateChartData();
    QMessageBox::information(this, "Succès", "Supprimé avec succès!");
  }
}

void GestionZones::setFormForEditing(int row) {
  if (row < 0 || row >= tableZones->rowCount())
    return;
  currentEditingRow = row;

  idEdit->setText(tableZones->item(row, 0)->text());
  nomEdit->setText(tableZones->item(row, 1)->text());
  localisationEdit->setText(tableZones->item(row, 2)->text());
  populationEdit->setText(tableZones->item(row, 3)->text());
  surfaceEdit->setText(tableZones->item(row, 4)->text());
  frequenceEdit->setCurrentText(tableZones->item(row, 5)->text());

  formTitleLabel->setText("✏️ Modifier zone #" +
                          tableZones->item(row, 0)->text());
  saveButton->setText("💾 Mettre à jour");
}

void GestionZones::clearFormInputs() {
  idEdit->clear();
  nomEdit->clear();
  localisationEdit->clear();
  populationEdit->clear();
  surfaceEdit->clear();
  frequenceEdit->setCurrentIndex(0);
  currentEditingRow = -1;
  formTitleLabel->setText("📝 Ajouter une zone");
  saveButton->setText("💾 Enregistrer");
}

void GestionZones::onSearchTextChanged(const QString &text) {
  for (int i = 0; i < tableZones->rowCount(); ++i) {
    bool match = false;
    for (int j = 0; j < 6; ++j) {
      QTableWidgetItem *item = tableZones->item(i, j);
      if (item && item->text().contains(text, Qt::CaseInsensitive)) {
        match = true;
        break;
      }
    }
    tableZones->setRowHidden(i, !match);
  }
}

void GestionZones::onExportPDF() {
  QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "",
                                                  "PDF Files (*.pdf)");
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

  // Header
  painter.fillRect(QRect(0, 0, writer.width(), 300), QColor("#6FA85E"));
  painter.setPen(Qt::white);
  painter.setFont(QFont("Segoe UI", 26, QFont::Bold));
  painter.drawText(QRect(50, 0, writer.width() - 100, 300),
                   Qt::AlignVCenter | Qt::AlignLeft,
                   "TuniWaste - Rapport Zones");

  // Content
  painter.setPen(Qt::black);
  painter.setFont(QFont("Segoe UI", 12));

  int y = 400;
  for (int row = 0; row < tableZones->rowCount(); ++row) {
    if (tableZones->isRowHidden(row))
      continue;

    QString line = QString("ID: %1 | Nom: %2 | Pop: %3 | Surf: %4")
                       .arg(tableZones->item(row, 0)->text())
                       .arg(tableZones->item(row, 1)->text())
                       .arg(tableZones->item(row, 3)->text())
                       .arg(tableZones->item(row, 4)->text());

    painter.drawText(50, y, line);
    y += 80;

    if (y > writer.height() - 100) {
      writer.newPage();
      y = 100;
    }
  }

  painter.end();
  QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}
