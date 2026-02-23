#include <QApplication>
#include <QMessageBox>
#include "gestion_des_poubelles.h"
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Connection c;
    bool test = c.createconnect();

    MainWindow w;

    if(test) {
        w.show();
        QMessageBox::information(nullptr, QObject::tr("Connexion Oracle"),
                                 QObject::tr("Connexion reussie a Oracle !\n\n"
                                             "Username: tuniwaste\n"
                                             "Database: XE\n\n"
                                             "Cliquez OK pour continuer."),
                                 QMessageBox::Ok);
    } else {
        w.show();
        QMessageBox::warning(nullptr, QObject::tr("Connexion Oracle"),
                             QObject::tr("Impossible de se connecter a Oracle.\n\n"
                                         "Verifiez:\n"
                                         "- Le service OracleServiceXE est demarre\n"
                                         "- Le service OracleXETNSListener est demarre\n"
                                         "- Username: tuniwaste\n"
                                         "- Password: tuni123\n\n"
                                         "Cliquez OK pour continuer en mode hors-ligne."),
                             QMessageBox::Ok);
    }

    return a.exec();
}
