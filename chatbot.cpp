#include "chatbot.h"
#include <QScrollBar>
#include <QScreen>
#include <QApplication>
#include <QSqlQuery>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QFrame>

// ══════════════════════════════════════════════════════════════════════════════
//  ChatBubbleButton
// ══════════════════════════════════════════════════════════════════════════════
ChatBubbleButton::ChatBubbleButton(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::SubWindow);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(68, 68);

    bubbleBtn = new QPushButton("💬", this);
    bubbleBtn->setFixedSize(64, 64);
    bubbleBtn->setCursor(Qt::PointingHandCursor);
    bubbleBtn->setToolTip("Assistant TuniWaste");
    bubbleBtn->setStyleSheet(
        "QPushButton {"
        "  background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #4CAF50,stop:1 #2E7D32);"
        "  color:white; font-size:26px; border:none; border-radius:32px;"
        "}"
        "QPushButton:hover{background-color:#388E3C;}"
        "QPushButton:pressed{background-color:#1B5E20;}"
        );
    connect(bubbleBtn, &QPushButton::clicked, this, &ChatBubbleButton::onBubbleClicked);
    reposition();
    show();
    raise();
}

void ChatBubbleButton::reposition()
{
    if (parentWidget())
        move(parentWidget()->width() - 80, 135);
}

void ChatBubbleButton::triggerOpen() { onBubbleClicked(); }

void ChatBubbleButton::onBubbleClicked()
{
    if (!chatWindow)
        chatWindow = new ChatBot();   // ← pas de parent

    if (!chatOpen) {
        QPoint gp  = mapToGlobal(QPoint(0,0));
        int cx = gp.x() - 440 + width();
        int cy = gp.y() + 75;
        QRect sg = QApplication::primaryScreen()->availableGeometry();
        if (cx < sg.left())        cx = sg.left() + 10;
        if (cx+440 > sg.right())   cx = sg.right() - 450;
        if (cy+640 > sg.bottom())  cy = gp.y() - 650;
        if (cy < sg.top())         cy = sg.top() + 10;

        chatWindow->move(cx, cy);
        chatWindow->show();
        chatWindow->raise();

        bubbleBtn->setText("✕");
        bubbleBtn->setStyleSheet(
            "QPushButton{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #EF5350,stop:1 #B71C1C);"
            "color:white;font-size:20px;font-weight:bold;border:none;border-radius:32px;}"
            "QPushButton:hover{background-color:#C62828;}"
            );
        chatOpen = true;
    } else {
        chatWindow->hide();
        bubbleBtn->setText("💬");
        bubbleBtn->setStyleSheet(
            "QPushButton{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #4CAF50,stop:1 #2E7D32);"
            "color:white;font-size:26px;border:none;border-radius:32px;}"
            "QPushButton:hover{background-color:#388E3C;}"
            );
        chatOpen = false;
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  ChatBot  — QWidget SANS parent, flags simples
// ══════════════════════════════════════════════════════════════════════════════
ChatBot::ChatBot() : QWidget(nullptr,
              Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setFixedSize(440, 640);

    typingTimer    = new QTimer(this);
    responseTimer  = new QTimer(this);
    typingTimer->setInterval(450);
    responseTimer->setSingleShot(true);
    connect(typingTimer,  &QTimer::timeout, this, &ChatBot::onTypingTimer);
    connect(responseTimer,&QTimer::timeout, this, &ChatBot::onResponseReady);

    setupUI();
    initRules();

    addMessage(
        "👋 Bonjour ! Je suis l'Assistant TuniWaste.\n\n"
        "Je peux vous aider avec :\n\n"
        "➕  Ajouter un utilisateur\n"
        "✏️  Modifier un utilisateur\n"
        "🗑️  Supprimer un utilisateur\n"
        "🔍  Rechercher & filtrer\n"
        "🔑  Mots de passe & sécurité\n"
        "📄  Exporter en PDF\n"
        "📊  Statistiques en temps réel\n\n"
        "Tapez votre question ci-dessous 👇",
        false
        );
}

void ChatBot::setupUI()
{
    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(8,8,8,8);
    outer->setSpacing(0);

    QWidget *card = new QWidget();
    card->setObjectName("chatCard");
    card->setStyleSheet(
        "QWidget#chatCard{background-color:#FAFAFA;border-radius:20px;border:1px solid rgba(0,0,0,0.08);}"
        );
    QGraphicsDropShadowEffect *sh = new QGraphicsDropShadowEffect();
    sh->setBlurRadius(30); sh->setOffset(0,6); sh->setColor(QColor(0,0,0,55));
    card->setGraphicsEffect(sh);
    outer->addWidget(card);

    QVBoxLayout *mainL = new QVBoxLayout(card);
    mainL->setContentsMargins(0,0,0,0);
    mainL->setSpacing(0);

    // Header
    QWidget *hdr = new QWidget();
    hdr->setFixedHeight(68);
    hdr->setStyleSheet(
        "QWidget{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #2E7D32,stop:1 #1B5E20);"
        "border-top-left-radius:20px;border-top-right-radius:20px;}"
        );
    QHBoxLayout *hdrL = new QHBoxLayout(hdr);
    hdrL->setContentsMargins(16,0,16,0); hdrL->setSpacing(12);

    QLabel *av = new QLabel("🤖");
    av->setFixedSize(42,42); av->setAlignment(Qt::AlignCenter);
    av->setStyleSheet("background-color:rgba(255,255,255,0.15);border-radius:21px;font-size:22px;border:2px solid rgba(255,255,255,0.3);");
    hdrL->addWidget(av);

    QWidget *ti = new QWidget(); ti->setStyleSheet("background:transparent;");
    QVBoxLayout *tiL = new QVBoxLayout(ti); tiL->setSpacing(2); tiL->setContentsMargins(0,0,0,0);
    QLabel *t1 = new QLabel("Assistant TuniWaste");
    t1->setStyleSheet("font-size:15px;font-weight:bold;color:#FFFFFF;background:transparent;");
    QLabel *t2 = new QLabel("🟢  En ligne");
    t2->setStyleSheet("font-size:11px;color:rgba(255,255,255,0.75);background:transparent;");
    tiL->addWidget(t1); tiL->addWidget(t2);
    hdrL->addWidget(ti); hdrL->addStretch();

    QPushButton *minB = new QPushButton("—");
    minB->setFixedSize(28,28); minB->setCursor(Qt::PointingHandCursor);
    minB->setStyleSheet(
        "QPushButton{background-color:rgba(255,255,255,0.15);color:white;font-size:14px;font-weight:bold;border:none;border-radius:14px;}"
        "QPushButton:hover{background-color:rgba(255,255,255,0.3);}"
        );
    connect(minB, &QPushButton::clicked, this, &QWidget::hide);
    hdrL->addWidget(minB);
    mainL->addWidget(hdr);

    // Messages
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(
        "QScrollArea{background-color:#F5F5F5;border:none;}"
        "QScrollBar:vertical{background:transparent;width:5px;}"
        "QScrollBar::handle:vertical{background:rgba(46,125,50,0.4);border-radius:3px;min-height:20px;}"
        "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0px;}"
        );
    messagesContainer = new QWidget();
    messagesContainer->setStyleSheet("background-color:#F5F5F5;");
    messagesLayout = new QVBoxLayout(messagesContainer);
    messagesLayout->setContentsMargins(14,14,14,14);
    messagesLayout->setSpacing(10);
    messagesLayout->addStretch();
    scrollArea->setWidget(messagesContainer);
    mainL->addWidget(scrollArea, 1);

    QFrame *sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color:rgba(0,0,0,0.07);");
    mainL->addWidget(sep);

    // Quick replies
    QWidget *qrW = new QWidget(); qrW->setStyleSheet("background-color:#FAFAFA;");
    quickRepliesLayout = new QHBoxLayout(qrW);
    quickRepliesLayout->setContentsMargins(10,6,10,4); quickRepliesLayout->setSpacing(6);
    QStringList row1 = {"➕ Ajouter","✏️ Modifier","🗑️ Supprimer","📊 Stats","🔑 MDP"};
    for (const QString &t : row1)
        addQuickReply(t);
    quickRepliesLayout->addStretch();
    mainL->addWidget(qrW);

    // Deuxième rangée — emails par rôle
    QWidget *qrW2 = new QWidget(); qrW2->setStyleSheet("background-color:#FAFAFA;");
    quickRepliesLayout = new QHBoxLayout(qrW2);
    quickRepliesLayout->setContentsMargins(10,2,10,6); quickRepliesLayout->setSpacing(6);
    QStringList row2 = {"📧 Admin","📧 Chauffeur","📧 Camion","📧 Recyclage","📧 Zone"};
    for (const QString &t : row2)
        addQuickReply(t);
    quickRepliesLayout->addStretch();
    mainL->addWidget(qrW2);

    // Input
    QWidget *inp = new QWidget();
    inp->setFixedHeight(64);
    inp->setStyleSheet(
        "QWidget{background-color:#FFFFFF;border-bottom-left-radius:20px;border-bottom-right-radius:20px;"
        "border-top:1px solid rgba(0,0,0,0.06);}"
        );
    QHBoxLayout *inpL = new QHBoxLayout(inp);
    inpL->setContentsMargins(14,10,14,10); inpL->setSpacing(8);

    inputEdit = new QLineEdit();
    inputEdit->setPlaceholderText("Écrivez votre message...");
    inputEdit->setFixedHeight(40);
    inputEdit->setStyleSheet(
        "QLineEdit{padding:8px 16px;border:1.5px solid rgba(0,0,0,0.1);border-radius:20px;"
        "background-color:#F5F5F5;color:#212121;font-size:13px;}"
        "QLineEdit:focus{border:1.5px solid #4CAF50;background-color:#FFFFFF;}"
        );
    connect(inputEdit, &QLineEdit::returnPressed, this, &ChatBot::onSendClicked);
    inpL->addWidget(inputEdit, 1);

    sendBtn = new QPushButton("➤");
    sendBtn->setFixedSize(40,40); sendBtn->setCursor(Qt::PointingHandCursor);
    sendBtn->setStyleSheet(
        "QPushButton{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #4CAF50,stop:1 #2E7D32);"
        "color:white;font-size:16px;border:none;border-radius:20px;}"
        "QPushButton:hover{background-color:#388E3C;}"
        "QPushButton:pressed{background-color:#1B5E20;}"
        );
    connect(sendBtn, &QPushButton::clicked, this, &ChatBot::onSendClicked);
    inpL->addWidget(sendBtn);
    mainL->addWidget(inp);
}

void ChatBot::addQuickReply(const QString &text)
{
    QPushButton *btn = new QPushButton(text);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedHeight(30);
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    btn->setStyleSheet(
        "QPushButton{background-color:rgba(46,125,50,0.08);color:#2E7D32;"
        "border:1.5px solid rgba(46,125,50,0.3);border-radius:14px;"
        "padding:2px 10px;font-size:11px;font-weight:bold;}"
        "QPushButton:hover{background-color:rgba(46,125,50,0.2);}"
        );
    connect(btn, &QPushButton::clicked, [this, text](){
        // Mapping bouton → phrase naturelle pour getResponse
        QMap<QString,QString> mapping = {
                                          {"➕ Ajouter",    "ajouter"},
                                          {"✏️ Modifier",   "modifier"},
                                          {"🗑️ Supprimer", "supprimer"},
                                          {"📊 Stats",      "statistiques"},
                                          {"🔑 MDP",        "mot de passe"},
                                          {"📧 Admin",      "email des administrateurs"},
                                          {"📧 Chauffeur",  "email des chauffeurs"},
                                          {"📧 Camion",     "email des responsable camion"},
                                          {"📧 Recyclage",  "email des responsable recyclage"},
                                          {"📧 Zone",       "email des responsable zone"},
                                          };
        QString msg = mapping.contains(text) ? mapping[text] : text;
        inputEdit->setText(msg);
        onSendClicked();
    });
    quickRepliesLayout->addWidget(btn);
}

void ChatBot::addMessage(const QString &text, bool isUser)
{
    QWidget *row = new QWidget(); row->setStyleSheet("background:transparent;");
    QHBoxLayout *rowL = new QHBoxLayout(row);
    rowL->setContentsMargins(0,2,0,2); rowL->setSpacing(8);

    if (isUser) {
        rowL->addStretch();
        QLabel *b = new QLabel(text); b->setWordWrap(true); b->setMaximumWidth(340);
        b->setTextInteractionFlags(Qt::TextSelectableByMouse);
        b->setStyleSheet(
            "QLabel{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #43A047,stop:1 #2E7D32);"
            "color:#FFFFFF;border-radius:18px;border-bottom-right-radius:4px;padding:10px 14px;font-size:13px;}"
            );
        rowL->addWidget(b);
        QLabel *av = new QLabel("👤"); av->setFixedSize(34,34); av->setAlignment(Qt::AlignCenter);
        av->setStyleSheet("background-color:#E8F5E9;border-radius:17px;font-size:16px;");
        rowL->addWidget(av);
    } else {
        QLabel *av = new QLabel("🤖"); av->setFixedSize(34,34); av->setAlignment(Qt::AlignCenter);
        av->setStyleSheet("background-color:#E8F5E9;border-radius:17px;font-size:16px;border:1.5px solid #A5D6A7;");
        rowL->addWidget(av);
        QLabel *b = new QLabel(text); b->setWordWrap(true); b->setMaximumWidth(340);
        b->setTextInteractionFlags(Qt::TextSelectableByMouse);
        b->setStyleSheet(
            "QLabel{background-color:#FFFFFF;color:#212121;border-radius:18px;border-top-left-radius:4px;"
            "padding:10px 14px;font-size:13px;border:1px solid rgba(0,0,0,0.06);}"
            );
        rowL->addWidget(b);
        rowL->addStretch();
    }

    messagesLayout->insertWidget(messagesLayout->count()-1, row);
    QTimer::singleShot(60, this, [this](){
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
    });
}

void ChatBot::showTypingIndicator()
{
    typingBubble = new QWidget(); typingBubble->setStyleSheet("background:transparent;");
    QHBoxLayout *tl = new QHBoxLayout(typingBubble);
    tl->setContentsMargins(0,2,0,2); tl->setSpacing(8);
    QLabel *av = new QLabel("🤖"); av->setFixedSize(34,34); av->setAlignment(Qt::AlignCenter);
    av->setStyleSheet("background-color:#E8F5E9;border-radius:17px;font-size:16px;border:1.5px solid #A5D6A7;");
    tl->addWidget(av);
    typingLabel = new QLabel("● ● ●");
    typingLabel->setStyleSheet(
        "QLabel{background-color:#FFFFFF;color:#4CAF50;border-radius:18px;border-top-left-radius:4px;"
        "padding:10px 18px;font-size:14px;font-weight:bold;letter-spacing:4px;border:1px solid rgba(0,0,0,0.06);}"
        );
    tl->addWidget(typingLabel); tl->addStretch();
    messagesLayout->insertWidget(messagesLayout->count()-1, typingBubble);
    typingTimer->start();
    QTimer::singleShot(60, this, [this](){
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
    });
}

void ChatBot::hideTypingIndicator()
{
    typingTimer->stop();
    if (typingBubble) {
        messagesLayout->removeWidget(typingBubble);
        typingBubble->deleteLater();
        typingBubble = nullptr; typingLabel = nullptr;
    }
}

void ChatBot::onTypingTimer()
{
    if (!typingLabel) return;
    typingDots = (typingDots+1)%4;
    QString d;
    for (int i=0;i<3;i++) d += (i<typingDots)?"● ":"○ ";
    typingLabel->setText(d.trimmed());
}

void ChatBot::onResponseReady()
{
    hideTypingIndicator();
    addMessage(pendingResponse, false);
    pendingResponse.clear();
}

void ChatBot::mousePressEvent(QMouseEvent *e)
{
    if (e->button()==Qt::LeftButton && e->pos().y()<=68){
        m_dragging=true;
        m_dragOffset=e->globalPosition().toPoint()-frameGeometry().topLeft();
        setCursor(Qt::ClosedHandCursor); e->accept(); return;
    }
    QWidget::mousePressEvent(e);
}
void ChatBot::mouseMoveEvent(QMouseEvent *e)
{
    if (m_dragging&&(e->buttons()&Qt::LeftButton)){
        move(e->globalPosition().toPoint()-m_dragOffset); e->accept(); return;
    }
    QWidget::mouseMoveEvent(e);
}
void ChatBot::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_dragging){ m_dragging=false; setCursor(Qt::ArrowCursor); e->accept(); return; }
    QWidget::mouseReleaseEvent(e);
}

void ChatBot::onSendClicked()
{
    QString text = inputEdit->text().trimmed();
    if (text.isEmpty()) return;
    addMessage(text, true);
    inputEdit->clear();
    inputEdit->setFocus();
    pendingResponse = getResponse(text);
    showTypingIndicator();
    int delay = 800 + (pendingResponse.length()/8)*10;
    if (delay>2200) delay=2200;
    responseTimer->start(delay);
}

void ChatBot::initRules()
{
    rules["ajouter"] =
        "➕  Ajouter un utilisateur\n\n"
        "1️⃣  Cliquez sur  ＋ Nouvel Utilisateur\n"
        "2️⃣  Remplissez les champs :\n"
        "     • Nom & Prénom\n"
        "     • Email (unique)\n"
        "     • Téléphone (8 chiffres)\n"
        "     • Mot de passe\n"
        "     • Rôle, Ville, Code Postal\n"
        "     • Sexe & Photo\n"
        "3️⃣  Cliquez  ✅ Enregistrer";
    rules["créer"]=rules["creer"]=rules["nouveau"]=rules["create"]=rules["add"]=rules["ajouter"];

    rules["modifier"] =
        "✏️  Modifier un utilisateur\n\n"
        "1️⃣  Sélectionnez la ligne\n"
        "2️⃣  Cliquez sur  ✏️ Modifier\n"
        "3️⃣  Modifiez les champs\n"
        "4️⃣  Cliquez  💾 Sauvegarder";
    rules["update"]=rules["editer"]=rules["changer"]=rules["modifier"];

    rules["supprimer"] =
        "🗑️  Supprimer un utilisateur\n\n"
        "1️⃣  Sélectionnez l'utilisateur\n"
        "2️⃣  Cliquez sur  🗑️ Supprimer\n"
        "3️⃣  Confirmez la suppression\n\n"
        "⚠️  La suppression est définitive !";
    rules["delete"]=rules["effacer"]=rules["enlever"]=rules["supprimer"];

    rules["rechercher"] =
        "🔍  Rechercher\n\n"
        "• Barre de recherche → nom, prénom, email\n"
        "• Filtre par rôle, ville, sexe\n"
        "• Résultats en temps réel\n"
        "• Bouton Réinitialiser pour tout effacer";
    rules["filtrer"]=rules["chercher"]=rules["search"]=rules["rechercher"];

    rules["mot de passe"] =
        "🔑  Mot de passe\n\n"
        "🔒  Règles :\n"
        "     • Minimum 8 caractères\n"
        "     • Majuscule + chiffre requis\n\n"
        "🔄  Oublié ?\n"
        "     Clic Mot de passe oublié\n"
        "     → entrez email → reçu par email";
    rules["password"]=rules["mdp"]=rules["oublie"]=rules["mot de passe"];

    rules["connexion"] =
        "🔐  Se connecter\n\n"
        "1️⃣  Email + Mot de passe\n"
        "2️⃣  Se connecter\n"
        "3️⃣  Code OTP reçu par email\n"
        "4️⃣  Entrez le code → Accès ✅\n\n"
        "⏱️  Code expire après 5 minutes";
    rules["login"]=rules["connecter"]=rules["connexion"];

    rules["otp"] =
        "📧  Code OTP\n\n"
        "• 6 chiffres envoyés par email\n"
        "• Valable 5 minutes\n"
        "• Cliquez Renvoyer si expiré\n"
        "• Vérifiez vos spams";
    rules["code"]=rules["verification"]=rules["otp"];

    rules["pdf"] =
        "📄  Exporter en PDF\n\n"
        "1️⃣  Cliquez  📄 Exporter PDF\n"
        "2️⃣  Choisissez l'emplacement\n"
        "3️⃣  Fichier généré automatiquement\n\n"
        "📋  Contient tableau complet + stats";
    rules["exporter"]=rules["export"]=rules["imprimer"]=rules["pdf"];

    rules["role"] =
        "👥  Rôles disponibles\n\n"
        "🟠  Administrateur — accès complet\n"
        "🔵  Chauffeur — collecte terrain\n"
        "🟣  Responsable Camion — véhicules\n"
        "🟢  Responsable Recyclage — tri\n"
        "🔴  Responsable Zone — zones";
    rules["rôle"]=rules["admin"]=rules["chauffeur"]=rules["role"];

    rules["photo"] =
        "📷  Photo de profil\n\n"
        "1️⃣  Formulaire → 📂 Parcourir\n"
        "2️⃣  Formats : JPG, PNG, BMP\n"
        "3️⃣  Chemin sauvegardé en base";
    rules["image"]=rules["avatar"]=rules["photo"];

    rules["aide"] =
        "❓  Sujets disponibles\n\n"
        "➕ ajouter    ✏️ modifier\n"
        "🗑️ supprimer  🔍 rechercher\n"
        "🔑 mot de passe  📄 pdf\n"
        "👥 rôle  🔐 connexion\n"
        "📊 statistiques / combien\n\n"
        "Tapez un mot-clé !";
    rules["help"]=rules["aide"];

    rules["bonjour"]="👋  Bonjour ! Comment puis-je vous aider ?\nTapez  aide  pour voir tous les sujets.";
    rules["salut"]=rules["hello"]=rules["hi"]=rules["bonjour"];
    rules["merci"]="🙏  Avec plaisir ! Autre chose ?";
    rules["ok"]="👍  Parfait ! Autre chose ?";

    rules["email"] =
        "📧  Emails par rôle\n\n"
        "Posez-moi une question comme :\n\n"
        "• email des chauffeurs\n"
        "• email des administrateurs\n"
        "• email des responsable camion\n"
        "• email des responsable recyclage\n"
        "• email des responsable zone\n"
        "• tous les emails\n\n"
        "Je vous affiche les noms et emails\n"
        "en temps réel depuis la base !";
    rules["mail"]    = rules["email"];
    rules["contact"] = rules["email"];
}

QString ChatBot::getResponse(const QString &input)
{
    QString lower = input.toLower().trimmed();

    // ── Requête emails par rôle ───────────────────────────────────────────────
    bool wantsEmail = lower.contains("email") || lower.contains("mail")
                      || lower.contains("contact") || lower.contains("adresse")
                      || lower.contains("@");

    bool asksForRole = lower.contains("responsable") || lower.contains("chauffeur")
                       || lower.contains("admin")        || lower.contains("camion")
                       || lower.contains("recyclage")    || lower.contains("zone");

    // Si rôle mentionné avec mot lié à email → activer wantsEmail
    if (asksForRole && (lower.contains("email") || lower.contains("mail")
                        || lower.contains("donner") || lower.contains("donne")
                        || lower.contains("liste")  || lower.contains("afficher")
                        || lower.contains("voir")   || lower.contains("show")
                        || lower.contains("contact")|| lower.contains("adresse")))
        wantsEmail = true;

    // Détection directe : "responsable camion email" ou "email responsable zone"
    if (lower.contains("responsable") && lower.contains("camion"))    wantsEmail = true;
    if (lower.contains("responsable") && lower.contains("zone"))      wantsEmail = true;
    if (lower.contains("responsable") && lower.contains("recyclage")) wantsEmail = true;

    auto emailsParRole = [](const QString &role, const QString &emoji, const QString &label) -> QString {
        QSqlQuery q;
        q.prepare("SELECT NOM, PRENOM, EMAIL FROM UTILISATEUR WHERE ROLE=:role ORDER BY NOM");
        q.bindValue(":role", role);
        if (!q.exec()) return "❌  Erreur d'accès à la base de données.";
        QString result = emoji + "  Emails — " + label + "\n\n";
        int count = 0;
        while (q.next()) {
            result += "👤 " + q.value(0).toString() + " " + q.value(1).toString() + "\n";
            result += "   📧 " + q.value(2).toString() + "\n\n";
            count++;
        }
        if (count == 0)
            result += "Aucun utilisateur trouvé pour ce rôle.";
        return result.trimmed();
    };

    auto tousLesEmails = [&emailsParRole]() -> QString {
        QString result = "📧  Tous les emails\n";
        result += "──────────────────────────\n\n";
        result += emailsParRole("Administrateur",    "🟠", "Administrateurs") + "\n\n";
        result += "──────────────────────────\n\n";
        result += emailsParRole("Chauffeur",         "🔵", "Chauffeurs") + "\n\n";
        result += "──────────────────────────\n\n";
        result += emailsParRole("Responsable Camion","🟣", "Resp. Camion") + "\n\n";
        result += "──────────────────────────\n\n";
        result += emailsParRole("Responsable Recyclage","🟢","Resp. Recyclage") + "\n\n";
        result += "──────────────────────────\n\n";
        result += emailsParRole("Responsable Zone",  "🔴", "Resp. Zone");
        return result;
    };

    if (wantsEmail) {
        // Vérifier les rôles composés EN PREMIER (avant "zone", "camion" seuls)
        if (lower.contains("responsable") && lower.contains("camion"))
            return emailsParRole("Responsable Camion",    "🟣", "Responsables Camion");
        if (lower.contains("responsable") && lower.contains("recyclage"))
            return emailsParRole("Responsable Recyclage", "🟢", "Responsables Recyclage");
        if (lower.contains("responsable") && lower.contains("zone"))
            return emailsParRole("Responsable Zone",      "🔴", "Responsables Zone");
        // Rôles simples
        if (lower.contains("admin"))
            return emailsParRole("Administrateur",        "🟠", "Administrateurs");
        if (lower.contains("chauffeur"))
            return emailsParRole("Chauffeur",             "🔵", "Chauffeurs");
        if (lower.contains("camion"))
            return emailsParRole("Responsable Camion",    "🟣", "Responsables Camion");
        if (lower.contains("recyclage"))
            return emailsParRole("Responsable Recyclage", "🟢", "Responsables Recyclage");
        if (lower.contains("zone"))
            return emailsParRole("Responsable Zone",      "🔴", "Responsables Zone");
        // Aucun rôle précisé → tous
        return tousLesEmails();
    }

    // ── Comptage ──────────────────────────────────────────────────────────────
    bool wantsCount = lower.contains("combien")||lower.contains("nombre")
                      ||lower.contains("stats")||lower.contains("statistic")
                      ||lower.contains("total")||lower.contains("répartition");

    auto countRole=[](const QString &role)->int{
        QSqlQuery q;
        q.prepare("SELECT COUNT(*) FROM UTILISATEUR WHERE ROLE=:role");
        q.bindValue(":role",role);
        return (q.exec()&&q.next())?q.value(0).toInt():-1;
    };
    auto countAll=[]()->int{
        QSqlQuery q;
        return (q.exec("SELECT COUNT(*) FROM UTILISATEUR")&&q.next())?q.value(0).toInt():-1;
    };

    if (wantsCount) {
        if (lower.contains("chauffeur")){int n=countRole("Chauffeur");return n>=0?QString("🔵  Chauffeurs : %1").arg(n):"❌ Erreur BD";}
        if (lower.contains("admin")){int n=countRole("Administrateur");return n>=0?QString("🟠  Administrateurs : %1").arg(n):"❌ Erreur BD";}
        if (lower.contains("camion")){int n=countRole("Responsable Camion");return n>=0?QString("🟣  Resp. Camion : %1").arg(n):"❌ Erreur BD";}
        if (lower.contains("recyclage")){int n=countRole("Responsable Recyclage");return n>=0?QString("🟢  Resp. Recyclage : %1").arg(n):"❌ Erreur BD";}
        if (lower.contains("zone")){int n=countRole("Responsable Zone");return n>=0?QString("🔴  Resp. Zone : %1").arg(n):"❌ Erreur BD";}
        int total=countAll(),admin=countRole("Administrateur"),chauf=countRole("Chauffeur");
        int camion=countRole("Responsable Camion"),recyl=countRole("Responsable Recyclage"),zone=countRole("Responsable Zone");
        if(total<0) return "❌  Impossible d'accéder à la base de données.";
        return QString("📊  Statistiques\n\n👥 Total : %1\n🟠 Admin : %2\n🔵 Chauffeurs : %3\n🟣 Resp. Camion : %4\n🟢 Resp. Recyclage : %5\n🔴 Resp. Zone : %6")
            .arg(total).arg(admin).arg(chauf).arg(camion).arg(recyl).arg(zone);
    }

    for (auto it=rules.constBegin();it!=rules.constEnd();++it)
        if (lower.contains(it.key())) return it.value();

    return "🤔  Je n'ai pas compris.\n\nEssayez : ajouter • modifier • supprimer\nrechercher • pdf • mot de passe • aide";
}
