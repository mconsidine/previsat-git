#-------------------------------------------------
#
# Project created by QtCreator 2011-10-20T20:56:11
#
#-------------------------------------------------

QT       += core gui

TARGET = PreviSat
TEMPLATE = app

VERSION = 3.0.0.0
ICON = resources/Icon3.ico
win32:RC_FILE = icone.rc

SOURCES += main.cpp\
        previsat.cpp \
        librairies/corps/corps.cpp \
        librairies/corps/etoiles/etoile.cpp \
        librairies/corps/etoiles/ligneconstellation.cpp \
        librairies/corps/satellite/elementsosculateurs.cpp \
        librairies/corps/satellite/satellite.cpp \
        librairies/corps/satellite/tle.cpp \
        librairies/corps/systemesolaire/lune.cpp \
        librairies/corps/systemesolaire/soleil.cpp \
        librairies/dates/date.cpp \
        librairies/exceptions/messages.cpp \
        librairies/exceptions/previsatexception.cpp \
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
        librairies/corps/corps.h \
        librairies/corps/etoiles/etoile.h \
        librairies/corps/etoiles/ligneconstellation.h \
        librairies/corps/satellite/elementsosculateurs.h \
        librairies/corps/satellite/satellite.h \
        librairies/corps/satellite/tle.h \
        librairies/corps/systemesolaire/lune.h \
        librairies/corps/systemesolaire/LuneConstants.h \
        librairies/corps/systemesolaire/soleil.h \
        librairies/corps/systemesolaire/SoleilConstants.h \
        librairies/corps/systemesolaire/TerreConstants.h \
        librairies/dates/date.h \
        librairies/dates/dateConstants.h \
        librairies/exceptions/messages.h \
        librairies/exceptions/previsatexception.h \
        librairies/maths/mathConstants.h \
        librairies/maths/maths.h \
        librairies/maths/matrice.h \
        librairies/maths/vecteur3d.h \
        librairies/observateur/observateur.h \
        previsions/conditions.h \
        previsions/evenements.h \
        previsions/iridium.h \
        previsions/prevision.h \
        previsions/transitiss.h

FORMS    += previsat.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    icone.rc
