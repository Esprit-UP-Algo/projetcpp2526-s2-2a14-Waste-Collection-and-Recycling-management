#include "gestionzones.h"

#include <QCoreApplication>
#include <QDate>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>
#include <QPixmap>
#include <QUrl>
#include <QVBoxLayout>

GestionZones::GestionZones(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("TuniWaste - Gestion des Zones");
  resize(1200, 700);

  setupUi();

  // Connect search filters
  connect(searchId, &QLineEdit::textChanged, this,
          &GestionZones::onRechercheChanged);
  connect(searchNom, &QLineEdit::textChanged, this,
          &GestionZones::onRechercheChanged);
}

void GestionZones::setupUi() {
  QWidget *central = new QWidget(this);
  setCentralWidget(central);
  central->setStyleSheet("background-color: #F4F7F6;"); // Light grey background

  QHBoxLayout *mainLayout = new QHBoxLayout(central);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // 1. Sidebar
  mainLayout->addWidget(createSideBar());

  // 2. Main Content
  QWidget *contentWidget = new QWidget();
  QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
  contentLayout->setContentsMargins(30, 30, 30, 30);
  contentLayout->setSpacing(20);

  // Header
  contentLayout->addWidget(createHeader());

  // Filter/Action Bar
  QHBoxLayout *filterLayout = new QHBoxLayout();

  searchId = new QLineEdit();
  searchId->setPlaceholderText("🔍 Rechercher par ID...");
  searchId->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #ddd; "
                          "border-radius: 5px; background: white; }");

  searchNom = new QLineEdit();
  searchNom->setPlaceholderText("🔍 Rechercher par Nom...");
  searchNom->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #ddd; "
                           "border-radius: 5px; background: white; }");

  btnExportPDF = new QPushButton("Export PDF");
  btnExportPDF->setIcon(QIcon::fromTheme("application-pdf"));
  // Red PDF Button #E74C3C
  btnExportPDF->setStyleSheet(
      "QPushButton { background-color: #FFA500; color: white; border: none; "
      "padding: 10px 20px; border-radius: 5px; font-weight: bold; } "
      "QPushButton:hover { background-color: #E69500; }");
  connect(btnExportPDF, &QPushButton::clicked, this,
          &GestionZones::onExportPDFClicked);

  btnAjouter = new QPushButton("+ Ajouter zone");
  btnAjouter->setCursor(Qt::PointingHandCursor);
  // Branding Green #9BCB4E
  btnAjouter->setStyleSheet(
      "QPushButton { background-color: #9BCB4E; color: white; border: none; "
      "padding: 10px 20px; border-radius: 5px; font-weight: bold; } "
      "QPushButton:hover { background-color: #8EB340; }");
  connect(btnAjouter, &QPushButton::clicked, this,
          &GestionZones::onAjouterClicked);

  filterLayout->addWidget(searchId);
  filterLayout->addWidget(searchNom);
  filterLayout->addWidget(btnExportPDF); // Added here
  filterLayout->addStretch();
  filterLayout->addWidget(btnAjouter);

  contentLayout->addLayout(filterLayout);

  // Form Section
  QGroupBox *formGroup = new QGroupBox("Détails de la zone");
  formGroup->setStyleSheet(
      "QGroupBox { background: white; border: 1px solid #ddd; border-radius: "
      "8px; margin-top: 10px; font-weight: bold; } QGroupBox::title { "
      "subcontrol-origin: margin; left: 10px; padding: 0 5px; }");
  QGridLayout *formLayout = new QGridLayout(formGroup);
  formLayout->setContentsMargins(20, 30, 20, 20);

  idEdit = new QLineEdit();
  idEdit->setPlaceholderText("ID");
  nomEdit = new QLineEdit();
  nomEdit->setPlaceholderText("Nom de la zone");
  localisationEdit = new QLineEdit();
  localisationEdit->setPlaceholderText("Localisation");
  populationEdit = new QLineEdit();
  populationEdit->setPlaceholderText("Population estimée");
  surfaceEdit = new QLineEdit();
  surfaceEdit->setPlaceholderText("Surface (km²)");

  frequenceEdit = new QComboBox();
  frequenceEdit->addItem("Quotidienne");
  frequenceEdit->addItem("Hebdomadaire");
  frequenceEdit->addItem("Mensuelle");

  QString inputStyle = "QLineEdit, QComboBox { padding: 8px; border: 1px solid "
                       "#ccc; border-radius: 4px; }";
  idEdit->setStyleSheet(inputStyle);
  nomEdit->setStyleSheet(inputStyle);
  localisationEdit->setStyleSheet(inputStyle);
  populationEdit->setStyleSheet(inputStyle);
  surfaceEdit->setStyleSheet(inputStyle);
  frequenceEdit->setStyleSheet(inputStyle);

  formLayout->addWidget(new QLabel("ID:"), 0, 0);
  formLayout->addWidget(idEdit, 0, 1);
  formLayout->addWidget(new QLabel("Nom:"), 0, 2);
  formLayout->addWidget(nomEdit, 0, 3);
  formLayout->addWidget(new QLabel("Localisation:"), 1, 0);
  formLayout->addWidget(localisationEdit, 1, 1);
  formLayout->addWidget(new QLabel("Population:"), 1, 2);
  formLayout->addWidget(populationEdit, 1, 3);
  formLayout->addWidget(new QLabel("Surface:"), 2, 0);
  formLayout->addWidget(surfaceEdit, 2, 1);
  formLayout->addWidget(new QLabel("Fréquence:"), 2, 2);
  formLayout->addWidget(frequenceEdit, 2, 3);

  contentLayout->addWidget(formGroup);

  // Table
  tableZones = new QTableWidget();
  tableZones->setColumnCount(7); // ID, Nom, Loc, Pop, Surf, Freq, Actions
  tableZones->setHorizontalHeaderLabels({"ID", "Nom", "Localisation",
                                         "Population", "Surface", "Fréquence",
                                         "Actions"});
  tableZones->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  tableZones->verticalHeader()->setVisible(false);
  tableZones->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableZones->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableZones->setStyleSheet(
      "QTableWidget { background: white; border: 1px solid #ddd; "
      "border-radius: 5px; gridline-color: #eee; } QHeaderView::section { "
      "background-color: #f8f9fa; padding: 10px; border: none; font-weight: "
      "bold; color: #555; }");

  contentLayout->addWidget(tableZones);

  // Footer removed (PDF button moved up)

  mainLayout->addWidget(contentWidget);
}

QWidget *GestionZones::createSideBar() {
  QFrame *sidebar = new QFrame();
  sidebar->setFixedWidth(260);
  // Sidebar Green #7FB069
  sidebar->setStyleSheet("background-color: #7FB069; border: none;");

  QVBoxLayout *layout = new QVBoxLayout(sidebar);
  layout->setContentsMargins(0, 30, 0, 30);
  layout->setSpacing(5);

  // Logo
  QLabel *logo = new QLabel();
  logo->setAlignment(Qt::AlignCenter);

  // Debug paths
  qDebug() << "Current path:" << QDir::currentPath();
  qDebug() << "App path:" << QCoreApplication::applicationDirPath();

  QString logoPath = "logo.png";
  if (!QFile::exists(logoPath)) {
    // Try absolute fallback
    logoPath = "C:/projet_qt/gest/gestzones/logo.png";
  }

  QPixmap logoPixmap(logoPath);
  if (!logoPixmap.isNull()) {
    logo->setPixmap(logoPixmap.scaled(150, 150, Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));
    logo->setStyleSheet("margin-bottom: 30px;");
  } else {
    logo->setText("TuniWaste");
    logo->setStyleSheet("font-size: 24px; font-weight: bold; color: white; "
                        "margin-bottom: 30px;");
  }
  layout->addWidget(logo);

  QString menuStyle = "QLabel { color: white; font-size: 16px; padding: 15px "
                      "20px; border-radius: 0px; } QLabel:hover { "
                      "background-color: rgba(255,255,255,0.1); }";
  QString activeStyle = "QLabel { color: #7FB069; background-color: white; "
                        "font-size: 16px; padding: 15px 20px; font-weight: "
                        "bold; border-left: 5px solid #5A8F47; }";

  QStringList menus = {"📊 Tableau de bord",     "👥 Gestion des utilisateurs",
                       "🚛 Gestion des camions", "🗑 Gestion des poubelles",
                       "📍 Gestion des zones",   "♻ Gestion de recyclage",
                       "📈 Suivi des collectes", "📄 Rapports"};

  for (const QString &m : menus) {
    QLabel *lbl = new QLabel(m);
    if (m.contains("Gestion des zones")) {
      lbl->setStyleSheet(activeStyle);
    } else {
      lbl->setStyleSheet(menuStyle);
    }
    layout->addWidget(lbl);
  }

  layout->addStretch();

  QLabel *settings = new QLabel("⚙ Paramètres");
  settings->setStyleSheet(menuStyle);
  layout->addWidget(settings);

  return sidebar;
}

QWidget *GestionZones::createHeader() {
  QWidget *header = new QWidget();
  QHBoxLayout *hLayout = new QHBoxLayout(header);
  hLayout->setContentsMargins(0, 0, 0, 20);

  QLabel *title = new QLabel("Gestion des Zones");
  title->setStyleSheet("font-size: 28px; font-weight: bold; color: #333;");

  hLayout->addWidget(title);
  hLayout->addStretch();

  QLabel *userLbl = new QLabel("Admin");
  userLbl->setStyleSheet("color: #666; font-size: 14px;");
  hLayout->addWidget(userLbl);

  return header;
}

void GestionZones::onAjouterClicked() {
  QString id = idEdit->text();
  QString nom = nomEdit->text();
  QString loc = localisationEdit->text();
  QString pop = populationEdit->text();
  QString surf = surfaceEdit->text();
  QString freq = frequenceEdit->currentText();

  if (id.isEmpty() || nom.isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez remplir au moins l'ID et le Nom.");
    return;
  }

  addTableRow(tableZones->rowCount(), id, nom, loc, pop, surf, freq);

  // Clear
  idEdit->clear();
  nomEdit->clear();
  localisationEdit->clear();
  populationEdit->clear();
  surfaceEdit->clear();
}

void GestionZones::onRechercheChanged() {
  QString idFilter = searchId->text().toLower();
  QString nomFilter = searchNom->text().toLower();

  for (int i = 0; i < tableZones->rowCount(); ++i) {
    bool match = true;
    QString id = tableZones->item(i, 0)->text().toLower();
    QString nom = tableZones->item(i, 1)->text().toLower();

    if (!idFilter.isEmpty() && !id.contains(idFilter))
      match = false;
    if (!nomFilter.isEmpty() && !nom.contains(nomFilter))
      match = false;

    tableZones->setRowHidden(i, !match);
  }
}

void GestionZones::addTableRow(int row, QString id, QString nom, QString loc,
                               QString pop, QString surf, QString freq) {
  tableZones->insertRow(row);
  tableZones->setItem(row, 0, new QTableWidgetItem(id));
  tableZones->setItem(row, 1, new QTableWidgetItem(nom));
  tableZones->setItem(row, 2, new QTableWidgetItem(loc));
  tableZones->setItem(row, 3, new QTableWidgetItem(pop));
  tableZones->setItem(row, 4, new QTableWidgetItem(surf));
  tableZones->setItem(row, 5, new QTableWidgetItem(freq));

  // Action Buttons Widget
  QWidget *actionWidget = new QWidget();
  QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
  actionLayout->setContentsMargins(5, 2, 5, 2);
  actionLayout->setSpacing(5);

  QPushButton *btnEdit = new QPushButton("Modifier");
  // Vert Action (Modifier): #2ECC71
  btnEdit->setStyleSheet(
      "background-color: #2ECC71; color: white; border: none; padding: 5px "
      "10px; border-radius: 3px; font-weight: bold;");

  QPushButton *btnDelete = new QPushButton("Supprimer");
  // Rouge (Supprimer): #E74C3C
  btnDelete->setStyleSheet(
      "background-color: #E74C3C; color: white; border: none; padding: 5px "
      "10px; border-radius: 3px; font-weight: bold;");

  actionLayout->addWidget(btnEdit);
  actionLayout->addWidget(btnDelete);

  tableZones->setCellWidget(row, 6, actionWidget);

  // Use sender() logic via slots
  connect(btnEdit, &QPushButton::clicked, [this]() { onModifierClicked(0); });

  connect(btnDelete, &QPushButton::clicked,
          [this]() { onSupprimerClicked(0); });
}

void GestionZones::onModifierClicked(int) {
  QPushButton *btn = qobject_cast<QPushButton *>(sender());
  if (!btn)
    return;

  // Find the row containing this button
  QPoint pos = btn->parentWidget()->mapTo(tableZones, QPoint(0, 0));
  int row = tableZones->indexAt(pos).row();

  if (row < 0 || row >= tableZones->rowCount())
    return;

  // Load data from table to form
  idEdit->setText(tableZones->item(row, 0)->text());
  nomEdit->setText(tableZones->item(row, 1)->text());
  localisationEdit->setText(tableZones->item(row, 2)->text());
  populationEdit->setText(tableZones->item(row, 3)->text());
  surfaceEdit->setText(tableZones->item(row, 4)->text());
  frequenceEdit->setCurrentText(tableZones->item(row, 5)->text());

  // Remove the row to allow re-adding (Update flow simulation)
  tableZones->removeRow(row);
}

void GestionZones::onSupprimerClicked(int) {
  QPushButton *btn = qobject_cast<QPushButton *>(sender());
  if (!btn)
    return;

  QPoint pos = btn->parentWidget()->mapTo(tableZones, QPoint(0, 0));
  int row = tableZones->indexAt(pos).row();

  if (row >= 0) {
    tableZones->removeRow(row);
  }
}

void GestionZones::onExportPDFClicked() {
  QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "",
                                                  "PDF Files (*.pdf)");
  if (fileName.isEmpty())
    return;

  if (QFileInfo(fileName).suffix().isEmpty()) {
    fileName.append(".pdf");
  }

  QPdfWriter writer(fileName);
  writer.setPageSize(QPageSize(QPageSize::A4));
  writer.setPageMargins(QMarginsF(30, 30, 30, 30));

  QPainter painter(&writer);
  painter.setPen(Qt::black);
  painter.setFont(QFont("Arial", 10));

  // Calculate Stats
  int totalZones = tableZones->rowCount();
  double totalPop = 0;
  double totalSurf = 0;

  for (int i = 0; i < totalZones; i++) {
    if (tableZones->item(i, 3))
      totalPop += tableZones->item(i, 3)->text().toDouble();
    if (tableZones->item(i, 4))
      totalSurf += tableZones->item(i, 4)->text().toDouble();
  }

  // Draw Header
  painter.setFont(QFont("Arial", 20, QFont::Bold));
  painter.setPen(QColor("#9BCB4E")); // Branding Green
  painter.drawText(200, 200, "TuniWaste - Rapport des Zones");

  painter.setFont(QFont("Arial", 12));
  painter.setPen(Qt::black);
  painter.drawText(200, 500, "Date: " + QDate::currentDate().toString());

  // Draw Stats
  int y = 1000;
  painter.setFont(QFont("Arial", 12, QFont::Bold));
  painter.drawText(200, y, "Statistiques Générales:");
  y += 300;
  painter.setFont(QFont("Arial", 11));
  painter.drawText(400, y,
                   QString("Nombre total de zones : %1").arg(totalZones));
  y += 250;
  painter.drawText(400, y,
                   QString("Population totale estimée : %1").arg(totalPop));
  y += 250;
  painter.drawText(400, y, QString("Surface totale (km²) : %1").arg(totalSurf));
  y += 250;

  // Draw Table Header
  y += 500;
  painter.setFont(QFont("Arial", 10, QFont::Bold));
  painter.drawText(200, y, "ID");
  painter.drawText(1000, y, "Nom");
  painter.drawText(3000, y, "Localisation");
  painter.drawText(5000, y, "Population");
  painter.drawText(7000, y, "Surface");
  painter.drawText(8500, y, "Fréquence");

  painter.drawLine(200, y + 100, 9000, y + 100);
  y += 300;

  // Draw Table Rows
  painter.setFont(QFont("Arial", 10));
  for (int i = 0; i < tableZones->rowCount(); i++) {
    if (tableZones->isRowHidden(i))
      continue;

    if (!tableZones->item(i, 0))
      continue;

    painter.drawText(200, y, tableZones->item(i, 0)->text());
    painter.drawText(1000, y, tableZones->item(i, 1)->text());
    painter.drawText(3000, y, tableZones->item(i, 2)->text());
    painter.drawText(5000, y, tableZones->item(i, 3)->text());
    painter.drawText(7000, y, tableZones->item(i, 4)->text());
    painter.drawText(8500, y, tableZones->item(i, 5)->text());

    y += 250;

    if (y > 13000) { // New page if full
      writer.newPage();
      y = 500;
    }
  }

  painter.end();

  QMessageBox::information(this, "Succès",
                           "Le fichier PDF a été généré avec succès.");

  // Try to open it
  QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}
