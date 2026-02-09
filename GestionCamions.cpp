#include "GestionCamions.h"
#include <QDate>
#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStyle>
#include <QTextStream>
#include <QUrl>

GestionCamions::GestionCamions(QWidget *parent)
    : QWidget(parent), nextId(4), currentEditingRow(-1) {
  setupUI();
  loadTruckData();
  applyStyles();
}

GestionCamions::~GestionCamions() {}

// Helper widgets for the layout (extracted from original code)
QWidget *formPanel;
QWidget *mainContent;

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
  titleFont.setPointSize(18);
  titleFont.setBold(true);
  formTitleLabel->setFont(titleFont);
  formLayout->addWidget(formTitleLabel);

  QFrame *separator = new QFrame();
  separator->setFrameShape(QFrame::HLine);
  separator->setStyleSheet("background: #ddd; max-height: 1px;");
  formLayout->addWidget(separator);

  formLayout->addSpacing(10);

  QFormLayout *fieldsLayout = new QFormLayout();
  fieldsLayout->setSpacing(20);
  fieldsLayout->setContentsMargins(0, 0, 0, 0);
  fieldsLayout->setLabelAlignment(Qt::AlignLeft);

  QLabel *typeLabel = new QLabel("Type de camion:");
  typeLabel->setObjectName("formLabel");
  typeInput = new QLineEdit();
  typeInput->setObjectName("formInput");
  typeInput->setPlaceholderText("Ex: Compact, Grand, Électrique");
  typeInput->setFixedHeight(45);
  fieldsLayout->addRow(typeLabel, typeInput);

  QLabel *capacityLabel = new QLabel("Capacité:");
  capacityLabel->setObjectName("formLabel");
  capacityInput = new QSpinBox();
  capacityInput->setObjectName("formInput");
  capacityInput->setSuffix(" T");
  capacityInput->setMinimum(1);
  capacityInput->setMaximum(50);
  capacityInput->setValue(5);
  capacityInput->setFixedHeight(45);
  fieldsLayout->addRow(capacityLabel, capacityInput);

  QLabel *statusLabel = new QLabel("Statut:");
  statusLabel->setObjectName("formLabel");
  statusCombo = new QComboBox();
  statusCombo->setObjectName("formInput");
  statusCombo->addItem("Actif");
  statusCombo->addItem("Maintenance");
  statusCombo->addItem("En panne");
  statusCombo->setFixedHeight(45);
  fieldsLayout->addRow(statusLabel, statusCombo);

  QLabel *locationLabel = new QLabel("Localisation:");
  locationLabel->setObjectName("formLabel");
  locationInput = new QLineEdit();
  locationInput->setObjectName("formInput");
  locationInput->setPlaceholderText("Ex: Tunis, Ariana, Sfax");
  locationInput->setFixedHeight(45);
  fieldsLayout->addRow(locationLabel, locationInput);

  formLayout->addLayout(fieldsLayout);
  formLayout->addSpacing(20);

  saveButton = new QPushButton("💾 Enregistrer");
  saveButton->setObjectName("saveButton");
  saveButton->setFixedHeight(50);
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
  contentLayout->setContentsMargins(30, 30, 30, 30);
  contentLayout->setSpacing(20);

  QWidget *contentWrapper = new QWidget();
  contentWrapper->setObjectName("contentWrapper");

  QVBoxLayout *wrapperLayout = new QVBoxLayout(contentWrapper);
  wrapperLayout->setContentsMargins(25, 25, 25, 25);
  wrapperLayout->setSpacing(20);

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
  searchInput->setFixedHeight(45);
  searchInput->setMinimumWidth(300);
  connect(searchInput, &QLineEdit::textChanged, this,
          &GestionCamions::onSearchTextChanged);

  statusFilter = new QComboBox();
  statusFilter->setObjectName("filterSelect");
  statusFilter->addItem("Tous les statuts");
  statusFilter->addItem("Actif");
  statusFilter->addItem("Maintenance");
  statusFilter->addItem("En panne");
  statusFilter->setFixedHeight(45);
  statusFilter->setMinimumWidth(180);
  connect(statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &GestionCamions::onFilterChanged);

  QPushButton *btnPdf = new QPushButton("📊 Export Rapport");
  btnPdf->setObjectName("exportPdfButton");
  btnPdf->setFixedHeight(45);
  connect(btnPdf, &QPushButton::clicked, this, &GestionCamions::onExportPDF);

  filtersLayout->addWidget(searchInput);
  filtersLayout->addWidget(statusFilter);
  filtersLayout->addStretch();
  filtersLayout->addWidget(btnPdf);

  wrapperLayout->addLayout(filtersLayout);

  truckTable = new QTableWidget();
  truckTable->setObjectName("truckTable");
  truckTable->setColumnCount(6);
  truckTable->setHorizontalHeaderLabels(
      {"ID", "Type", "Capacité", "Statut", "Localisation", "Actions"});
  truckTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  truckTable->verticalHeader()->setVisible(false);
  truckTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  truckTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  truckTable->setShowGrid(false);

  wrapperLayout->addWidget(truckTable);
  contentLayout->addWidget(contentWrapper);
}

void GestionCamions::applyStyles() {
  QString styleSheet = R"(
        #formPanel { background: white; border-right: 1px solid #e0e0e0; }
        #formTitle { color: #2C5F2D; }
        #formLabel { color: #000000; font-weight: 600; font-size: 14px; }
        #formInput { border: 2px solid #e0e0e0; border-radius: 10px; padding: 10px 15px; font-size: 14px; background: white; color: #000000; }
        #formInput:focus { border-color: #A3C651; }
        #saveButton { background: #A3C651; color: white; border: none; border-radius: 10px; font-size: 16px; font-weight: bold; }
        #saveButton:hover { background: #8AB344; }
        #infoBox { background: white; color: #333; padding: 15px; border-radius: 10px; border: 1px solid #eee; }
        #mainContent { background: #f5f5f5; }
        #contentWrapper { background: white; border-radius: 15px; }
        #pageTitle { font-size: 26px; font-weight: bold; color: #000000; }
        #searchInput, #filterSelect { border: 1px solid #ddd; border-radius: 10px; padding: 0 15px; font-size: 14px; background: white; color: #000000; }
        #truckTable { background: white; border: none; gridline-color: #eee; color: #000000; }
        #truckTable::item { padding: 15px; border-bottom: 1px solid #eee; color: #000000; }
        QHeaderView::section { background: #f9f9f9; padding: 12px; border: none; border-bottom: 2px solid #eee; font-weight: 600; color: #000000; }
        #exportPdfButton { background: #E74C3C; color: white; border: none; border-radius: 8px; padding: 0 25px; font-size: 15px; font-weight: bold; }
        #exportPdfButton:hover { background: #C0392B; }
    )";
  this->setStyleSheet(styleSheet);
}

void GestionCamions::loadTruckData() {
  addTableRow(1, "Compact", "5 T", "Actif", "Tunis");
  addTableRow(2, "Grand", "15 T", "Maintenance", "Sousse");
  addTableRow(3, "Électrique", "8 T", "En panne", "Sfax");
}

void GestionCamions::addTableRow(int id, const QString &type,
                                 const QString &capacity, const QString &status,
                                 const QString &location) {
  int row = truckTable->rowCount();
  truckTable->insertRow(row);

  truckTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
  truckTable->setItem(row, 1, new QTableWidgetItem(type));
  truckTable->setItem(row, 2, new QTableWidgetItem(capacity));

  QLabel *statusLabel = new QLabel(status);
  statusLabel->setStyleSheet(getStatusStyle(status));
  statusLabel->setAlignment(Qt::AlignCenter);
  statusLabel->setContentsMargins(10, 5, 10, 5);
  truckTable->setCellWidget(row, 3, statusLabel);

  truckTable->setItem(row, 4, new QTableWidgetItem(location));

  QWidget *actionsWidget = new QWidget();
  QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
  actionsLayout->setContentsMargins(5, 5, 5, 5);
  actionsLayout->setSpacing(10);

  QPushButton *editBtn = new QPushButton("Modifier");
  editBtn->setStyleSheet(
      "background: #E3F2FD; color: #2196F3; border-radius: 5px; padding: 5px;");
  connect(editBtn, &QPushButton::clicked,
          [this, row]() { onModifyTruck(row); });

  QPushButton *deleteBtn = new QPushButton("Supprimer");
  deleteBtn->setStyleSheet(
      "background: #FFEBEE; color: #B84446; border-radius: 5px; padding: 5px;");
  connect(deleteBtn, &QPushButton::clicked,
          [this, row]() { onDeleteTruck(row); });

  actionsLayout->addWidget(editBtn);
  actionsLayout->addWidget(deleteBtn);
  truckTable->setCellWidget(row, 5, actionsWidget);
}

QString GestionCamions::getStatusStyle(const QString &status) {
  if (status == "Actif")
    return "color: #4CAF50; background: #E8F5E9; border-radius: 12px; "
           "font-weight: bold; padding: 2px 10px;";
  if (status == "Maintenance")
    return "color: #FF9800; background: #FFF3E0; border-radius: 12px; "
           "font-weight: bold; padding: 2px 10px;";
  return "color: #F44336; background: #FFEBEE; border-radius: 12px; "
         "font-weight: bold; padding: 2px 10px;";
}

void GestionCamions::onAddTruck() {
  QString type = typeInput->text();
  QString location = locationInput->text();
  if (type.isEmpty() || location.isEmpty())
    return;

  QString status = statusCombo->currentText();
  QString capacity = QString::number(capacityInput->value()) + " T";

  if (currentEditingRow >= 0) {
    truckTable->item(currentEditingRow, 1)->setText(type);
    truckTable->item(currentEditingRow, 2)->setText(capacity);
    truckTable->item(currentEditingRow, 4)->setText(location);

    QLabel *newStatusLabel = new QLabel(status);
    newStatusLabel->setStyleSheet(getStatusStyle(status));
    newStatusLabel->setAlignment(Qt::AlignCenter);
    truckTable->setCellWidget(currentEditingRow, 3, newStatusLabel);

    clearFormInputs();
  } else {
    addTableRow(nextId++, type, capacity, status, location);
    clearFormInputs();
  }
}

void GestionCamions::onModifyTruck(int row) {
  currentEditingRow = row;
  formTitleLabel->setText("📝 Modifier camion #" +
                          truckTable->item(row, 0)->text());
  typeInput->setText(truckTable->item(row, 1)->text());
  locationInput->setText(truckTable->item(row, 4)->text());

  QString capStr = truckTable->item(row, 2)->text();
  capacityInput->setValue(capStr.remove(" T").toInt());

  QLabel *statusLbl = qobject_cast<QLabel *>(truckTable->cellWidget(row, 3));
  if (statusLbl)
    statusCombo->setCurrentText(statusLbl->text());

  saveButton->setText("💾 Mettre à jour");
}

void GestionCamions::onDeleteTruck(int row) { truckTable->removeRow(row); }

void GestionCamions::onSearchTextChanged(const QString &text) {
  for (int i = 0; i < truckTable->rowCount(); ++i) {
    bool match = false;
    for (int j = 0; j < truckTable->columnCount() - 1; ++j) {
      QTableWidgetItem *item = truckTable->item(i, j);
      if (item && item->text().contains(text, Qt::CaseInsensitive)) {
        match = true;
        break;
      }
    }
    truckTable->setRowHidden(i, !match);
  }
}

void GestionCamions::onFilterChanged(int index) {
  QString selected = statusFilter->currentText();
  for (int i = 0; i < truckTable->rowCount(); ++i) {
    if (index == 0) {
      truckTable->setRowHidden(i, false);
      continue;
    }
    QLabel *lbl = qobject_cast<QLabel *>(truckTable->cellWidget(i, 3));
    truckTable->setRowHidden(i, lbl && lbl->text() != selected);
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
  QString fileName = QFileDialog::getSaveFileName(this, "Exporter en Rapport",
                                                  "Rapport_Camions.html",
                                                  "Fichiers HTML (*.html)");
  if (fileName.isEmpty())
    return;

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;

  QTextStream out(&file);
  out << "<html><head><style>table { width:100%; border-collapse: collapse; } "
         "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; } "
         "th { background-color: #2C5F2D; color: white; }</style></head><body>";
  out << "<h1>Rapport de Gestion des Camions - TuniWaste</h1>";
  out << "<table><tr><th>ID</th><th>Type</th><th>Capacite</th><th>Statut</"
         "th><th>Localisation</th></tr>";

  for (int i = 0; i < truckTable->rowCount(); ++i) {
    out << "<tr>";
    out << "<td>" << truckTable->item(i, 0)->text() << "</td>";
    out << "<td>" << truckTable->item(i, 1)->text() << "</td>";
    out << "<td>" << truckTable->item(i, 2)->text() << "</td>";
    QLabel *lbl = qobject_cast<QLabel *>(truckTable->cellWidget(i, 3));
    out << "<td>" << (lbl ? lbl->text() : "") << "</td>";
    out << "<td>" << truckTable->item(i, 4)->text() << "</td>";
    out << "</tr>";
  }
  out << "</table></body></html>";
  file.close();
  QMessageBox::information(this, "Succès", "Rapport exporté !");
  QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}
