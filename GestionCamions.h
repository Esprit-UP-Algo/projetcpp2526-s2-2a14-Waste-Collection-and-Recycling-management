#ifndef GESTIONCAMIONS_H
#define GESTIONCAMIONS_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
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
  // Main widgets
  QWidget *formPanel;
  QWidget *mainContent;

  // Table
  QTableWidget *truckTable;

  // Form inputs
  QLineEdit *typeInput;
  QSpinBox *capacityInput;
  QComboBox *statusCombo;
  QLineEdit *locationInput;
  QPushButton *saveButton;
  QLabel *formTitleLabel;

  // Buttons
  QPushButton *exportPdfButton;

  // Chart
  QWidget *chartWidget;

  // Search and filters
  QLineEdit *searchInput;
  QComboBox *statusFilter;

  // Data
  int nextId;
  int currentEditingRow;

  // Helper methods
  void setupUI();
  void createFormPanel();
  void createMainContent();
  void createChartWidget();
  void loadTruckData();
  void applyStyles();
  void addTableRow(int id, const QString &type, const QString &capacity,
                   const QString &status, const QString &location);
  QString getStatusStyle(const QString &status);
  void clearFormInputs();
  void setFormForEditing(int row);
  void updateChartData();
  void addStatBar(QVBoxLayout *layout, const QString &label, int count,
                  int total, const QString &color);
};

#endif // GESTIONCAMIONS_H
