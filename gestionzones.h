#ifndef GESTIONZONES_H
#define GESTIONZONES_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

class GestionZones : public QWidget {
  Q_OBJECT

public:
  explicit GestionZones(QWidget *parent = nullptr);
  ~GestionZones(); // removed = default to define in cpp

private slots:
  void onAddZone();
  void onModifyZone(int row);
  void onDeleteZone(int row);
  void onSearchTextChanged(const QString &text);
  void onExportPDF();

private:
  // Main widgets
  QWidget *formPanel;
  QWidget *mainContent;

  // Table
  QTableWidget *tableZones;

  // Form inputs
  QLineEdit *idEdit;
  QLineEdit *nomEdit;
  QLineEdit *localisationEdit;
  QLineEdit *populationEdit;
  QLineEdit *surfaceEdit;
  QComboBox *frequenceEdit;
  QPushButton *saveButton;
  QLabel *formTitleLabel;

  // Buttons
  QPushButton *btnExportPDF;

  // Chart
  QWidget *chartWidget;

  // Search
  QLineEdit *searchNom;

  // Data
  int currentEditingRow;

  // Helper methods
  void setupUI();
  void createFormPanel();
  void createMainContent();
  void createChartWidget();
  void loadZoneData();
  void applyStyles();
  void addTableRow(const QString &id, const QString &nom, const QString &loc,
                   const QString &pop, const QString &surf,
                   const QString &freq);
  void clearFormInputs();
  void setFormForEditing(int row);
  void updateChartData();
  void addStatBar(QVBoxLayout *layout, const QString &label, int count,
                  int total, const QString &color);
};

#endif // GESTIONZONES_H
