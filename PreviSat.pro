#
#     PreviSat, Satellite tracking software
#     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
#
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# _______________________________________________________________________________________________________
#
# Nom du fichier
# >    PreviSat.pro
#
# Localisation
# >
#
# Description
# >    Fichier de configuration du projet
#
# Auteur
# >    Astropedia
#
# Date de creation
# >    11 juillet 2011
#
# Date de revision
# >    21 septembre 2022

#-------------------------------------------------
VER_MAJ      = 6.0
VERSION      = 6.0.0.0
ANNEES_DEV   = 2005-2022
TRANSLATIONS = translations/PreviSat_en.ts
BUILD_TEST   = false
CLEANUP_TEST = true
COV_TEST     = false
#-------------------------------------------------

TARGET = PreviSat
TEMPLATE = app

!equals(QT_MAJOR_VERSION, 6) {
    error("Qt $${QT_VERSION} is not suited to compile $${TARGET}. Use Qt 6.3.2 in preference")
}

QT += concurrent multimedia printsupport widgets xml
greaterThan(QT_GCC_MAJOR_VERSION, 5) {
    QMAKE_CXXFLAGS += -std=c++17
    CONFIG += c++17
}


ORGANIZATION = Astropedia
DOMAIN = http://astropedia.free.fr/

VERSION_STR     = '\\"$${VERSION}\\"'
VER_MAJ_STR     = '\\"$${VER_MAJ}\\"'
ANNEES_DEV_STR  = '\\"$${ANNEES_DEV}\\"'
APP_NAME_STR    = '\\"$${TARGET}\\"'
ORG_NAME_STR    = '\\"$${ORGANIZATION}\\"'
DOMAIN_NAME_STR = '\\"$${DOMAIN}\\"'

DEFINES += APPVERSION=\"$${VERSION_STR}\" \
    APPVER_MAJ=\"$${VER_MAJ_STR}\"        \
    APP_ANNEES_DEV=\"$${ANNEES_DEV_STR}\" \
    APP_NAME=\"$${APP_NAME_STR}\"         \
    ORG_NAME=\"$${ORG_NAME_STR}\"         \
    DOMAIN_NAME=\"$${DOMAIN_NAME_STR}\"   \
    BUILD_TEST=$$BUILD_TEST               \
    QT_DEPRECATED_WARNINGS                \
    QT_MESSAGELOGCONTEXT

INCLUDEPATH += src


SOURCES += \
    src/configuration/configuration.cpp                     \
    src/configuration/evenementsstationspatiale.cpp         \
    src/configuration/fichierobs.cpp                        \
    src/configuration/gestionnairexml.cpp                   \
    src/interface/onglets/donnees/informationsiss.cpp       \
    src/interface/onglets/donnees/informationssatellite.cpp \
    src/interface/onglets/donnees/recherchesatellite.cpp    \
    src/interface/onglets/general/general.cpp               \
    src/interface/onglets/onglets.cpp                       \
    src/interface/onglets/osculateurs/osculateurs.cpp       \
    src/interface/onglets/previsions/evenementsorbitaux.cpp \
    src/interface/onglets/previsions/flashs.cpp             \
    src/interface/onglets/previsions/previsionspassage.cpp  \
    src/interface/onglets/previsions/transits.cpp           \
    src/interface/options/options.cpp                       \
    src/interface/outils/outils.cpp                         \
    src/interface/previsat.cpp                              \
    src/librairies/corps/corps.cpp                          \
    src/librairies/corps/etoiles/constellation.cpp          \
    src/librairies/corps/etoiles/etoile.cpp                 \
    src/librairies/corps/etoiles/ligneconstellation.cpp     \
    src/librairies/corps/satellite/conditioneclipse.cpp     \
    src/librairies/corps/satellite/donnees.cpp              \
    src/librairies/corps/satellite/elementsosculateurs.cpp  \
    src/librairies/corps/satellite/evenements.cpp           \
    src/librairies/corps/satellite/gpformat.cpp             \
    src/librairies/corps/satellite/magnitude.cpp            \
    src/librairies/corps/satellite/phasage.cpp              \
    src/librairies/corps/satellite/satellite.cpp            \
    src/librairies/corps/satellite/sgp4.cpp                 \
    src/librairies/corps/satellite/signal.cpp               \
    src/librairies/corps/satellite/tle.cpp                  \
    src/librairies/corps/systemesolaire/lune.cpp            \
    src/librairies/corps/systemesolaire/planete.cpp         \
    src/librairies/corps/systemesolaire/soleil.cpp          \
    src/librairies/dates/date.cpp                           \
    src/librairies/exceptions/message.cpp                   \
    src/librairies/exceptions/previsatexception.cpp         \
    src/librairies/maths/maths.cpp                          \
    src/librairies/maths/matrice3d.cpp                      \
    src/librairies/maths/vecteur3d.cpp                      \
    src/librairies/observateur/observateur.cpp              \
    src/librairies/systeme/logmessage.cpp                   \
    src/librairies/systeme/telechargement.cpp               \
#    src/previsions/flashs.cpp \
    src/previsions/prevision.cpp


HEADERS += \
    src/configuration/categorieelementsorbitaux.h         \
    src/configuration/configuration.h                     \
    src/configuration/configurationconst.h                \
    src/configuration/evenementsstationspatiale.h         \
    src/configuration/fichierobs.h                        \
    src/configuration/gestionnairexml.h                   \
    src/configuration/satellitesflashs.h                  \
    src/configuration/satellitetdrs.h                     \
    src/interface/onglets/donnees/informationsiss.h       \
    src/interface/onglets/donnees/informationssatellite.h \
    src/interface/onglets/donnees/recherchesatellite.h    \
    src/interface/onglets/general/general.h               \
    src/interface/onglets/onglets.h                       \
    src/interface/onglets/osculateurs/osculateurs.h       \
    src/interface/onglets/previsions/evenementsorbitaux.h \
    src/interface/onglets/previsions/flashs.h             \
    src/interface/onglets/previsions/previsionspassage.h  \
    src/interface/onglets/previsions/transits.h           \
    src/interface/options/options.h                       \
    src/interface/outils/outils.h                         \
    src/interface/previsat.h                              \
    src/librairies/corps/corps.h                          \
    src/librairies/corps/corpsconst.h                     \
    src/librairies/corps/ephemerides.h                    \
    src/librairies/corps/etoiles/constellation.h          \
    src/librairies/corps/etoiles/etoile.h                 \
    src/librairies/corps/etoiles/ligneconstellation.h     \
    src/librairies/corps/satellite/conditioneclipse.h     \
    src/librairies/corps/satellite/donnees.h              \
    src/librairies/corps/satellite/elementsorbitaux.h     \
    src/librairies/corps/satellite/elementsosculateurs.h  \
    src/librairies/corps/satellite/evenements.h           \
    src/librairies/corps/satellite/evenementsconst.h      \
    src/librairies/corps/satellite/gpformat.h             \
    src/librairies/corps/satellite/magnitude.h            \
    src/librairies/corps/satellite/phasage.h              \
    src/librairies/corps/satellite/satellite.h            \
    src/librairies/corps/satellite/sgp4.h                 \
    src/librairies/corps/satellite/sgp4const.h            \
    src/librairies/corps/satellite/signal.h               \
    src/librairies/corps/satellite/tle.h                  \
    src/librairies/corps/systemesolaire/lune.h            \
    src/librairies/corps/systemesolaire/luneconst.h       \
    src/librairies/corps/systemesolaire/planete.h         \
    src/librairies/corps/systemesolaire/planeteconst.h    \
    src/librairies/corps/systemesolaire/soleil.h          \
    src/librairies/corps/systemesolaire/soleilconst.h     \
    src/librairies/corps/terreconst.h                     \
    src/librairies/dates/date.h                           \
    src/librairies/dates/dateconst.h                      \
    src/librairies/exceptions/message.h                   \
    src/librairies/exceptions/messageconst.h              \
    src/librairies/exceptions/previsatexception.h         \
    src/librairies/maths/maths.h                          \
    src/librairies/maths/mathsconst.h                     \
    src/librairies/maths/matrice3d.h                      \
    src/librairies/maths/vecteur3d.h                      \
    src/librairies/observateur/observateur.h              \
    src/librairies/systeme/logmessage.h                   \
    src/librairies/systeme/telechargement.h               \
#    src/previsions/flashs.h \
    src/previsions/prevision.h                            \
    src/previsions/previsionsconst.h


FORMS += \
    src/interface/onglets/donnees/informationsiss.ui       \
    src/interface/onglets/donnees/informationssatellite.ui \
    src/interface/onglets/donnees/recherchesatellite.ui    \
    src/interface/onglets/general/general.ui               \
    src/interface/onglets/onglets.ui                       \
    src/interface/onglets/osculateurs/osculateurs.ui       \
    src/interface/onglets/previsions/evenementsorbitaux.ui \
    src/interface/onglets/previsions/flashs.ui             \
    src/interface/onglets/previsions/previsionspassage.ui  \
    src/interface/onglets/previsions/transits.ui           \
    src/interface/options/options.ui                       \
    src/interface/outils/outils.ui                         \
    src/interface/previsat.ui

win32|win64 {
    SOURCES += src/interface/onglets/telescope/suivitelescope.cpp
    HEADERS += src/interface/onglets/telescope/suivitelescope.h
    FORMS += src/interface/onglets/telescope/suivitelescope.ui
}

OTHER_FILES += icone.rc

RESOURCES += resources.qrc


equals(BUILD_TEST, true) {

    message("Building test configuration")

    QT += testlib
    TARGET = PreviSatTest

    CONFIG += qt warn_on depend_includepath testcase

    DEFINES += CLEANUP_TEST=$$CLEANUP_TEST

    CONFIG(debug, debug|release) {
        DESTDIR = TestPreviSat/debug

        equals(COV_TEST, true) {
            QMAKE_CXXFLAGS += --coverage
            QMAKE_LFLAGS += --coverage
        }
    } else {
        DESTDIR = TestPreviSat/release
    }

SOURCES += \
    test/src/librairies/corps/satellite/conditioneclipsetest.cpp    \
    test/src/librairies/corps/satellite/donneestest.cpp             \
    test/src/librairies/corps/satellite/elementsosculateurstest.cpp \
    test/src/librairies/corps/satellite/evenementstest.cpp          \
    test/src/librairies/corps/satellite/gpformattest.cpp            \
    test/src/librairies/corps/satellite/magnitudetest.cpp           \
    test/src/librairies/corps/satellite/phasagetest.cpp             \
    test/src/librairies/corps/satellite/satellitetest.cpp           \
    test/src/librairies/corps/satellite/sgp4test.cpp                \
    test/src/librairies/corps/satellite/signaltest.cpp              \
    test/src/librairies/corps/satellite/tletest.cpp                 \
    test/src/librairies/corps/systemesolaire/lunetest.cpp           \
    test/src/librairies/corps/systemesolaire/planetetest.cpp        \
    test/src/librairies/corps/systemesolaire/soleiltest.cpp         \
    test/src/librairies/dates/datetest.cpp                          \
    test/src/librairies/maths/mathstest.cpp                         \
    test/src/librairies/observateur/observateurtest.cpp             \
    test/src/librairies/systeme/logmessagetest.cpp                  \
    test/src/librairies/systeme/telechargementtest.cpp              \
    test/src/testtools.cpp                                          \
    test/src/tst_previsattest.cpp

HEADERS += \
    test/src/librairies/corps/satellite/conditioneclipsetest.h    \
    test/src/librairies/corps/satellite/donneestest.h             \
    test/src/librairies/corps/satellite/elementsosculateurstest.h \
    test/src/librairies/corps/satellite/evenementstest.h          \
    test/src/librairies/corps/satellite/gpformattest.h            \
    test/src/librairies/corps/satellite/magnitudetest.h           \
    test/src/librairies/corps/satellite/phasagetest.h             \
    test/src/librairies/corps/satellite/satellitetest.h           \
    test/src/librairies/corps/satellite/sgp4test.h                \
    test/src/librairies/corps/satellite/signaltest.h              \
    test/src/librairies/corps/satellite/tletest.h                 \
    test/src/librairies/corps/systemesolaire/lunetest.h           \
    test/src/librairies/corps/systemesolaire/planetetest.h        \
    test/src/librairies/corps/systemesolaire/soleiltest.h         \
    test/src/librairies/dates/datetest.h                          \
    test/src/librairies/maths/mathstest.h                         \
    test/src/librairies/observateur/observateurtest.h             \
    test/src/librairies/systeme/logmessagetest.h                  \
    test/src/librairies/systeme/telechargementtest.h              \
    test/src/testtools.h

} else {

    message("Building software configuration")

    QT += core gui network
    TARGET = PreviSat

    CONFIG(debug, debug|release) {
        DESTDIR = debug
    } else {
        DESTDIR = release
    }

    #CONFIG += lrelease

    ICON = resources/interface/icone.ico
    win32:RC_FILE = icone.rc
    mac:ICON = resources/interface/icone.icns

    SOURCES += \
        src/main.cpp
}


OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR     = $$DESTDIR/moc
UI_DIR      = $$DESTDIR/ui


QMAKE_CXXFLAGS += -Wconversion -Wfloat-equal -pipe -W -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wextra -Wformat \
    -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wmain -Wmissing-declarations \
    -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-noreturn -Wno-deprecated-declarations -Wpacked -Wparentheses \
    -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wstack-protector -Wswitch -Wswitch-default \
    -Wswitch-enum -Wtrigraphs -Wundef -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunused -Wunused-parameter -Wvariadic-macros -Wwrite-strings

QMAKE_CXXFLAGS_RELEASE += -Os -fbounds-check -fbranch-target-load-optimize -fcaller-saves -fcommon -fcprop-registers -fcrossjumping -floop-optimize \
    -foptimize-register-move -fpeephole -fpeephole2 -frerun-cse-after-loop -fstrength-reduce -malign-double -s

