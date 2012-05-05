#-------------------------------------------------
#
# Project created by QtCreator 2012-01-25T21:18:45
#
#-------------------------------------------------

QT       += core gui network webkit

TARGET = PreviSat
TEMPLATE = app

VERSION = 3.0.0.11
ICON = resources/icone.ico

win32 {
    LIBS = zlibwapi.dll
    RC_FILE = icone.rc
}

TRANSLATIONS = PreviSat_en.ts PreviSat_es.ts

SOURCES += main.cpp\
    previsat.cpp \
    afficher.cpp \
    apropos.cpp \
    gestionnairetle.cpp \
    telecharger.cpp \
    threadcalculs.cpp \
    librairies/corps/corps.cpp \
    librairies/corps/etoiles/constellation.cpp \
    librairies/corps/etoiles/etoile.cpp \
    librairies/corps/etoiles/ligneconstellation.cpp \
    librairies/corps/satellite/elementsosculateurs.cpp \
    librairies/corps/satellite/satellite.cpp \
    librairies/corps/satellite/tle.cpp \
    librairies/corps/systemesolaire/lune.cpp \
    librairies/corps/systemesolaire/planete.cpp \
    librairies/corps/systemesolaire/soleil.cpp \
    librairies/dates/date.cpp \
    librairies/exceptions/previsatexception.cpp \
    librairies/exceptions/messages.cpp \
    librairies/maths/maths.cpp \
    librairies/maths/matrice.cpp \
    librairies/maths/vecteur3d.cpp \
    librairies/observateur/observateur.cpp \
    previsions/conditions.cpp \
    previsions/evenements.cpp \
    previsions/iridium.cpp \
    previsions/prevision.cpp \
    previsions/transitiss.cpp

HEADERS  += previsat.h \
    afficher.h \
    apropos.h \
    gestionnairetle.h \
    telecharger.h \
    threadcalculs.h \
    librairies/corps/corps.h \
    librairies/corps/etoiles/constellation.h \
    librairies/corps/etoiles/etoile.h \
    librairies/corps/etoiles/ligneconstellation.h \
    librairies/corps/satellite/elementsosculateurs.h \
    librairies/corps/satellite/satellite.h \
    librairies/corps/satellite/tle.h \
    librairies/corps/systemesolaire/lune.h \
    librairies/corps/systemesolaire/LuneConstants.h \
    librairies/corps/systemesolaire/planete.h \
    librairies/corps/systemesolaire/soleil.h \
    librairies/corps/systemesolaire/SoleilConstants.h \
    librairies/corps/systemesolaire/TerreConstants.h \
    librairies/dates/date.h \
    librairies/dates/dateConstants.h \
    librairies/exceptions/previsatexception.h \
    librairies/exceptions/messages.h \
    librairies/maths/mathConstants.h \
    librairies/maths/maths.h \
    librairies/maths/matrice.h \
    librairies/maths/vecteur3d.h \
    librairies/observateur/observateur.h \
    previsions/conditions.h \
    previsions/evenements.h \
    previsions/iridium.h \
    previsions/prevision.h \
    previsions/transitiss.h \
    zlib/zlib.h \
    zlib/zconf.h \
    zlib/ioapi.h

FORMS    += previsat.ui \
    afficher.ui \
    apropos.ui \
    gestionnairetle.ui \
    telecharger.ui

OTHER_FILES += icone.rc

RESOURCES += \
    resources.qrc
