#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTableWidget>
#include <QVector>
#include <QPainter>

// ══════════════════════════════════════════════════════════════
//  DonutChart
// ══════════════════════════════════════════════════════════════
class DonutChart : public QWidget
{
    Q_OBJECT
public:
    struct Slice { QString label; double value; QColor color; };
    explicit DonutChart(QWidget *parent = nullptr);
    void setSlices(const QVector<Slice> &slices);
    void setCenterText(const QString &text);
    QSize sizeHint() const override { return QSize(160, 160); }
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QVector<Slice> m_slices;
    QString        m_center;
};

// ══════════════════════════════════════════════════════════════
//  MainWindow
// ══════════════════════════════════════════════════════════════
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSave();
    void onModify(int row);
    void onDelete(int row);
    void onSearchChanged(const QString &t);
    void onStateFilterChanged(int i);
    void onTypeFilterChanged(int i);
    void onExportPDF();
    void onMenuClicked(int i);

private:
    // ── Widgets (créés dans buildUI()) ───────────────────────
    QLabel          *logoIcon;
    QLabel          *formTitleLbl;
    QLineEdit       *idCamionInput;
    QLineEdit       *locInput;
    QLineEdit       *typeDechetInput;
    QSpinBox        *capacityInput;
    QSpinBox        *fillInput;
    QLineEdit       *stateInput;
    QLineEdit       *zoneInput;
    QLineEdit       *driverInput;
    QLineEdit       *phoneInput;
    QPushButton     *saveBtn;
    QTableWidget    *tableWidget;
    QLineEdit       *searchBar;
    QComboBox       *stateFilter;
    QComboBox       *typeFilter;
    QPushButton     *exportBtn;
    DonutChart      *stateChart;
    QLabel          *stateLegend;
    DonutChart      *typeChart;
    QLabel          *typeLegend;
    DonutChart      *levelChart;
    QLabel          *levelSubLabel;
    QVector<QPushButton*> menuBtns;

    // ── Base de données ───────────────────────────────────────
    bool creerTables();
    void loadDataFromDB();
    bool saveToDB   (const QString &idCamion, const QString &loc,
                  const QString &cap,      const QString &fill,
                  const QString &state,    const QString &type,
                  const QString &zone,     const QString &driver,
                  const QString &phone);
    bool updateInDB (int id,
                    const QString &idCamion, const QString &loc,
                    const QString &cap,      const QString &fill,
                    const QString &state,    const QString &type,
                    const QString &zone,     const QString &driver,
                    const QString &phone);
    bool deleteFromDB(int id);

    // ── État interne ──────────────────────────────────────────
    int nextId;
    int editRow;
    int currentDbId;

    // ── Helpers ───────────────────────────────────────────────
    void buildUI();
    void loadData();
    void addRow(int id,
                const QString &idCamion, const QString &loc,
                const QString &cap,      const QString &fill,
                const QString &state,    const QString &type,
                const QString &zone,     const QString &driver,
                const QString &phone);
    void updateCharts();
    QString stateStyle(const QString &s);
    void clearForm();
    void fillForm(int row);
    void applyStyles();
};

#endif // MAINWINDOW_H
