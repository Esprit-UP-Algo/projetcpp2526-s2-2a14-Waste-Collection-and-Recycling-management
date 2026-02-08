#ifndef RECYCLAGE_H
#define RECYCLAGE_H

#include <QMainWindow>
#include <QVector>
#include <QString>
#include <QDate>

class QTableWidget;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QPushButton;
class QDateEdit;

struct RecyclageItem {
    int id;
    QString centre;
    QString type;
    double quantite; // Changed to double for precision
    QDate date;
    QString responsable;
};

class Recyclage : public QMainWindow
{
    Q_OBJECT

public:
    explicit Recyclage(QWidget *parent = nullptr);

private:
    void charger();
    void remplirForm(int row);
    void viderForm();

    QVector<RecyclageItem> data;

    QTableWidget *table;
    QLineEdit *idEdit;          // New
    QLineEdit *centreEdit;
    QComboBox *typeBox;
    QDoubleSpinBox *quantiteSpin; // Changed to DoubleSpinBox
    QDateEdit *dateEdit;        // New
    QLineEdit *responsableEdit; // New
    
    // Search & Sort Widgets
    QLineEdit *searchID;
    QLineEdit *searchCentre;
    
    QPushButton *btnEdit;
    QPushButton *btnDelete;
};

#endif
