#ifndef GESTION_DES_POUBELLES_H
#define GESTION_DES_POUBELLES_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QPainter>
#include <QPainterPath>

// ============================================
// NIVEAU DONUT CHART WIDGET
// ============================================
class NiveauDonutChart : public QWidget
{
    Q_OBJECT
public:
    NiveauDonutChart(QWidget *parent = nullptr)
        : QWidget(parent), niveauMoyen(0)
    {
        setMinimumSize(250, 300);
        setStyleSheet("background-color: white; border: 1px solid #ddd; border-radius: 8px;");
    }

    void setNiveau(int niveau)
    {
        niveauMoyen = niveau;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        int w = width();
        int h = height();

        // Titre
        QFont titleFont;
        titleFont.setPointSize(12);
        titleFont.setBold(true);
        painter.setFont(titleFont);
        painter.setPen(Qt::black);
        painter.drawText(10, 20, w - 20, 25, Qt::AlignLeft, "Niveau Moyen");

        // Déterminer la couleur
        QColor donutColor;
        if (niveauMoyen >= 90) {
            donutColor = QColor(244, 67, 54);  // Rouge
        } else if (niveauMoyen >= 70) {
            donutColor = QColor(255, 193, 7);  // Jaune
        } else if (niveauMoyen >= 50) {
            donutColor = QColor(255, 152, 0);  // Orange
        } else {
            donutColor = QColor(76, 175, 80);  // Vert
        }

        // Donut
        int centerX = w / 2;
        int centerY = 90;
        int outerRadius = 45;
        int innerRadius = 30;

        float used = (niveauMoyen * 360.0f) / 100.0f;
        float remaining = 360.0f - used;

        // Part utilisée
        painter.setBrush(QBrush(donutColor));
        painter.setPen(Qt::NoPen);

        QPainterPath path;
        path.moveTo(centerX, centerY);
        path.arcTo(centerX - outerRadius, centerY - outerRadius,
                   2 * outerRadius, 2 * outerRadius, 0, used);
        path.arcTo(centerX - innerRadius, centerY - innerRadius,
                   2 * innerRadius, 2 * innerRadius, used, -used);
        path.closeSubpath();
        painter.drawPath(path);

        // Part non utilisée (gris clair)
        painter.setBrush(QBrush(QColor(230, 230, 230)));

        QPainterPath path2;
        path2.moveTo(centerX, centerY);
        path2.arcTo(centerX - outerRadius, centerY - outerRadius,
                    2 * outerRadius, 2 * outerRadius, used, remaining);
        path2.arcTo(centerX - innerRadius, centerY - innerRadius,
                    2 * innerRadius, 2 * innerRadius, 360.0f, -remaining);
        path2.closeSubpath();
        painter.drawPath(path2);

        // Texte au centre - Pourcentage en noir, même taille que les autres donuts (comme le "3")
        painter.setFont(QFont("Arial", 20, QFont::Bold));
        painter.setPen(Qt::black);  // Texte en noir
        painter.drawText(centerX - 50, centerY - 15, 100, 30, Qt::AlignCenter,
                         QString::number(niveauMoyen) + "%");

        // Légende (texte descriptif) - même position que les autres légendes
        painter.setFont(QFont("Arial", 9));
        painter.setPen(Qt::black);  // Texte en noir, comme les autres légendes
        painter.drawText(10, 160, w - 20, 15, Qt::AlignCenter,
                         "Taux de remplissage moyen");
    }

private:
    int niveauMoyen;
};

class NiveauDonutChart;

// Widget personnalisé pour les graphiques donut
class DonutChart : public QWidget
{
    Q_OBJECT
public:
    DonutChart(const QString &title, QWidget *parent = nullptr);
    void setData(const QMap<QString, int> &data);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString title;
    QMap<QString, QColor> colors;
    QMap<QString, int> data;
    int total;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTableCellDoubleClicked(int row, int column);
    void onEnregistrerClicked();
    void onModifierClicked();
    void onSupprimerClicked();
    void onExporterPDFClicked();
    void onSearchChanged(const QString &text);
    void onEtatFilterChanged(int index);
    void onTypeFilterChanged(int index);
    void updateCharts();

private:
    void setupUI();
    void createSidebar();
    void createMainContent();
    void createFormPanel();
    void createTablePanel();
    void createStatsPanel();
    void applyStyles();
    void loadDummyData();
    void addTableRow(const QString &capacite, int niveau, const QString &etat,
                     const QString &idZone);

    // Widgets principaux
    QWidget *centralWidget;
    QWidget *sidebar;
    QWidget *mainContent;

    // Formulaire (gauche)
    QWidget *formPanel;
    QLineEdit *localisationInput;
    QComboBox *typeCombo;
    QSpinBox *capaciteInput;
    QSpinBox *niveauInput;
    QComboBox *etatCombo;
    QLineEdit *idZoneInput;
    QPushButton *enregistrerBtn;

    // Tableau (haut-centre)
    QWidget *tablePanel;
    QTableWidget *poubelleTable;
    QLineEdit *searchInput;
    QComboBox *etatFilterCombo;
    QComboBox *typeFilterCombo;
    QPushButton *exportPdfBtn;

    // Statistiques (bas-centre/droite)
    QWidget *statsPanel;
    DonutChart *etatChart;
    DonutChart *typeChart;
    NiveauDonutChart *niveauDonutChart;
    QLabel *niveauMoyenLabel;

    int currentEditingRow;
    int nextId;
};

#endif // GESTION_DES_POUBELLES_H
