#include <QLibraryInfo>
#include <QLocale>
#include <QtGui/QApplication>
#include <QTranslator>
#include "previsat.h"

#include "librairies/corps/systemesolaire/planete.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&translator);

    Date date(2005, 1, 1., 0.);
    Planete planete(0);
    Soleil soleil;
    soleil.CalculPosition(date);
    planete.CalculPosition(date, soleil);


    PreviSat w;

    w.Initialisations();

    w.EnchainementCalculs();

    w.AffichageDonnees();
    w.AffichageCourbes();

    w.show();

    return a.exec();
}
