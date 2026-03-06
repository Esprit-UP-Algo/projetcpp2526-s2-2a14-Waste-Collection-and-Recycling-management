#ifndef OTPMANAGER_H
#define OTPMANAGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTcpSocket>
#include <QDebug>

// ══════════════════════════════════════════════════════════════════════════════
//  OtpManager — Génère et envoie un code OTP par email (Gmail SMTP)
//
//  Prérequis Gmail :
//    1. Activer la validation en deux étapes sur le compte Gmail
//    2. Générer un "Mot de passe d'application" (App Password) dans :
//       Mon compte → Sécurité → Mots de passe des applications
//    3. Remplacer GMAIL_SENDER et GMAIL_APP_PASSWORD ci-dessous
//
//  Le code OTP est à 6 chiffres, valide 5 minutes.
// ══════════════════════════════════════════════════════════════════════════════

// ─── À CONFIGURER ────────────────────────────────────────────────────────────
#define GMAIL_SENDER       "dhiahobby1@gmail.com"
#define GMAIL_APP_PASSWORD "rvlviuvwkfkzptjk"   // mot de passe d'application (16 cars)
// ─────────────────────────────────────────────────────────────────────────────

class OtpManager : public QObject
{
    Q_OBJECT

public:
    static OtpManager& getInstance();

    // Génère un code 6 chiffres + l'envoie par email → retourne true si envoi OK
    bool sendOtp(const QString &toEmail);

    // Envoie le mot de passe actuel par email (réinitialisation)
    bool sendPasswordEmail(const QString &toEmail, const QString &password);

    // Vérifie si le code saisi correspond au code envoyé (et non expiré)
    bool verifyOtp(const QString &code);

    // Code encore valide ?
    bool isOtpValid() const;

    // Durée restante en secondes
    int remainingSeconds() const;

private:
    OtpManager() {}
    OtpManager(const OtpManager&) = delete;
    OtpManager& operator=(const OtpManager&) = delete;

    QString   currentOtp;
    QDateTime otpExpiry;
    QString   otpEmail;

    QString generateCode();
    bool    sendEmail(const QString &to, const QString &code);
};

// ══════════════════════════════════════════════════════════════════════════════
//  OtpDialog — Dialogue de saisie du code OTP
// ══════════════════════════════════════════════════════════════════════════════
class OtpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OtpDialog(const QString &email, QWidget *parent = nullptr);
    bool isVerified() const { return verified; }

private slots:
    void onVerifyClicked();
    void onResendClicked();
    void updateTimer();

private:
    QString    targetEmail;
    bool       verified = false;
    int        secondsLeft = 300; // 5 minutes

    QLineEdit  *otpInput;
    QLabel     *timerLabel;
    QLabel     *statusLabel;
    QPushButton *resendBtn;
    QTimer     *countdownTimer;
};

#endif // OTPMANAGER_H
