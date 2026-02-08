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
    QString name;
    QString email;
    QString phone;
    QString role;
};

class AddUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddUserDialog(QWidget *parent = nullptr, User *editUser = nullptr);
    QString getName() const { return nameEdit->text(); }
    QString getEmail() const { return emailEdit->text(); }
    QString getPhone() const { return phoneEdit->text(); }
    QString getRole() const { return roleCombo->currentText(); }

private:
    QLineEdit *nameEdit;
    QLineEdit *emailEdit;
    QLineEdit *phoneEdit;
    QComboBox *roleCombo;
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
    void onAddUserClicked();
    void onModifyUser(int row);
    void onDeleteUser(int row);
    void onForgotPasswordClicked();
    void onSearchTextChanged(const QString &text);
    void onSortByColumn(int column);
    void onExportPdfClicked();

private:
    void setupLoginScreen();
    void setupUserManagementScreen();
    void updateUserTable();
    void filterAndSortUsers();
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
};

#endif
