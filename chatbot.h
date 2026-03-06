#ifndef CHATBOT_H
#define CHATBOT_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMap>
#include <QTimer>
#include <QMouseEvent>
#include <QRegularExpression>

class ChatBot;

class ChatBubbleButton : public QWidget
{
    Q_OBJECT
public:
    explicit ChatBubbleButton(QWidget *parent = nullptr);
    void reposition();
    void triggerOpen();
private slots:
    void onBubbleClicked();
private:
    QPushButton *bubbleBtn  = nullptr;
    ChatBot     *chatWindow = nullptr;
    bool         chatOpen   = false;
};

class ChatBot : public QWidget
{
    Q_OBJECT
public:
    explicit ChatBot();   // PAS de parent
protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
private slots:
    void onSendClicked();
    void onTypingTimer();
    void onResponseReady();
private:
    void setupUI();
    void initRules();
    void addMessage(const QString &text, bool isUser);
    void showTypingIndicator();
    void hideTypingIndicator();
    QString getResponse(const QString &input);
    void addQuickReply(const QString &text);

    QScrollArea *scrollArea        = nullptr;
    QWidget     *messagesContainer = nullptr;
    QVBoxLayout *messagesLayout    = nullptr;
    QLineEdit   *inputEdit         = nullptr;
    QPushButton *sendBtn           = nullptr;
    QHBoxLayout *quickRepliesLayout= nullptr;

    bool   m_dragging  = false;
    QPoint m_dragOffset;

    QWidget *typingBubble = nullptr;
    QLabel  *typingLabel  = nullptr;
    QTimer  *typingTimer  = nullptr;
    QTimer  *responseTimer= nullptr;
    int      typingDots   = 0;
    QString  pendingResponse;

    QMap<QString,QString> rules;
};

#endif
