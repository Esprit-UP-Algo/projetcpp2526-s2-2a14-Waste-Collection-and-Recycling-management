/********************************************************************************
** Form generated from reading UI file 'gestion_des_poubelles.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GESTION_DES_POUBELLES_H
#define UI_GESTION_DES_POUBELLES_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_gestion_des_poubelles
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *gestion_des_poubelles)
    {
        if (gestion_des_poubelles->objectName().isEmpty())
            gestion_des_poubelles->setObjectName("gestion_des_poubelles");
        gestion_des_poubelles->resize(800, 600);
        centralwidget = new QWidget(gestion_des_poubelles);
        centralwidget->setObjectName("centralwidget");
        gestion_des_poubelles->setCentralWidget(centralwidget);
        menubar = new QMenuBar(gestion_des_poubelles);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        gestion_des_poubelles->setMenuBar(menubar);
        statusbar = new QStatusBar(gestion_des_poubelles);
        statusbar->setObjectName("statusbar");
        gestion_des_poubelles->setStatusBar(statusbar);

        retranslateUi(gestion_des_poubelles);

        QMetaObject::connectSlotsByName(gestion_des_poubelles);
    } // setupUi

    void retranslateUi(QMainWindow *gestion_des_poubelles)
    {
        gestion_des_poubelles->setWindowTitle(QCoreApplication::translate("gestion_des_poubelles", "gestion_des_poubelles", nullptr));
    } // retranslateUi

};

namespace Ui {
    class gestion_des_poubelles: public Ui_gestion_des_poubelles {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GESTION_DES_POUBELLES_H
