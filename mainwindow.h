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
#include <QSqlQuery>
#include <QSqlError>
#include "database.h"

struct User {
    int id;
    QString firstName;
    QString lastName;
    QString email;
    QString phone;
    QString role;
    QString gender;
    QString city;
    QString postalCode;
    QString photoPath;
    QString password;
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
    void onSaveUserClicked();
    void onModifyUser(int row);
    void onDeleteUser(int row);
    void onForgotPasswordClicked();
    void onSearchTextChanged(const QString &text);
    void onSortByColumn(int column);
    void onExportPdfClicked();
    void onBrowsePhotoClicked();
    void onClearFormClicked();

private:
    void setupLoginScreen();
    void setupUserManagementScreen();
    void updateUserTable();
    void filterAndSortUsers();
    void clearForm();
    void loadUserToForm(const User &user);
    QWidget* createSidebar();

    // Oracle DB methods — TUNIWASTE.UTILISATEUR
    void loadUsersFromDB();
    bool addUserToDB(const User &user);
    bool updateUserInDB(const User &user);
    bool deleteUserFromDB(int userId);
    bool loginFromDB(const QString &email, const QString &password);

    QStackedWidget *stackedWidget;
    QTableWidget *userTable;
    QVector<User> users;
    QVector<User> filteredUsers;
    int nextUserId;
    QLineEdit *searchEdit;
    QComboBox *roleFilter;
    int currentSortColumn;
    Qt::SortOrder currentSortOrder;

    QLineEdit *formFirstNameEdit;
    QLineEdit *formLastNameEdit;
    QLineEdit *formEmailEdit;
    QLineEdit *formPhoneEdit;
    QComboBox *formRoleCombo;
    QComboBox *formGenderCombo;
    QLineEdit *formCityEdit;
    QLineEdit *formPostalCodeEdit;
    QLabel *formPhotoLabel;
    QLineEdit *formPasswordEdit;
    QString formPhotoPath;
    int editingUserId;

    // Saved login fields to use in onLoginClicked
    QLineEdit *loginEmailEdit;
    QLineEdit *loginPasswordEdit;
};

#endif
