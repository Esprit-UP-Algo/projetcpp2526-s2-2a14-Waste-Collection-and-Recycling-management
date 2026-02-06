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
#include <QDialog>
#include <QSpinBox>

// Dialogue d'ajout/modification intégré
class TruckDialog : public QDialog
{
    Q_OBJECT
public:
    TruckDialog(QWidget *parent = nullptr);
    void setData(const QString &type, int capacity, const QString &status, const QString &location);
    QString getType() const;
    int getCapacity() const;
    QString getStatus() const;
    QString getLocation() const;

private:
    QLineEdit *typeInput;
    QSpinBox *capacityInput;
    QComboBox *statusCombo;
    QLineEdit *locationInput;
};

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

private:
    // Main widgets
    QWidget *centralWidget;
    QWidget *sidebar;
    QWidget *mainContent;

    // Table
    QTableWidget *truckTable;

    // Buttons
    QPushButton *addButton;
    QVector<QPushButton*> menuButtons;

    // Search and filters
    QLineEdit *searchInput;
    QComboBox *statusFilter;
    QComboBox *typeFilter;

    // Data
    int nextId;

    // Helper methods
    void setupUI();
    void createSidebar();
    void createMainContent();
    void loadTruckData();
    void applyStyles();
    void addTableRow(int id, const QString &type, const QString &capacity,
                     const QString &status, const QString &location);
    QString getStatusStyle(const QString &status);
};

#endif // MAINWINDOW_H
