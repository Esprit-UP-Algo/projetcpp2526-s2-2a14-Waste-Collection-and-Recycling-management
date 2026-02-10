#ifndef GESTIONCAMIONS_H
#define GESTIONCAMIONS_H

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

class GestionCamions : public QWidget {
  Q_OBJECT

public:
  explicit GestionCamions(QWidget *parent = nullptr);
  ~GestionCamions();

private slots:
  void onAddTruck();
  void onModifyTruck(int row);
  void onDeleteTruck(int row);
  void onSearchTextChanged(const QString &text);
  void onFilterChanged(int index);
  void onExportPDF();

private:
  // UI Helpers
  void setupUI();
  void createFormPanel();
  void createMainContent();
  void applyStyles();
  void addTableRow(int id, const QString &type, const QString &capacity,
                   const QString &status, const QString &location);
  QString getStatusStyle(const QString &status);
  void clearFormInputs();
  void setFormForEditing(int row);
  void loadTruckData();

  // UI Components
  QWidget *formPanel;
  QWidget *mainContent;
  QTableWidget *truckTable;
  QLineEdit *typeInput;
  QSpinBox *capacityInput;
  QComboBox *statusCombo;
  QLineEdit *locationInput;
  QPushButton *saveButton;
  QLabel *formTitleLabel;
  QLineEdit *searchInput;
  QComboBox *statusFilter;
  QComboBox *typeFilter;

  // Data
  int nextId;
  int currentEditingRow;
};

#endif // GESTIONCAMIONS_H
