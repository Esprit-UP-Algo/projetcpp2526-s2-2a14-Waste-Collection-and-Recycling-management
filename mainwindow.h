#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QDialog>
#include <QComboBox>
#include <QVector>
#include <QLabel>

struct User {
    int id;
    QString firstName;     // Prénom
    QString lastName;      // Nom
    QString email;
    QString phone;
    QString role;
    QString gender;        // New: sexe (homme/femme)
    QString city;          // New: ville
    QString postalCode;    // New: code postal
    QString photoPath;     // New: photo de profile path
};

class PasswordResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordResetDialog(QWidget *parent = nullptr);
    QString getEmail() const { return emailEdit->text(); }

private slots:
    void onResetClicked();

private:
    QLineEdit *emailEdit;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginClicked();
    void onSaveUserClicked();     // Changed from onAddUserClicked
    void onModifyUser(int row);
    void onDeleteUser(int row);
    void onForgotPasswordClicked();
    void onSearchTextChanged(const QString &text);
    void onSortByColumn(int column);
    void onExportPdfClicked();
    void onBrowsePhotoClicked();  // New
    void onClearFormClicked();    // New

private:
    void setupLoginScreen();
    void setupUserManagementScreen();
    void updateUserTable();
    void filterAndSortUsers();
    void clearForm();             // New
    void loadUserToForm(const User &user); // New
    QWidget* createSidebar();

    QStackedWidget *stackedWidget;
    QTableWidget *userTable;
    QVector<User> users;
    QVector<User> filteredUsers;
    int nextUserId;
    QLineEdit *searchEdit;
    QComboBox *roleFilter;
    int currentSortColumn;
    Qt::SortOrder currentSortOrder;
    
    // Form fields for the permanent form on top
    QLineEdit *formFirstNameEdit;  // Prénom
    QLineEdit *formLastNameEdit;   // Nom
    QLineEdit *formEmailEdit;
    QLineEdit *formPhoneEdit;
    QComboBox *formRoleCombo;
    QComboBox *formGenderCombo;
    QLineEdit *formCityEdit;
    QLineEdit *formPostalCodeEdit;
    QLabel *formPhotoLabel;
    QString formPhotoPath;
    int editingUserId;  // -1 when adding new, otherwise the ID being edited
};

#endif
