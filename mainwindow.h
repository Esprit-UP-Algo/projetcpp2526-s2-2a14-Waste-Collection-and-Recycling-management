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

    // TODO: Méthodes pour l'intégration utilisateur
    // À appeler depuis le module de gestion des utilisateurs
    void setCurrentUser(const QString &name, const QString &avatarPath = "");
    void updateUserProfile(const QString &name, const QString &role = "");

private slots:
    void onAddPoubelle();
    void onModifyPoubelle(int row);
    void onDeletePoubelle(int row);
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
    QVector<QPushButton*> menuButtons;
    QPushButton *exportPdfButton;

    // Chart
    QWidget *chartWidget;

    // Search and filters
    QLineEdit *searchInput;
    QComboBox *etatFilter;
    QComboBox *typeFilter;

    // TODO: User profile widgets - pour l'intégration future
    QLabel *userAvatar;      // Photo de profil de l'utilisateur
    QLabel *userNameLabel;   // Nom de l'utilisateur connecté

    // Data
    int nextId;
    int currentEditingRow;

    // Helper methods
    void setupUI();
    void createSidebar();
    void createFormPanel();
    void createMainContent();
    void createChartWidget();
    void loadPoubelleData();
    void applyStyles();
    void addTableRow(int id, const QString &localisation, int niveau,
                     const QString &etat, const QString &idZone,
                     int capacite, const QString &type);
    QString getEtatStyle(const QString &etat);
    QString getNiveauStyle(int niveau);
    void clearFormInputs();
    void setFormForEditing(int row);
    void updateChartData();
    void addStatBar(QVBoxLayout *layout, const QString &label, int count, int total, const QString &color);
};

#endif // MAINWINDOW_H
