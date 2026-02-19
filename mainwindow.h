#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QString>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

class Recyclage;
class GestionZones;
class GestionCamions;
class GestionPoubelles;
class Dashboard;

struct User {
  int id;
  QString name;
  QString email;
  QString phone;
  QString role;
};

class AddUserDialog : public QDialog {
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

class PasswordResetDialog : public QDialog {
  Q_OBJECT

public:
  explicit PasswordResetDialog(QWidget *parent = nullptr);
  QString getEmail() const { return emailEdit->text(); }

private slots:
  void onResetClicked();

private:
  QLineEdit *emailEdit;
};

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void onLoginClicked();
  void onAddUserClicked();
  void onModifyUser(int row);
  void onDeleteUser(int row);
  void onUsersClicked();
  void onForgotPasswordClicked();
  void onSearchTextChanged(const QString &text);
  void onSortByColumn(int column);
  void onRecycleClicked();
  void onZonesClicked();
  void onTrucksClicked();
  void onBinsClicked();
  void onDashboardClicked();
  void onExportUsersPDF();
  void onReportsClicked();
  void onSettingsClicked();

  // New Slots for Split Layout User Management
  void onUserSaveClicked();
  void onUserSearchChanged(const QString &text);

private:
  // Initialization
  void setupLoginScreen();
  void setupAppShell();

  // Module Setup
  void setupUserManagementScreen();
  void setupRecyclingScreen();
  void setupZonesScreen();
  void setupTrucksScreen();
  void setupBinsScreen();
  void setupDashboardScreen();

  // User Management Helpers
  void createUserFormPanel();
  void createUserMainContent();
  void createUserChartWidget();
  void updateUserTable();
  void filterAndSortUsers();
  void updateUserChart();
  void addStatBar(QVBoxLayout *layout, const QString &label, int count,
                  int total, const QString &color);
  void clearUserForm();

  // General Helpers
  QWidget *createSidebar();

  // Widgets
  QStackedWidget *stackedWidget;        // Main: Login vs App
  QStackedWidget *contentStackedWidget; // App: Modules

  // UI Elements - User Management
  QWidget *userFormPanel;
  QWidget *userMainContent;
  QWidget *userChartWidget;
  QLabel *userFormTitle;
  QLineEdit *userNameInput;
  QLineEdit *userEmailInput;
  QLineEdit *userPhoneInput;
  QComboBox *userRoleCombo;
  QPushButton *userSaveButton;
  QLineEdit *userSearchInput;
  QComboBox *userRoleFilter;

  // UI Elements - Login
  QLineEdit *emailEdit;
  QLineEdit *passwordEdit;

  // Shared/Legacy Members (kept for compatibility with existing .cpp code until
  // fully refactored)
  QTableWidget *userTable;
  QVector<User> users;
  QVector<User> filteredUsers;
  int nextUserId;
  int currentUserEditingRow; // For new edit logic

  // Legacy members that might still be referenced in non-refactored code
  QLineEdit *searchEdit;
  QComboBox *roleFilter;
  int currentSortColumn;
  Qt::SortOrder currentSortOrder;

  // Module Instances
  Recyclage *recyclageWidget;
  GestionZones *zonesWidget;
  GestionCamions *camionsWidget;
  GestionPoubelles *poubellesWidget;
  Dashboard *dashboardWidget;
};

#endif
