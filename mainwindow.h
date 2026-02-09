#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QGroupBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddTruck();
    void onModifyTruck(int row);
    void onDeleteTruck(int row);
    void onMenuItemClicked();
    void onSearchTextChanged(const QString &text);
    void onFilterChanged(int index);
    void onExportPDF();

private:
    // Main widgets
    QWidget *centralWidget;
    QWidget *sidebar;
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
    QVector<QPushButton*> menuButtons;
    QPushButton *exportPdfButton;

    // Chart
    QWidget *chartWidget;

    // Search and filters
    QLineEdit *searchInput;
    QComboBox *statusFilter;
    QComboBox *typeFilter;

    // Data
    int nextId;
    int currentEditingRow;

    // Helper methods
    void setupUI();
    void createSidebar();
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
    void addStatBar(QVBoxLayout *layout, const QString &label, int count, int total, const QString &color);
};

#endif // MAINWINDOW_H
