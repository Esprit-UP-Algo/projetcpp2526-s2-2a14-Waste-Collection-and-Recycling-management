#ifndef GESTIONPOUBELLES_H
#define GESTIONPOUBELLES_H

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>


class GestionPoubelles : public QWidget {
  Q_OBJECT

public:
  explicit GestionPoubelles(QWidget *parent = nullptr);
  ~GestionPoubelles();

private slots:
  void onAddPoubelle();
  void onModifyPoubelle(int row);
  void onDeletePoubelle(int row);
  void onSearchTextChanged(const QString &text);
  void onFilterChanged(int index);
  void onExportPDF();

private:
  // Main widgets
  QWidget *formPanel;
  QWidget *mainContent;

  // Table
  QTableWidget *poubelleTable;

  // Form inputs
  QLineEdit *localisationInput;
  QSpinBox *niveauInput;
  QComboBox *etatCombo;
  QLineEdit *idZoneInput;
  QSpinBox *capaciteInput;
  QComboBox *typeCombo;
  QPushButton *saveButton;
  QLabel *formTitleLabel;

  // Buttons
  QPushButton *exportPdfButton;

  // Chart
  QWidget *chartWidget;

  // Search and filters
  QLineEdit *searchInput;
  QComboBox *etatFilter;
  QComboBox *typeFilter;

  // Data
  int nextId;
  int currentEditingRow;

  // Helper methods
  void setupUI();
  void createFormPanel();
  void createMainContent();
  void createChartWidget();
  void loadPoubelleData();
  void applyStyles();
  void addTableRow(int id, const QString &localisation, int niveau,
                   const QString &etat, const QString &idZone, int capacite,
                   const QString &type);
  QString getEtatStyle(const QString &etat);
  QString getNiveauStyle(int niveau);
  void clearFormInputs();
  void setFormForEditing(int row);
  void updateChartData();
  void addStatBar(QVBoxLayout *layout, const QString &label, int count,
                  int total, const QString &color);
};

#endif // GESTIONPOUBELLES_H
