#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>


class Dashboard : public QWidget {
  Q_OBJECT

public:
  explicit Dashboard(QWidget *parent = nullptr);

private:
  void setupUI();
  void createStatsCards();
  void createRecentActivity();
  void createQuickActions();
  void applyStyles();

  QWidget *createStatCard(const QString &title, const QString &value,
                          const QString &icon, const QString &color);

  // UI Components
  QWidget *statsContainer;
  QWidget *activityContainer;
  QWidget *actionsContainer;

  // Stat labels (for updating)
  QLabel *totalUsersLabel;
  QLabel *totalBinsLabel;
  QLabel *totalTrucksLabel;
  QLabel *totalZonesLabel;
};

#endif // DASHBOARD_H
