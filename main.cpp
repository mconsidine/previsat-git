#include <QLibraryInfo>
#include <QLocale>
#include <QtGui/QApplication>
#include <QTranslator>
#include "previsat.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&translator);

    PreviSat w;

    w.Initialisations();

    w.EnchainementCalculs();

    w.AffichageDonnees();
    w.AffichageCourbes();

    w.show();

    return a.exec();
}
