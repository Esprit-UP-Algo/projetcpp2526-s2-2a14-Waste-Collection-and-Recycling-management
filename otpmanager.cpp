#include "otpmanager.h"
#include <QSslSocket>
#include <QRandomGenerator>

// ══════════════════════════════════════════════════════════════════════════════
//  OtpManager — Singleton
// ══════════════════════════════════════════════════════════════════════════════
OtpManager& OtpManager::getInstance()
{
    static OtpManager instance;
    return instance;
}

QString OtpManager::generateCode()
{
    // Génère un nombre aléatoire entre 100000 et 999999
    int code = QRandomGenerator::global()->bounded(100000, 1000000);
    return QString::number(code);
}

bool OtpManager::sendOtp(const QString &toEmail)
{
    currentOtp = generateCode();
    otpExpiry  = QDateTime::currentDateTime().addSecs(300); // 5 minutes
    otpEmail   = toEmail;

    qDebug() << "[OTP] Code généré :" << currentOtp << "pour" << toEmail;

    return sendEmail(toEmail, currentOtp);
}

// ══════════════════════════════════════════════════════════════════════════════
//  sendPasswordEmail() — Envoie le mot de passe actuel à l'utilisateur
// ══════════════════════════════════════════════════════════════════════════════
bool OtpManager::sendPasswordEmail(const QString &toEmail, const QString &password)
{
    QSslSocket socket;
    socket.connectToHost("smtp.gmail.com", 587);

    if (!socket.waitForConnected(5000)) {
        qDebug() << "[PWD] Impossible de contacter smtp.gmail.com:587";
        return false;
    }

    auto readResponse = [&]() -> QString {
        socket.waitForReadyRead(5000);
        return QString::fromUtf8(socket.readAll());
    };

    auto send = [&](const QString &cmd) {
        socket.write((cmd + "\r\n").toUtf8());
        socket.flush();
        qDebug() << "[SMTP] >>>" << cmd;
        QString resp = readResponse();
        qDebug() << "[SMTP] <<<" << resp.trimmed();
        return resp;
    };

    readResponse(); // Bannière

    send("EHLO localhost");
    send("STARTTLS");
    socket.startClientEncryption();
    if (!socket.waitForEncrypted(5000)) {
        qDebug() << "[PWD] Échec TLS :" << socket.errorString();
        return false;
    }

    send("EHLO localhost");
    send("AUTH LOGIN");
    send(QString(GMAIL_SENDER).toUtf8().toBase64());
    QString authResp = send(QString(GMAIL_APP_PASSWORD).toUtf8().toBase64());
    if (!authResp.startsWith("235")) {
        qDebug() << "[PWD] Authentification Gmail échouée.";
        socket.disconnectFromHost();
        return false;
    }

    send(QString("MAIL FROM:<%1>").arg(GMAIL_SENDER));
    send(QString("RCPT TO:<%1>").arg(toEmail));
    send("DATA");

    QString emailBody;
    emailBody += QString("From: TuniWaste <%1>\r\n").arg(GMAIL_SENDER);
    emailBody += QString("To: %1\r\n").arg(toEmail);
    emailBody += "Subject: =?UTF-8?B?" +
                 QString("Votre mot de passe TuniWaste").toUtf8().toBase64() +
                 "?=\r\n";
    emailBody += "MIME-Version: 1.0\r\n";
    emailBody += "Content-Type: text/plain; charset=UTF-8\r\n";
    emailBody += "Content-Transfer-Encoding: 8bit\r\n";
    emailBody += "\r\n";
    emailBody += "Bonjour,\r\n\r\n";
    emailBody += "Vous avez demandé la récupération de votre mot de passe TuniWaste.\r\n\r\n";
    emailBody += "Votre mot de passe actuel est :\r\n\r\n";
    emailBody += "    " + password + "\r\n\r\n";
    emailBody += "Nous vous recommandons de le changer après connexion.\r\n\r\n";
    emailBody += "Si vous n'avez pas fait cette demande, ignorez cet email.\r\n\r\n";
    emailBody += "Cordialement,\r\nL'équipe TuniWaste\r\n";
    emailBody += "\r\n.\r\n";

    socket.write(emailBody.toUtf8());
    socket.flush();
    QString dataResp = readResponse();
    qDebug() << "[SMTP] DATA réponse :" << dataResp.trimmed();

    send("QUIT");
    socket.disconnectFromHost();

    bool success = dataResp.startsWith("250");
    qDebug() << "[PWD] Email envoyé :" << (success ? "OUI" : "NON");
    return success;
}

bool OtpManager::verifyOtp(const QString &code)
{
    if (currentOtp.isEmpty()) return false;
    if (QDateTime::currentDateTime() > otpExpiry) {
        qDebug() << "[OTP] Code expiré.";
        return false;
    }
    return code.trimmed() == currentOtp;
}

bool OtpManager::isOtpValid() const
{
    return !currentOtp.isEmpty() && QDateTime::currentDateTime() <= otpExpiry;
}

int OtpManager::remainingSeconds() const
{
    if (currentOtp.isEmpty()) return 0;
    return (int)QDateTime::currentDateTime().secsTo(otpExpiry);
}

// ══════════════════════════════════════════════════════════════════════════════
//  sendEmail() — Connexion SMTP/TLS à Gmail (port 587 STARTTLS)
//
//  Protocol SMTP manuel :
//   EHLO → STARTTLS → (upgrade TLS) → AUTH LOGIN → MAIL FROM → RCPT TO → DATA
// ══════════════════════════════════════════════════════════════════════════════
bool OtpManager::sendEmail(const QString &to, const QString &code)
{
    QSslSocket socket;
    socket.connectToHost("smtp.gmail.com", 587);

    if (!socket.waitForConnected(5000)) {
        qDebug() << "[OTP] Impossible de contacter smtp.gmail.com:587";
        return false;
    }

    auto readResponse = [&]() -> QString {
        socket.waitForReadyRead(5000);
        return QString::fromUtf8(socket.readAll());
    };

    auto send = [&](const QString &cmd) {
        socket.write((cmd + "\r\n").toUtf8());
        socket.flush();
        qDebug() << "[SMTP] >>>" << cmd;
        QString resp = readResponse();
        qDebug() << "[SMTP] <<<" << resp.trimmed();
        return resp;
    };

    // Lecture bannière
    QString banner = readResponse();
    qDebug() << "[SMTP] Bannière :" << banner.trimmed();

    // EHLO
    send("EHLO localhost");

    // STARTTLS
    send("STARTTLS");
    socket.startClientEncryption();
    if (!socket.waitForEncrypted(5000)) {
        qDebug() << "[OTP] Échec TLS :" << socket.errorString();
        return false;
    }
    qDebug() << "[SMTP] TLS établi.";

    // Après TLS, re-EHLO
    send("EHLO localhost");

    // AUTH LOGIN
    send("AUTH LOGIN");
    send(QString(GMAIL_SENDER).toUtf8().toBase64());
    QString authResp = send(QString(GMAIL_APP_PASSWORD).toUtf8().toBase64());
    if (!authResp.startsWith("235")) {
        qDebug() << "[OTP] Authentification Gmail échouée.";
        socket.disconnectFromHost();
        return false;
    }

    // MAIL FROM
    send(QString("MAIL FROM:<%1>").arg(GMAIL_SENDER));

    // RCPT TO
    send(QString("RCPT TO:<%1>").arg(to));

    // DATA
    send("DATA");

    // Corps de l'email
    QString emailBody;
    emailBody += QString("From: TuniWaste <%1>\r\n").arg(GMAIL_SENDER);
    emailBody += QString("To: %1\r\n").arg(to);
    emailBody += "Subject: =?UTF-8?B?" +
                 QString("Votre code de vérification TuniWaste").toUtf8().toBase64() +
                 "?=\r\n";
    emailBody += "MIME-Version: 1.0\r\n";
    emailBody += "Content-Type: text/plain; charset=UTF-8\r\n";
    emailBody += "Content-Transfer-Encoding: 8bit\r\n";
    emailBody += "\r\n";
    emailBody += "Bonjour,\r\n\r\n";
    emailBody += "Votre code de vérification TuniWaste est :\r\n\r\n";
    emailBody += "    " + code + "\r\n\r\n";
    emailBody += "Ce code est valable 5 minutes.\r\n\r\n";
    emailBody += "Si vous n'avez pas demandé ce code, ignorez cet email.\r\n\r\n";
    emailBody += "Cordialement,\r\nL'équipe TuniWaste\r\n";
    emailBody += "\r\n.\r\n";

    socket.write(emailBody.toUtf8());
    socket.flush();
    QString dataResp = readResponse();
    qDebug() << "[SMTP] DATA réponse :" << dataResp.trimmed();

    send("QUIT");
    socket.disconnectFromHost();

    bool success = dataResp.startsWith("250");
    qDebug() << "[OTP] Email envoyé :" << (success ? "OUI" : "NON");
    return success;
}

// ══════════════════════════════════════════════════════════════════════════════
//  OtpDialog — Interface de saisie du code OTP
// ══════════════════════════════════════════════════════════════════════════════
OtpDialog::OtpDialog(const QString &email, QWidget *parent)
    : QDialog(parent), targetEmail(email)
{
    setWindowTitle("Vérification en deux étapes");
    setModal(true);
    setFixedSize(450, 380);
    setStyleSheet("QDialog { background-color: #FFFFFF; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(40, 35, 40, 35);

    // Icône + titre
    QLabel *iconLabel = new QLabel("🔐");
    iconLabel->setStyleSheet("font-size: 48px; background: transparent;");
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);

    QLabel *titleLabel = new QLabel("Vérification en deux étapes");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #2E5D28;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // Message explicatif
    QString shortEmail = email.length() > 25
        ? email.left(6) + "***" + email.mid(email.indexOf('@'))
        : email;
    QLabel *infoLabel = new QLabel(
        QString("Un code à 6 chiffres a été envoyé à\n%1").arg(shortEmail)
    );
    infoLabel->setStyleSheet("font-size: 13px; color: #666666; background: transparent;");
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    // Champ de saisie OTP
    otpInput = new QLineEdit();
    otpInput->setPlaceholderText("_ _ _ _ _ _");
    otpInput->setMaxLength(6);
    otpInput->setFixedHeight(56);
    otpInput->setAlignment(Qt::AlignCenter);
    otpInput->setStyleSheet(
        "QLineEdit { padding: 10px; border: 2px solid #D4EDDA; border-radius: 10px;"
        " font-size: 28px; font-weight: bold; color: #2E5D28; letter-spacing: 12px; }"
        "QLineEdit:focus { border: 2px solid #6FA85E; }"
    );
    layout->addWidget(otpInput);

    // Statut / erreur
    statusLabel = new QLabel("");
    statusLabel->setStyleSheet("font-size: 13px; color: #CC0000; background: transparent;");
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);

    // Timer
    timerLabel = new QLabel("⏱ Code valide : 5:00");
    timerLabel->setStyleSheet("font-size: 12px; color: #6FA85E; background: transparent;");
    timerLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(timerLabel);

    // Boutons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    resendBtn = new QPushButton("🔄 Renvoyer");
    resendBtn->setFixedHeight(44);
    resendBtn->setStyleSheet(
        "QPushButton { background-color: #F5F5F5; color: #3B6B35; border: 2px solid #D4EDDA;"
        " border-radius: 8px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #E8F5E3; }"
    );
    connect(resendBtn, &QPushButton::clicked, this, &OtpDialog::onResendClicked);
    btnLayout->addWidget(resendBtn);

    QPushButton *verifyBtn = new QPushButton("✅ Vérifier");
    verifyBtn->setFixedHeight(44);
    verifyBtn->setStyleSheet(
        "QPushButton { background-color: #6FA85E; color: #FFFFFF; border: none;"
        " border-radius: 8px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #5A9048; }"
    );
    connect(verifyBtn, &QPushButton::clicked, this, &OtpDialog::onVerifyClicked);
    connect(otpInput, &QLineEdit::returnPressed, this, &OtpDialog::onVerifyClicked);
    btnLayout->addWidget(verifyBtn);

    layout->addLayout(btnLayout);

    // Timer countdown
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &OtpDialog::updateTimer);
    countdownTimer->start(1000);

    otpInput->setFocus();
}

void OtpDialog::onVerifyClicked()
{
    QString code = otpInput->text().trimmed();

    if (code.length() != 6) {
        statusLabel->setText("⚠️ Veuillez entrer les 6 chiffres.");
        return;
    }

    if (!OtpManager::getInstance().isOtpValid()) {
        statusLabel->setText("❌ Code expiré. Cliquez sur 'Renvoyer'.");
        return;
    }

    if (OtpManager::getInstance().verifyOtp(code)) {
        verified = true;
        countdownTimer->stop();
        accept();
    } else {
        statusLabel->setText("❌ Code incorrect. Réessayez.");
        otpInput->clear();
        otpInput->setFocus();
        otpInput->setStyleSheet(
            "QLineEdit { padding: 10px; border: 2px solid #CC0000; border-radius: 10px;"
            " font-size: 28px; font-weight: bold; color: #2E5D28; letter-spacing: 12px; }"
        );
    }
}

void OtpDialog::onResendClicked()
{
    resendBtn->setEnabled(false);
    resendBtn->setText("Envoi...");
    statusLabel->setStyleSheet("font-size: 13px; color: #3B6B35; background: transparent;");
    statusLabel->setText("📤 Envoi du code en cours...");

    bool sent = OtpManager::getInstance().sendOtp(targetEmail);

    if (sent) {
        secondsLeft = 300;
        statusLabel->setText("✅ Nouveau code envoyé !");
        otpInput->clear();
        otpInput->setStyleSheet(
            "QLineEdit { padding: 10px; border: 2px solid #D4EDDA; border-radius: 10px;"
            " font-size: 28px; font-weight: bold; color: #2E5D28; letter-spacing: 12px; }"
        );
    } else {
        statusLabel->setStyleSheet("font-size: 13px; color: #CC0000; background: transparent;");
        statusLabel->setText("❌ Échec d'envoi. Vérifiez la connexion réseau.");
    }

    resendBtn->setEnabled(true);
    resendBtn->setText("🔄 Renvoyer");
}

void OtpDialog::updateTimer()
{
    secondsLeft--;
    if (secondsLeft <= 0) {
        countdownTimer->stop();
        timerLabel->setText("⏱ Code expiré");
        timerLabel->setStyleSheet("font-size: 12px; color: #CC0000; background: transparent;");
        return;
    }
    int minutes = secondsLeft / 60;
    int seconds = secondsLeft % 60;
    timerLabel->setText(QString("⏱ Code valide : %1:%2")
        .arg(minutes)
        .arg(seconds, 2, 10, QChar('0')));
}
