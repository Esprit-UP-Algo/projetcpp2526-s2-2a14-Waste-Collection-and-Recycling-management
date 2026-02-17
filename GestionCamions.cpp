#include "GestionCamions.h"
#include <QDate>
#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPainter>
#include <QPdfWriter>
#include <QStyle>
#include <QUrl>

GestionCamions::GestionCamions(QWidget *parent)
    : QWidget(parent), nextId(4), currentEditingRow(-1) {
  setupUI();
  loadTruckData();
  applyStyles();
}

GestionCamions::~GestionCamions() {}

void GestionCamions::setupUI() {
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  createFormPanel();
  createMainContent();

  mainLayout->addWidget(formPanel);
  mainLayout->addWidget(mainContent);
}

void GestionCamions::createFormPanel() {
  formPanel = new QWidget();
  formPanel->setFixedWidth(400);
  formPanel->setObjectName("formPanel");

  QVBoxLayout *formLayout = new QVBoxLayout(formPanel);
  formLayout->setContentsMargins(25, 30, 25, 30);
  formLayout->setSpacing(20);

  formTitleLabel = new QLabel("📝 Ajouter un camion");
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

  QLabel *typeLabel = new QLabel("🚛 Type de camion");
  typeLabel->setObjectName("formLabel");
  typeInput = new QLineEdit();
  typeInput->setObjectName("formInput");
  typeInput->setPlaceholderText("Ex: Compact, Grand, Électrique");
  typeInput->setFixedHeight(40);
  fieldsLayout->addWidget(typeLabel);
  fieldsLayout->addWidget(typeInput);
  fieldsLayout->addSpacing(12);

  QLabel *capacityLabel = new QLabel("📦 Capacité (T)");
  capacityLabel->setObjectName("formLabel");
  capacityInput = new QSpinBox();
  capacityInput->setObjectName("formInput");
  capacityInput->setSuffix(" T");
  capacityInput->setMinimum(1);
  capacityInput->setMaximum(50);
  capacityInput->setValue(5);
  capacityInput->setFixedHeight(40);
  fieldsLayout->addWidget(capacityLabel);
  fieldsLayout->addWidget(capacityInput);
  fieldsLayout->addSpacing(12);

  QLabel *statusLabel = new QLabel("⚙️ Statut");
  statusLabel->setObjectName("formLabel");
  statusCombo = new QComboBox();
  statusCombo->setObjectName("formInput");
  statusCombo->addItem("Actif");
  statusCombo->addItem("Maintenance");
  statusCombo->addItem("En panne");
  statusCombo->setFixedHeight(40);
  fieldsLayout->addWidget(statusLabel);
  fieldsLayout->addWidget(statusCombo);
  fieldsLayout->addSpacing(12);

  QLabel *locationLabel = new QLabel("📍 Localisation");
  locationLabel->setObjectName("formLabel");
  locationInput = new QLineEdit();
  locationInput->setObjectName("formInput");
  locationInput->setPlaceholderText("Ex: Tunis, Ariana, Sfax");
  locationInput->setFixedHeight(40);
  fieldsLayout->addWidget(locationLabel);
  fieldsLayout->addWidget(locationInput);

  formLayout->addLayout(fieldsLayout);
  formLayout->addSpacing(20);

  saveButton = new QPushButton("💾 Enregistrer");
  saveButton->setObjectName("saveButton");
  saveButton->setFixedHeight(42);
  saveButton->setCursor(Qt::PointingHandCursor);
  connect(saveButton, &QPushButton::clicked, this, &GestionCamions::onAddTruck);

  formLayout->addWidget(saveButton);
  formLayout->addStretch();

  QLabel *infoBox = new QLabel(
      "💡 <b>Astuce:</b><br>"
      "Cliquez sur 'Modifier' dans le tableau pour éditer un camion existant.");
  infoBox->setObjectName("infoBox");
  infoBox->setWordWrap(true);
  formLayout->addWidget(infoBox);
}

void GestionCamions::createMainContent() {
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
  QLabel *breadcrumb = new QLabel("Tableau de bord / Gestion des camions");
  breadcrumb->setObjectName("breadcrumb");
  headerLayout->addWidget(breadcrumb);
  contentLayout->addWidget(headerWidget);

  QWidget *contentWrapper = new QWidget();
  contentWrapper->setObjectName("contentWrapper");

  QVBoxLayout *wrapperLayout = new QVBoxLayout(contentWrapper);
  wrapperLayout->setContentsMargins(20, 20, 20, 20);
  wrapperLayout->setSpacing(15);

  QHBoxLayout *titleLayout = new QHBoxLayout();
  QLabel *pageTitle = new QLabel("Liste des camions");
  pageTitle->setObjectName("pageTitle");
  titleLayout->addWidget(pageTitle);
  titleLayout->addStretch();
  wrapperLayout->addLayout(titleLayout);

  QHBoxLayout *filtersLayout = new QHBoxLayout();
  filtersLayout->setSpacing(15);

  searchInput = new QLineEdit();
  searchInput->setObjectName("searchInput");
  searchInput->setPlaceholderText("🔍 Rechercher...");
  searchInput->setFixedHeight(38);
  searchInput->setMinimumWidth(300);
  connect(searchInput, &QLineEdit::textChanged, this,
          &GestionCamions::onSearchTextChanged);

  statusFilter = new QComboBox();
  statusFilter->setObjectName("filterSelect");
  statusFilter->addItem("Tous les statuts");
  statusFilter->addItem("Actif");
  statusFilter->addItem("Maintenance");
  statusFilter->addItem("En panne");
  statusFilter->setFixedHeight(38);
  statusFilter->setMinimumWidth(180);
  connect(statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &GestionCamions::onFilterChanged);

  filtersLayout->addWidget(searchInput);
  filtersLayout->addWidget(statusFilter);
  filtersLayout->addStretch();

  wrapperLayout->addLayout(filtersLayout);

  truckTable = new QTableWidget();
  truckTable->setObjectName("truckTable");
  truckTable->setColumnCount(6);
  truckTable->setHorizontalHeaderLabels({"🆔 ID", "🚛 Type", "📦 Capacité",
                                         "⚙️ Statut", "📍 Localisation",
                                         "⚡ Actions"});
  truckTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  truckTable->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  truckTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
  truckTable->setColumnWidth(5, 250);
  truckTable->verticalHeader()->setVisible(false);
  truckTable->verticalHeader()->setDefaultSectionSize(60);
  truckTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  truckTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  truckTable->setShowGrid(false);
  truckTable->setAlternatingRowColors(true);

  truckTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  truckTable->setMinimumHeight(400);

  wrapperLayout->addWidget(truckTable, 1);
  contentLayout->addWidget(contentWrapper);

  // Export PDF Button
  QHBoxLayout *exportLayout = new QHBoxLayout();
  exportLayout->addStretch();
  exportPdfButton = new QPushButton("📄 Exporter PDF");
  exportPdfButton->setObjectName("exportPdfButton");
  exportPdfButton->setFixedHeight(36);
  exportPdfButton->setCursor(Qt::PointingHandCursor);
  connect(exportPdfButton, &QPushButton::clicked, this,
          &GestionCamions::onExportPDF);
  exportLayout->addWidget(exportPdfButton);
  contentLayout->addLayout(exportLayout);

  // Chart Widget
  createChartWidget();
  contentLayout->addWidget(chartWidget);
}

void GestionCamions::applyStyles() {
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
        #truckTable { background: white; border: none; gridline-color: #F0F0F0; color: #2C3E50; font-size: 12px; }
        #truckTable::item { padding: 12px 10px; border-bottom: 1px solid #F0F0F0; }
        #truckTable::item:selected { background: #F8FFF9; color: #2C3E50; }
        #truckTable::item:alternate { background: #FAFAFA; }
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

void GestionCamions::loadTruckData() {
  addTableRow(1, "Compact", "5 T", "Actif", "Tunis");
  addTableRow(2, "Grand", "15 T", "Maintenance", "Sousse");
  addTableRow(3, "Électrique", "8 T", "En panne", "Sfax");
  truckTable->resizeColumnsToContents();
  updateChartData();
}

void GestionCamions::createChartWidget() {
  chartWidget = new QWidget();
  chartWidget->setObjectName("chartWidget");
  chartWidget->setMinimumHeight(350);

  QVBoxLayout *chartLayout = new QVBoxLayout(chartWidget);
  chartLayout->setContentsMargins(25, 25, 25, 25);
  chartLayout->setSpacing(15);

  QLabel *chartTitle = new QLabel("📊 Statistiques des Camions");
  chartTitle->setObjectName("chartTitle");
  QFont titleFont = chartTitle->font();
  titleFont.setPointSize(18);
  titleFont.setBold(true);
  chartTitle->setFont(titleFont);
  chartLayout->addWidget(chartTitle);

  QHBoxLayout *chartsContainer = new QHBoxLayout();
  chartsContainer->setSpacing(20);

  // Status Chart
  QWidget *statusChartWidget = new QWidget();
  statusChartWidget->setObjectName("miniChart");
  statusChartWidget->setProperty("chartType", "status");
  QVBoxLayout *statusChartLayout = new QVBoxLayout(statusChartWidget);
  statusChartLayout->setContentsMargins(20, 20, 20, 20);
  QLabel *statusChartTitle = new QLabel("État du Parc");
  statusChartTitle->setObjectName("miniChartTitle");
  statusChartLayout->addWidget(statusChartTitle);
  statusChartLayout->addStretch();

  // Total Capacity widget
  QWidget *totalWidget = new QWidget();
  totalWidget->setObjectName("miniChart");
  QVBoxLayout *totalLayout = new QVBoxLayout(totalWidget);
  totalLayout->setContentsMargins(20, 20, 20, 20);
  QLabel *totalTitle = new QLabel("Capacité Totale");
  totalTitle->setObjectName("miniChartTitle");
  totalLayout->addWidget(totalTitle);

  QLabel *totalValue = new QLabel("0 T");
  totalValue->setObjectName("statValue");
  QFont statFont = totalValue->font();
  statFont.setPointSize(36);
  statFont.setBold(true);
  totalValue->setFont(statFont);
  totalValue->setAlignment(Qt::AlignCenter);
  totalLayout->addWidget(totalValue);

  QLabel *totalDesc = new QLabel("Capacité de transport");
  totalDesc->setObjectName("statLabel");
  totalDesc->setAlignment(Qt::AlignCenter);
  totalLayout->addWidget(totalDesc);
  totalLayout->addStretch();

  chartsContainer->addWidget(statusChartWidget);
  chartsContainer->addWidget(totalWidget);

  chartLayout->addLayout(chartsContainer);
}

void GestionCamions::updateChartData() {
  int actif = 0;
  int maintenance = 0;
  int panne = 0;
  int totalCap = 0;

  for (int row = 0; row < truckTable->rowCount(); ++row) {
    if (truckTable->isRowHidden(row))
      continue;

    QWidget *statusWidget = truckTable->cellWidget(row, 3);
    QLabel *statusLabel =
        statusWidget ? statusWidget->findChild<QLabel *>() : nullptr;
    if (statusLabel) {
      if (statusLabel->text() == "Actif")
        actif++;
      else if (statusLabel->text() == "Maintenance")
        maintenance++;
      else if (statusLabel->text() == "En panne")
        panne++;
    }

    QString capStr = truckTable->item(row, 2)->text();
    totalCap += capStr.remove(" T").toInt();
  }

  // Update Total
  QLabel *totalLabel = chartWidget->findChild<QLabel *>("statValue");
  if (totalLabel) {
    totalLabel->setText(QString::number(totalCap) + " T");
  }

  // Update Status Chart
  QList<QWidget *> charts = chartWidget->findChildren<QWidget *>("miniChart");
  for (QWidget *chart : charts) {
    if (chart->property("chartType").toString() == "status") {
      QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(chart->layout());
      if (layout) {
        while (layout->count() > 2) {
          QLayoutItem *item = layout->takeAt(1);
          delete item->widget();
          delete item;
        }

        int total = actif + maintenance + panne;
        addStatBar(layout, "Actif", actif, total, "#4CAF50");
        addStatBar(layout, "Maintenance", maintenance, total, "#FF9800");
        addStatBar(layout, "En panne", panne, total, "#F44336");

        layout->addStretch();
      }
    }
  }
}

void GestionCamions::addStatBar(QVBoxLayout *layout, const QString &label,
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

void GestionCamions::addTableRow(int id, const QString &type,
                                 const QString &capacity, const QString &status,
                                 const QString &location) {
  int row = truckTable->rowCount();
  truckTable->insertRow(row);

  truckTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
  truckTable->setItem(row, 1, new QTableWidgetItem(type));
  truckTable->setItem(row, 2, new QTableWidgetItem(capacity));

  QWidget *statusWidget = new QWidget();
  statusWidget->setStyleSheet("background: transparent;");
  QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);
  statusLayout->setContentsMargins(5, 5, 5, 5);
  QLabel *statusLabel = new QLabel(status);
  statusLabel->setAlignment(Qt::AlignCenter);
  statusLabel->setStyleSheet(getStatusStyle(status) +
                             " min-width: 100px; padding: 5px;");
  statusLabel->setFixedHeight(26);
  statusLayout->addStretch();
  statusLayout->addWidget(statusLabel);
  statusLayout->addStretch();
  truckTable->setCellWidget(row, 3, statusWidget);

  truckTable->setItem(row, 4, new QTableWidgetItem(location));

  QWidget *actionsWidget = new QWidget();
  actionsWidget->setStyleSheet("background: transparent;");
  QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
  actionsLayout->setContentsMargins(5, 0, 5, 0);
  actionsLayout->setSpacing(10);

  QPushButton *editBtn = new QPushButton("Modifier");
  editBtn->setFixedWidth(100);
  editBtn->setFixedHeight(36);
  editBtn->setCursor(Qt::PointingHandCursor);
  editBtn->setStyleSheet("background: #5CB85C; color: white; border-radius: "
                         "8px; font-weight: bold; font-size: 13px;");
  connect(editBtn, &QPushButton::clicked, this, [this, editBtn]() {
    QPoint pos = editBtn->parentWidget()->mapTo(truckTable, QPoint(0, 0));
    int r = truckTable->indexAt(pos).row();
    if (r >= 0)
      onModifyTruck(r);
  });

  QPushButton *deleteBtn = new QPushButton("Supprimer");
  deleteBtn->setFixedWidth(100);
  deleteBtn->setFixedHeight(36);
  deleteBtn->setCursor(Qt::PointingHandCursor);
  deleteBtn->setStyleSheet("background: #D9534F; color: white; border-radius: "
                           "8px; font-weight: bold; font-size: 13px;");
  connect(deleteBtn, &QPushButton::clicked, this, [this, deleteBtn]() {
    QPoint pos = deleteBtn->parentWidget()->mapTo(truckTable, QPoint(0, 0));
    int r = truckTable->indexAt(pos).row();
    if (r >= 0)
      onDeleteTruck(r);
  });

  actionsLayout->addStretch();
  actionsLayout->addWidget(editBtn);
  actionsLayout->addWidget(deleteBtn);
  actionsLayout->addStretch();
  truckTable->setCellWidget(row, 5, actionsWidget);

  truckTable->setRowHeight(row, 80);
}

QString GestionCamions::getStatusStyle(const QString &status) {
  if (status == "Actif")
    return "QLabel { background-color: #4CAF50; color: white; border-radius: "
           "8px; font-weight: bold; font-size: 10px; }";
  if (status == "Maintenance")
    return "QLabel { background-color: #FF9800; color: white; border-radius: "
           "8px; font-weight: bold; font-size: 10px; }";
  return "QLabel { background-color: #F44336; color: white; border-radius: "
         "8px; font-weight: bold; font-size: 10px; }";
}

void GestionCamions::onAddTruck() {
  QString type = typeInput->text();
  QString location = locationInput->text();
  if (type.isEmpty() || location.isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez remplir les champs obligatoires");
    return;
  }

  QString status = statusCombo->currentText();
  QString capacity = QString::number(capacityInput->value()) + " T";

  if (currentEditingRow >= 0) {
    truckTable->item(currentEditingRow, 1)->setText(type);
    truckTable->item(currentEditingRow, 2)->setText(capacity);
    truckTable->item(currentEditingRow, 4)->setText(location);

    QWidget *statusWidget = truckTable->cellWidget(currentEditingRow, 3);
    QLabel *statusLabel =
        statusWidget ? statusWidget->findChild<QLabel *>() : nullptr;
    if (statusLabel) {
      statusLabel->setText(status);
      statusLabel->setStyleSheet(getStatusStyle(status) +
                                 " min-width: 100px; padding: 5px;");
    }

    QMessageBox::information(this, "Succès", "Camion mis à jour avec succès!");
    clearFormInputs();
    truckTable->resizeColumnsToContents();
    updateChartData();
  } else {
    addTableRow(nextId++, type, capacity, status, location);
    QMessageBox::information(this, "Succès", "Camion ajouté avec succès!");
    clearFormInputs();
    // truckTable->searchText(searchInput->text()); // Re-apply search if needed
    truckTable->resizeColumnsToContents();
    updateChartData();
  }
}

void GestionCamions::onModifyTruck(int row) { setFormForEditing(row); }

void GestionCamions::setFormForEditing(int row) {
  if (row < 0 || row >= truckTable->rowCount())
    return;

  currentEditingRow = row;
  formTitleLabel->setText("✏️ Modifier camion #" +
                          truckTable->item(row, 0)->text());
  typeInput->setText(truckTable->item(row, 1)->text());
  locationInput->setText(truckTable->item(row, 4)->text());

  QString capStr = truckTable->item(row, 2)->text();
  capacityInput->setValue(capStr.remove(" T").toInt());

  QWidget *statusWidget = truckTable->cellWidget(row, 3);
  QLabel *statusLbl =
      statusWidget ? statusWidget->findChild<QLabel *>() : nullptr;
  if (statusLbl)
    statusCombo->setCurrentText(statusLbl->text());

  saveButton->setText("💾 Mettre à jour");
}

void GestionCamions::onDeleteTruck(int row) {
  if (row < 0 || row >= truckTable->rowCount())
    return;

  QMessageBox::StandardButton reply = QMessageBox::question(
      this, "Confirmer", "Voulez-vous vraiment supprimer ce camion ?",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    if (currentEditingRow == row)
      clearFormInputs();
    truckTable->removeRow(row);
    updateChartData();
    QMessageBox::information(this, "Succès", "Supprimé avec succès!");
  }
}

void GestionCamions::onSearchTextChanged(const QString &text) {
  for (int i = 0; i < truckTable->rowCount(); ++i) {
    bool match = false;
    for (int j = 0; j < 5; ++j) { // Check first 5 columns
      QTableWidgetItem *item = truckTable->item(i, j);
      if (item && item->text().contains(text, Qt::CaseInsensitive)) {
        match = true;
        break;
      }
    }
    // Also check status widget which is not a QTableWidgetItem
    if (!match) {
      QWidget *statusWidget = truckTable->cellWidget(i, 3);
      QLabel *statusLabel =
          statusWidget ? statusWidget->findChild<QLabel *>() : nullptr;
      if (statusLabel &&
          statusLabel->text().contains(text, Qt::CaseInsensitive))
        match = true;
    }

    truckTable->setRowHidden(i, !match);
  }
}

void GestionCamions::onFilterChanged(int index) {
  QString selected = statusFilter->currentText();
  if (selected == "Tous les statuts") {
    onSearchTextChanged(searchInput->text());
    return;
  }

  for (int i = 0; i < truckTable->rowCount(); ++i) {
    if (truckTable->isRowHidden(i))
      continue; // Keep search result context

    QWidget *statusWidget = truckTable->cellWidget(i, 3);
    QLabel *statusLabel =
        statusWidget ? statusWidget->findChild<QLabel *>() : nullptr;
    if (statusLabel && statusLabel->text() != selected) {
      truckTable->setRowHidden(i, true);
    }
  }
}

void GestionCamions::clearFormInputs() {
  typeInput->clear();
  locationInput->clear();
  capacityInput->setValue(5);
  statusCombo->setCurrentIndex(0);
  currentEditingRow = -1;
  formTitleLabel->setText("📝 Ajouter un camion");
  saveButton->setText("💾 Enregistrer");
}

void GestionCamions::onExportPDF() {
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
                   "TuniWaste - Rapport Camions");

  // Content
  painter.setPen(Qt::black);
  painter.setFont(QFont("Segoe UI", 12));

  int y = 400;
  for (int row = 0; row < truckTable->rowCount(); ++row) {
    if (truckTable->isRowHidden(row))
      continue;

    QWidget *statusWidget = truckTable->cellWidget(row, 3);
    QLabel *statusLabel =
        statusWidget ? statusWidget->findChild<QLabel *>() : nullptr;

    QString line = QString("Type: %1 | Cap: %2 | Statut: %3 | Loc: %4")
                       .arg(truckTable->item(row, 1)->text())
                       .arg(truckTable->item(row, 2)->text())
                       .arg(statusLabel ? statusLabel->text() : "")
                       .arg(truckTable->item(row, 4)->text());

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
