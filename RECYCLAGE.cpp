#include "recyclage.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFrame>
#include <QPixmap>
#include <QDateEdit>
#include <QMessageBox>
#include <algorithm>
#include <QIntValidator>
#include <QApplication>
#include <QStyle>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>

Recyclage::Recyclage(QWidget *parent) : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    central->setStyleSheet("background:#F4F6F8;");

    /* ================= SIDEBAR ================= */
    QFrame *sidebar = new QFrame;
    sidebar->setFixedWidth(260);
    sidebar->setStyleSheet("background:#5d9e63;"); // Matching the screenshot green (approx)

    QVBoxLayout *side = new QVBoxLayout(sidebar);
    side->setSpacing(5);
    side->setContentsMargins(0, 20, 0, 20);

    // Header Container
    QWidget *headerWidget = new QWidget;
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 0, 20, 20); // Add padding
    
    QLabel *logo = new QLabel;
    // Logo setup
    QPixmap logoPix("logo.png"); // Make sure logo.png is in the build directory
    if(logoPix.isNull()) {
         // Try absolute path for safety if relative fails (dev convenience)
         logoPix.load("c:/Users/LENOVO/Downloads/CR/logo.png"); 
    }
    
    if(logoPix.isNull()) {
        logo->setText("♻️"); 
        logo->setStyleSheet("font-size:30px; color:white; background:transparent; border:none;");
    } else {
        // Remove white background
        logoPix.setMask(logoPix.createMaskFromColor(Qt::white));
        
        // Scale to small icon size
        logo->setPixmap(logoPix.scaled(40,40,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        logo->setStyleSheet("background:transparent; border:none;");
    }
    logo->setAlignment(Qt::AlignCenter);

    QLabel *app = new QLabel("TuniWaste");
    app->setStyleSheet("color:white;font-size:20px;font-weight:800;font-family: 'Segoe UI', sans-serif; background:transparent; border:none;");
    
    headerLayout->addWidget(logo);
    headerLayout->addWidget(app);
    headerLayout->addStretch(); // Push to left if needed, or center? Screenshot shows left alignment or center block.
    // Actually screenshot shows: [Icon] TuniWaste. Centered or Left?
    // Let's keep them handled by layout.
    
    side->addWidget(headerWidget);

    // Sidebar Menu Item Helper with Emojis for "Colored Logos"
    auto menu = [&](const QString &icon, const QString &text, bool active = false){
        QPushButton *btn = new QPushButton(icon + "  " + text);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { "
            "   color:white; font-size:15px; text-align:left; padding:12px 20px; border:none; font-family: 'Segoe UI', sans-serif; font-weight: 500;"
            "   background-color: " + QString(active ? "#4a8a50" : "transparent") + ";" // Darker highlight
            "   border-left: " + QString(active ? "4px solid #FBC02D" : "4px solid transparent") + ";" // Orange accents in screenshot? Or white? Let's stick to screenshot hint. Screenshot shows no left border maybe, but distinct background.
            // Actually screenshot shows: Simple text, generic icons. The selected item has a slightly different background?
            // Let's stick to the user's previous "active" style but refine colors.
            "}"
            "QPushButton:hover { background-color:#6ba370; }"
        );
        return btn;
    };

    side->addWidget(menu("🏠", "Tableau de bord"));
    side->addWidget(menu("👥", "Gestion des utilisateurs"));
    side->addWidget(menu("🚚", "Gestion des camions"));
    side->addWidget(menu("🗑️", "Gestion des poubelles"));
    side->addWidget(menu("🗺️", "Gestion des zones"));
    side->addWidget(menu("♻️", "Gestion de recyclage", true));
    side->addWidget(menu("📑", "Suivi des collectes"));
    side->addWidget(menu("📊", "Rapports"));
    side->addStretch();
    side->addWidget(menu("⚙️", "Paramètres"));

    /* ================= CONTENT ================= */
    QWidget *content = new QWidget;
    
    // ... (rest of layout structure) ...
    // Note: To avoid replacing the whole file and losing Search fix, I will target the specific sidebar block
    // But replace_file_content needs contiguous block. 
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(20);
    contentLayout->setContentsMargins(30, 30, 30, 30);

    /* ===== Header ===== */
    // Brand
    QLabel *brand = new QLabel("TuniWaste");
    brand->setStyleSheet("color:#5d9e63;font-size:20px;font-weight:800;font-family: 'Segoe UI', sans-serif; margin-bottom: 5px;");

    // Path
    QLabel *path = new QLabel("Tableau de bord / Tableau de bord / Gestion de recyclage");
    path->setStyleSheet("color:#90A4AE;font-size:14px; margin-bottom: 10px; font-weight: 500;");

    // Title
    QLabel *title = new QLabel("Gestion de recyclage");
    title->setStyleSheet("color:#212121;font-size:28px;font-weight:800;font-family: 'Segoe UI', sans-serif;");

    contentLayout->addWidget(brand);
    contentLayout->addWidget(path);
    contentLayout->addWidget(title);
    contentLayout->addSpacing(20);

    /* ===== Search & Filter Bar ===== */
    QHBoxLayout *searchLayout = new QHBoxLayout;
    QString searchStyle = "QLineEdit{background:white;border:none;padding:10px;border-radius:6px;min-width:200px;color:#455A64;}";
    
    // Correctly initialize member variables
    searchID = new QLineEdit;
    searchID->setPlaceholderText("🔍 Rechercher par ID...");
    searchID->setStyleSheet(searchStyle);

    searchCentre = new QLineEdit;
    searchCentre->setPlaceholderText("🔍 Rechercher par Centre...");
    searchCentre->setStyleSheet(searchStyle);

    QLabel *lblSort = new QLabel("Trier par:");
    lblSort->setStyleSheet("color:#455A64;font-weight:600;");

    QPushButton *btnSort = new QPushButton("Quantité");
    btnSort->setStyleSheet("QPushButton{background:white;color:#455A64;border:none;padding:10px 20px;border-radius:6px;}");
    btnSort->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarShadeButton));
    
    // PDF Button
    QPushButton *btnPdf = new QPushButton("📄 Exporter PDF");
    btnPdf->setStyleSheet("QPushButton{background:#E53935;color:white;font-weight:800;padding:10px 20px;border-radius:6px;} QPushButton:hover{background:#D32F2F;}");
    
    // PDF Logic (Using 300 DPI and correct layout)
    connect(btnPdf, &QPushButton::clicked, this, [=](){
        QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "PDF Files (*.pdf)");
        if (fileName.isEmpty()) return;
        if (QFileInfo(fileName).suffix().isEmpty()) fileName.append(".pdf");

        QPdfWriter writer(fileName);
        writer.setPageSize(QPageSize(QPageSize::A4));
        writer.setResolution(300); // 300 DPI
        writer.setPageMargins(QMarginsF(15, 15, 15, 15));

        QPainter painter(&writer);
        painter.setRenderHint(QPainter::Antialiasing);

        int width = writer.width();
        int headerHeight = 350;
        
        // Header
        painter.fillRect(QRect(0, 0, width, headerHeight), QColor("#5d9e63"));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Segoe UI", 36, QFont::ExtraBold));
        painter.drawText(QRect(50, 0, width - 100, headerHeight), Qt::AlignVCenter | Qt::AlignLeft, "TuniWaste");
        painter.setFont(QFont("Segoe UI", 16, QFont::Bold));
        painter.drawText(QRect(50, 0, width - 100, headerHeight), Qt::AlignVCenter | Qt::AlignRight, "RAPPORT DE GESTION\nDE RECYCLAGE");
        
        int y = headerHeight + 100;
        
        // Date
        painter.setPen(Qt::black);
        painter.setFont(QFont("Segoe UI", 12));
        painter.drawText(50, y, "Date du rapport: " + QDate::currentDate().toString("dd/MM/yyyy"));
        y += 150;
        
        // Stats
        double totalQ = 0;
        QMap<QString, double> typeStats;
        for(const auto &item : data) {
            totalQ += item.quantite;
            typeStats[item.type] += item.quantite;
        }
        
        // General Stats Section
        painter.setPen(QColor("#5d9e63"));
        painter.setFont(QFont("Segoe UI", 18, QFont::Bold));
        painter.drawText(50, y, "STATISTIQUES GÉNÉRALES");
        QPen greenPen(QColor("#5d9e63")); greenPen.setWidth(3); painter.setPen(greenPen);
        painter.drawLine(50, y + 30, width - 50, y + 30);
        y += 100;
        
        painter.setPen(Qt::black); painter.setFont(QFont("Segoe UI", 14));
        painter.drawText(80, y, "• Nombre total d'enregistrements: " + QString::number(data.size())); y += 60;
        painter.drawText(80, y, "• Quantité totale traitée: " + QString::number(totalQ, 'f', 2) + " kg"); y += 150;
        
        // Details Section
        painter.setPen(QColor("#5d9e63")); painter.setFont(QFont("Segoe UI", 18, QFont::Bold));
        painter.drawText(50, y, "DÉTAILS PAR TYPE"); painter.setPen(greenPen);
        painter.drawLine(50, y + 30, width - 50, y + 30); y += 100;
        
        // Table
        int tableIndent = 50; int col1 = tableIndent; int col2 = tableIndent + 500; int col3 = tableIndent + 900; int rowHeight = 80;
        
        painter.fillRect(QRect(tableIndent - 20, y - 10, width - (2*tableIndent) + 40, rowHeight), QColor("#E8F5E9"));
        painter.setPen(QColor("#1B5E20")); painter.setFont(QFont("Segoe UI", 14, QFont::Bold));
        painter.drawText(col1, y + 55, "Type de Déchet"); painter.drawText(col2, y + 55, "Quantité (kg)"); painter.drawText(col3, y + 55, "Pourcentage"); y += rowHeight;
        
        painter.setFont(QFont("Segoe UI", 12)); painter.setPen(Qt::black); QPen linePen(QColor("#E0E0E0")); linePen.setWidth(2);
        for(auto it = typeStats.begin(); it != typeStats.end(); ++it) {
            double percent = (totalQ > 0) ? (it.value() / totalQ) * 100.0 : 0.0;
            painter.drawText(col1, y + 55, it.key()); painter.drawText(col2, y + 55, QString::number(it.value(), 'f', 2)); painter.drawText(col3, y + 55, QString::number(percent, 'f', 1) + "%");
            painter.setPen(linePen); painter.drawLine(tableIndent - 20, y + rowHeight, width - tableIndent + 20, y + rowHeight); painter.setPen(Qt::black); y += rowHeight;
        }
        
        // Footer
        painter.setPen(Qt::gray); painter.setFont(QFont("Segoe UI", 10));
        painter.drawText(QRect(0, writer.height() - 150, width, 50), Qt::AlignCenter, "TuniWaste - Application de Gestion de Recyclage - " + QString::number(QDate::currentDate().year()));
        painter.end();
        QMessageBox::information(this, "Succès", "Le PDF a été généré avec succès.");
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    });
    
    QPushButton *btnAdd = new QPushButton(" + Ajouter recyclage");
    btnAdd->setStyleSheet("QPushButton{background:#66BB6A;color:white;font-weight:800;padding:10px 20px;border-radius:6px;} QPushButton:hover{background:#4CAF50;}");

    searchLayout->addWidget(searchID);
    searchLayout->addWidget(searchCentre);
    searchLayout->addStretch();
    searchLayout->addWidget(btnSort);
    searchLayout->addWidget(btnPdf); 
    searchLayout->addWidget(btnAdd);

    contentLayout->addLayout(searchLayout);

    /* ===== Form Area ===== */
    QFrame *formCard = new QFrame;
    formCard->setStyleSheet("background:white;border-radius:8px;");
    QVBoxLayout *formCardLayout = new QVBoxLayout(formCard);
    
    QLabel *formTitle = new QLabel("Formulaire de Recyclage");
    formTitle->setStyleSheet("color:#455A64;font-size:16px;font-weight:600;margin-bottom:10px;");
    formCardLayout->addWidget(formTitle);

    QHBoxLayout *formLayout = new QHBoxLayout;
    
    QString inputStyle = 
        "QLineEdit, QComboBox, QDateEdit, QDoubleSpinBox {"
        "background:#F5F5F5; border:1px solid #E0E0E0; border-radius:4px; padding:8px; color:#757575; font-weight:600; font-size:13px;"
        "}"
        "QLineEdit:focus, QComboBox:focus { border:1px solid #66BB6A; background:white; }"
        "QComboBox QAbstractItemView {"
        "   background: white;"
        "   color: #37474F;"
        "   selection-background-color: #E8F5E9;"
        "   selection-color: #1B5E20;"
        "   border: 1px solid #CFD8DC;"
        "}"
        // Fix for Calendar Widget
        "QCalendarWidget QWidget {"
        "   background-color: white; color: #37474F;"
        "}"
        "QCalendarWidget QToolButton {"
        "   color: #37474F;"
        "   background-color: transparent;"
        "}"
        "QCalendarWidget QToolButton:hover {"
        "   background-color: #E8F5E9;"
        "}"
        "QCalendarWidget QMenu {"
        "   background-color: white; color: #37474F;"
        "}"
        "QCalendarWidget QSpinBox {"
        "   background-color: white; color: #37474F;"
        "}";

    idEdit = new QLineEdit;
    idEdit->setPlaceholderText("ID");
    idEdit->setStyleSheet(inputStyle);
    idEdit->setValidator(new QIntValidator(0, 999999, this));

    centreEdit = new QLineEdit;
    centreEdit->setPlaceholderText("Centre de Recyclage");
    centreEdit->setStyleSheet(inputStyle);

    typeBox = new QComboBox;
    typeBox->addItems({"Plastique", "Papier", "Verre", "Métal", "Organique", "Autre"});
    typeBox->setStyleSheet(inputStyle);

    quantiteSpin = new QDoubleSpinBox;
    quantiteSpin->setRange(0, 99999.99);
    quantiteSpin->setSuffix(" kg");
    quantiteSpin->setStyleSheet(inputStyle);

    dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    dateEdit->setCalendarPopup(true);
    dateEdit->setStyleSheet(inputStyle);

    responsableEdit = new QLineEdit;
    responsableEdit->setPlaceholderText("Responsable");
    responsableEdit->setStyleSheet(inputStyle);

    // Form Action Buttons
    btnEdit = new QPushButton("Confirmer"); 
    btnDelete = new QPushButton("Annuler"); 

    btnEdit->setStyleSheet("background:#66BB6A;color:white;padding:8px 20px;border-radius:4px;font-weight:bold;");
    btnDelete->setStyleSheet("background:#EF5350;color:white;padding:8px 20px;border-radius:4px;font-weight:bold;");

    formLayout->addWidget(idEdit);
    formLayout->addWidget(centreEdit);
    formLayout->addWidget(typeBox);
    formLayout->addWidget(quantiteSpin);
    formLayout->addWidget(dateEdit);
    formLayout->addWidget(responsableEdit);
    formLayout->addWidget(btnEdit);
    formLayout->addWidget(btnDelete);

    formCardLayout->addLayout(formLayout);
    contentLayout->addWidget(formCard);

    /* ===== Table ===== */
    table = new QTableWidget(0, 7);
    table->setHorizontalHeaderLabels({"ID", "Centre", "Responsable", "Type", "Quantité", "Date", "Actions"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents); 
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(50); // Increase Row Height for Buttons
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setShowGrid(false);
    
    // Table Style match
    table->setStyleSheet(
        "QTableWidget { background:white; border-radius:8px; padding:10px; border:none; gridline-color:#ECEFF1; font-family: 'Segoe UI', sans-serif; }"
        "QHeaderView::section { background:white; color:#212121; padding:10px; font-weight:700; font-size: 13px; border-bottom:2px solid #E0E0E0; border-right: 1px solid #ECEFF1; }"
        "QTableWidget::item { padding-left:10px; border-bottom:1px solid #F5F5F5; border-right: 1px solid #ECEFF1; color:#37474F; font-weight:600; font-size:12px; }"
        "QTableWidget::item:selected { background:#E8F5E9; color:#1B5E20; }"
        // Scrollbar Style
        "QScrollBar:vertical { border: none; background: #F5F5F5; width: 8px; margin: 0px 0px 0px 0px; border-radius: 4px; }"
        "QScrollBar::handle:vertical { background: #B0BEC5; min-height: 20px; border-radius: 4px; }"
        "QScrollBar::add-line:vertical { height: 0px; subcontrol-position: bottom; subcontrol-origin: margin; }"
        "QScrollBar::sub-line:vertical { height: 0px; subcontrol-position: top; subcontrol-origin: margin; }"
    );
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    contentLayout->addWidget(table);

    /* ================= GLOBAL LAYOUT ================= */
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(content);

    /* ================= LOGIGUE ================= */
    connect(btnAdd, &QPushButton::clicked, this, [=](){
        idEdit->setFocus();
        viderForm();
    });

    connect(btnEdit, &QPushButton::clicked, this, [=](){
        if(idEdit->text().isEmpty() || centreEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Veuillez remplir les champs obligatoires (ID, Centre).");
            return;
        }

        int id = idEdit->text().toInt();
        
        bool exists = false;
        for(const auto &item : data) {
            if(item.id == id) {
                exists = true;
                break;
            }
        }

        if (idEdit->isReadOnly()) {
            for(int i=0; i<data.size(); ++i) {
                if(data[i].id == id) {
                    data[i].centre = centreEdit->text();
                    data[i].type = typeBox->currentText();
                    data[i].quantite = quantiteSpin->value();
                    data[i].date = dateEdit->date();
                    data[i].responsable = responsableEdit->text();
                    break;
                }
            }
        } else {
            if(exists) {
                QMessageBox::warning(this, "Erreur", "Cet ID existe déjà.");
                return;
            }
            data.push_back({
                id,
                centreEdit->text(),
                typeBox->currentText(),
                quantiteSpin->value(),
                dateEdit->date(),
                responsableEdit->text()
            });
        }

        charger();
        viderForm();
    });

    connect(btnDelete, &QPushButton::clicked, this, &Recyclage::viderForm);

    connect(searchID, &QLineEdit::textChanged, this, &Recyclage::charger);
    connect(searchCentre, &QLineEdit::textChanged, this, &Recyclage::charger);
    
    // Sort logic
    connect(btnSort, &QPushButton::clicked, this, [=](){
        std::sort(data.begin(), data.end(), [](const RecyclageItem &a, const RecyclageItem &b){
            return a.quantite > b.quantite; 
        });
        charger();
    });

    /* ===== Data Init ===== */
    data.push_back({1, "Centre Tunis", "Plastique", 120.5, QDate(2023, 10, 20), "Ahmed Ben Ali"});
    data.push_back({2, "Centre Ariana", "Verre", 80.0, QDate(2023, 10, 21), "Sami Trabelsi"});
    data.push_back({3, "Centre Sfax", "Papier", 200.0, QDate(2023, 10, 22), "Ali Haddad"});

    charger();
}

void Recyclage::charger()
{
    table->setRowCount(0);
    
    QString sId = "";
    if(searchID) sId = searchID->text();
    QString sCentre = "";
    if(searchCentre) sCentre = searchCentre->text().toLower();

    for(int i=0; i<data.size(); ++i) {
        if(!sId.isEmpty() && !QString::number(data[i].id).contains(sId)) continue;
        if(!sCentre.isEmpty() && !data[i].centre.toLower().contains(sCentre)) continue;

        int row = table->rowCount();
        table->insertRow(row);
        table->setColumnWidth(6, 140); // Force wider column for Actions

        table->setItem(row, 0, new QTableWidgetItem(QString::number(data[i].id)));
        table->setItem(row, 1, new QTableWidgetItem(data[i].centre));
        table->setItem(row, 2, new QTableWidgetItem(data[i].responsable));
        table->setItem(row, 3, new QTableWidgetItem(data[i].type));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(data[i].quantite, 'f', 2) + " kg"));
        table->setItem(row, 5, new QTableWidgetItem(data[i].date.toString("dd/MM/yyyy")));

        // ACTIONS COLUMN FIX
        QWidget *actionWidget = new QWidget;
        QVBoxLayout *actionLayout = new QVBoxLayout(actionWidget); // VERTICAL STACK
        actionLayout->setContentsMargins(5, 5, 5, 5); // Add margins for padding
        actionLayout->setSpacing(4);
        actionLayout->setAlignment(Qt::AlignCenter);

        QPushButton *btnMod = new QPushButton("Modifier");
        btnMod->setCursor(Qt::PointingHandCursor);
        btnMod->setStyleSheet("background:#4CAF50;color:white;border:none;padding:6px 12px;border-radius:6px;font-weight:800;font-size:12px;min-width: 90px;");
        
        QPushButton *btnSup = new QPushButton("Supprimer");
        btnSup->setCursor(Qt::PointingHandCursor);
        btnSup->setStyleSheet("background:#D32F2F;color:white;border:none;padding:6px 12px;border-radius:6px;font-weight:800;font-size:12px;min-width: 90px;");
        
        // Make sure row is tall enough
        table->setRowHeight(row, 90);

        int currentId = data[i].id;

        connect(btnMod, &QPushButton::clicked, this, [=](){
            for(int k=0; k<data.size(); k++) {
                if(data[k].id == currentId) {
                    remplirForm(k);
                    break;
                }
            }
        });

        connect(btnSup, &QPushButton::clicked, this, [=](){
             if(QMessageBox::question(this, "Supprimer", "Confirmer la suppression ?") == QMessageBox::Yes) {
                for(int k=0; k<data.size(); k++) {
                    if(data[k].id == currentId) {
                        data.remove(k);
                        break;
                    }
                }
                charger();
            }
        });

        actionLayout->addWidget(btnMod);
        actionLayout->addWidget(btnSup);
        
        table->setCellWidget(row, 6, actionWidget);
    }
}

void Recyclage::remplirForm(int row)
{
    if(row < 0 || row >= data.size()) return;
    idEdit->setText(QString::number(data[row].id));
    idEdit->setReadOnly(true); // LOCK KEY
    idEdit->setStyleSheet("background:#E0E0E0; border:1px solid #B0BEC5; border-radius:4px; padding:8px; color:#546E7A;");
    
    centreEdit->setText(data[row].centre);
    typeBox->setCurrentText(data[row].type);
    quantiteSpin->setValue(data[row].quantite);
    dateEdit->setDate(data[row].date);
    responsableEdit->setText(data[row].responsable);
}

void Recyclage::viderForm()
{
    idEdit->clear();
    idEdit->setReadOnly(false); // UNLOCK KEY
    idEdit->setStyleSheet("background:#F5F5F5; border:1px solid #E0E0E0; border-radius:4px; padding:8px; color:#37474F;");

    centreEdit->clear();
    typeBox->setCurrentIndex(0);
    quantiteSpin->setValue(0);
    dateEdit->setDate(QDate::currentDate());
    responsableEdit->clear();
}
