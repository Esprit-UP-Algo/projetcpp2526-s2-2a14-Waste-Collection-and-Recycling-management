#ifndef GESTIONZONES_H
#define GESTIONZONES_H

#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>

class GestionZones : public QWidget {
  Q_OBJECT

public:
  explicit GestionZones(QWidget *parent = nullptr);

private slots:
  void onAjouterClicked();
  void onModifierClicked(int row);
  void onSupprimerClicked(int row);
  void onExportPDFClicked();
  void onRechercheChanged();

private:
  // UI Helpers
  void setupUi();
  void updateTable();
  void addTableRow(int row, QString id, QString nom, QString loc, QString pop,
                   QString surf, QString freq);
  QWidget *createSideBar();
  QWidget *createHeader();

  // Data handling (currently using table as storage, but could be struct list)

  // Widgets
  QTableWidget *tableZones;

  // Form Inputs (for the dialog/form)
  QLineEdit *idEdit;
  QLineEdit *nomEdit;
  QLineEdit *localisationEdit;
  QLineEdit *populationEdit;
  QLineEdit *surfaceEdit;
  QComboBox
      *frequenceEdit; // Frequence might be better as combo (Daily, Weekly...)

  // Search/Filter
  QLineEdit *searchId;
  QLineEdit *searchNom;

  // Buttons
  QPushButton *btnAjouter;
  QPushButton *btnExportPDF;
};

#endif // GESTIONZONES_H
