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
#include <QRegularExpression>
#include "utilisateur.h"
#include "chatbot.h"
#include "otpmanager.h"

// ── Structure locale pour la table en mémoire (cache depuis BD) ──────────────
struct UserRow {
    int     id;
    QString prenom;
    QString nom;
    QString email;
    QString telephone;
    QString role;
    QString sexe;
    QString ville;
    QString codePostal;
    QString photo;
    QString motDePasse;
};

// ── Dialog réinitialisation mot de passe ─────────────────────────────────────
class PasswordResetDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PasswordResetDialog(QWidget *parent = nullptr);
    QString getEmail() const { return emailEdit->text(); }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onResetClicked();

private:
    void showStatus(const QString &msg, bool success);

    QLineEdit *emailEdit;
    QLabel    *statusLabel;

    bool   m_dragging   = false;
    QPoint m_dragOffset;
};

// ── Fenêtre principale ────────────────────────────────────────────────────────
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void refreshTable();           // charge depuis BD et reaffiche

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
    void updateUserTable();        // remplit le QTableWidget depuis filteredUsers
    void filterAndSortUsers();
    void clearForm();
    void loadUserToForm(const UserRow &user);
    QWidget* createSidebar();

    // Modèle (patron Modèle-Vue)
    Utilisateur Utmp;

    // Cache local chargé depuis la BD
    QVector<UserRow> users;
    QVector<UserRow> filteredUsers;

    QStackedWidget *stackedWidget;
    QTableWidget   *userTable;
    QLineEdit      *searchEdit;
    QComboBox      *roleFilter;
    int             currentSortColumn;
    Qt::SortOrder   currentSortOrder;
    int             editingUserId;

    // Champs du formulaire
    QLineEdit *formFirstNameEdit;
    QLineEdit *formLastNameEdit;
    QLineEdit *formEmailEdit;
    QLineEdit *formPhoneEdit;
    QComboBox *formRoleCombo;
    QComboBox *formGenderCombo;
    QLineEdit *formCityEdit;
    QLineEdit *formPostalCodeEdit;
    QLabel    *formPhotoLabel;
    QLineEdit *formPasswordEdit;
    QString    formPhotoPath;

    // Champs login (pour validation)
    QLineEdit *loginEmailEdit;
    QLineEdit *loginPasswordEdit;

    // ChatBot — bulle flottante
    ChatBubbleButton *chatBubble = nullptr;
};

#endif // MAINWINDOW_H
