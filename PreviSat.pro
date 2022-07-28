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
# >    26 juillet 2022

#-------------------------------------------------
VER_MAJ      = 5.0
VERSION      = 5.0.1.4
ANNEES_DEV   = 2005-2022
TRANSLATIONS = translations/PreviSat_en.ts
BUILD_TEST   = false
CLEANUP_TEST = true
#-------------------------------------------------

TARGET = PreviSat
TEMPLATE = app

equals(QT_MAJOR_VERSION, 5) {
    lessThan(QT_MINOR_VERSION, 4) {
        error("Qt $${QT_VERSION} is not suited to compile $${TARGET}. Use Qt 5.15.2 in preference")
    }
} else {
    error("Qt $${QT_VERSION} is not suited to compile $${TARGET}. Use Qt 5.15.2 in preference")
}

QT += concurrent

equals(QT_MAJOR_VERSION, 5): QT += multimedia printsupport widgets xml
greaterThan(QT_GCC_MAJOR_VERSION, 4) {
    QMAKE_CXXFLAGS += -std=c++11
    CONFIG += c++11
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
    QT_DEPRECATED_WARNINGS                \
    BUILD_TEST=$$BUILD_TEST

INCLUDEPATH += src

unix|macx {
    LIBS += -lz
}


SOURCES += \
    src/configuration/configuration.cpp                    \
    src/interface/afficher.cpp                             \
    src/interface/apropos.cpp                              \
    src/interface/carte.cpp                                \
    src/interface/ciel.cpp                                 \
    src/interface/coordiss.cpp                             \
    src/interface/gestionnairetle.cpp                      \
    src/interface/informations.cpp                         \
    src/interface/onglets.cpp                              \
    src/interface/previsat.cpp                             \
    src/interface/radar.cpp                                \
    src/interface/telecharger.cpp                          \
    src/librairies/corps/corps.cpp                         \
    src/librairies/corps/etoiles/constellation.cpp         \
    src/librairies/corps/etoiles/etoile.cpp                \
    src/librairies/corps/etoiles/ligneconstellation.cpp    \
    src/librairies/corps/satellite/conditioneclipse.cpp    \
    src/librairies/corps/satellite/donnees.cpp             \
    src/librairies/corps/satellite/elementsosculateurs.cpp \
    src/librairies/corps/satellite/evenements.cpp          \
    src/librairies/corps/satellite/magnitude.cpp           \
    src/librairies/corps/satellite/phasage.cpp             \
    src/librairies/corps/satellite/satellite.cpp           \
    src/librairies/corps/satellite/sgp4.cpp                \
    src/librairies/corps/satellite/signal.cpp              \
    src/librairies/corps/satellite/tle.cpp                 \
    src/librairies/corps/systemesolaire/lune.cpp           \
    src/librairies/corps/systemesolaire/planete.cpp        \
    src/librairies/corps/systemesolaire/soleil.cpp         \
    src/librairies/dates/date.cpp                          \
    src/librairies/exceptions/message.cpp                  \
    src/librairies/exceptions/previsatexception.cpp        \
    src/librairies/maths/maths.cpp                         \
    src/librairies/maths/matrice3d.cpp                     \
    src/librairies/maths/vecteur3d.cpp                     \
    src/librairies/observateur/observateur.cpp             \
    src/librairies/systeme/decompression.cpp               \
    src/previsions/evenementsorbitaux.cpp                  \
    src/previsions/flashs.cpp                              \
    src/previsions/prevision.cpp                           \
    src/previsions/telescope.cpp                           \
    src/previsions/transitsiss.cpp


HEADERS += \
    src/configuration/configuration.h                    \
    src/interface/afficher.h                             \
    src/interface/apropos.h                              \
    src/interface/carte.h                                \
    src/interface/ciel.h                                 \
    src/interface/coordiss.h                             \
    src/interface/gestionnairetle.h                      \
    src/interface/listwidgetitem.h                       \
    src/interface/informations.h                         \
    src/interface/onglets.h                              \
    src/interface/previsat.h                             \
    src/interface/radar.h                                \
    src/interface/telecharger.h                          \
    src/librairies/corps/corps.h                         \
    src/librairies/corps/etoiles/constellation.h         \
    src/librairies/corps/etoiles/etoile.h                \
    src/librairies/corps/etoiles/ligneconstellation.h    \
    src/librairies/corps/satellite/conditioneclipse.h    \
    src/librairies/corps/satellite/donnees.h             \
    src/librairies/corps/satellite/elementsosculateurs.h \
    src/librairies/corps/satellite/evenements.h          \
    src/librairies/corps/satellite/magnitude.h           \
    src/librairies/corps/satellite/phasage.h             \
    src/librairies/corps/satellite/satellite.h           \
    src/librairies/corps/satellite/sgp4.h                \
    src/librairies/corps/satellite/sgp4const.h           \
    src/librairies/corps/satellite/signal.h              \
    src/librairies/corps/satellite/tle.h                 \
    src/librairies/corps/systemesolaire/lune.h           \
    src/librairies/corps/systemesolaire/luneconst.h      \
    src/librairies/corps/systemesolaire/planete.h        \
    src/librairies/corps/systemesolaire/planeteconst.h   \
    src/librairies/corps/systemesolaire/soleil.h         \
    src/librairies/corps/systemesolaire/soleilconst.h    \
    src/librairies/corps/systemesolaire/terreconst.h     \
    src/librairies/dates/date.h                          \
    src/librairies/dates/dateconst.h                     \
    src/librairies/exceptions/message.h                  \
    src/librairies/exceptions/messageconst.h             \
    src/librairies/exceptions/previsatexception.h        \
    src/librairies/maths/maths.h                         \
    src/librairies/maths/mathsconst.h                    \
    src/librairies/maths/matrice3d.h                     \
    src/librairies/maths/vecteur3d.h                     \
    src/librairies/observateur/observateur.h             \
    src/librairies/systeme/decompression.h               \
    src/previsions/evenementsorbitaux.h                  \
    src/previsions/flashs.h                              \
    src/previsions/prevision.h                           \
    src/previsions/previsionsconst.h                     \
    src/previsions/telescope.h                           \
    src/previsions/transitsiss.h


FORMS += \
    src/interface/afficher.ui        \
    src/interface/apropos.ui         \
    src/interface/carte.ui           \
    src/interface/ciel.ui            \
    src/interface/coordiss.ui        \
    src/interface/gestionnairetle.ui \
    src/interface/informations.ui    \
    src/interface/onglets.ui         \
    src/interface/previsat.ui        \
    src/interface/radar.ui           \
    src/interface/telecharger.ui

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
    } else {
        DESTDIR = TestPreviSat/release
    }

    SOURCES += \
        test/src/interface/ongletstest.cpp                       \
        test/src/librairies/corps/satellite/satellitetest.cpp    \
        test/src/librairies/corps/systemesolaire/lunetest.cpp    \
        test/src/librairies/corps/systemesolaire/planetetest.cpp \
        test/src/librairies/corps/systemesolaire/soleiltest.cpp  \
        test/src/librairies/dates/datetest.cpp                   \
        test/src/librairies/maths/mathstest.cpp                  \
        test/src/librairies/observateur/observateurtest.cpp      \
        test/src/librairies/systeme/decompressiontest.cpp        \
        test/src/previsions/evenementsorbitauxtest.cpp           \
        test/src/previsions/flashstest.cpp                       \
        test/src/previsions/previsiontest.cpp                    \
        test/src/previsions/telescopetest.cpp                    \
        test/src/previsions/transitsisstest.cpp                  \
        test/src/testtools.cpp                                   \
        test/src/tst_previsattest.cpp

    HEADERS += \
        test/src/interface/ongletstest.h                       \
        test/src/librairies/corps/satellite/satellitetest.h    \
        test/src/librairies/corps/systemesolaire/lunetest.h    \
        test/src/librairies/corps/systemesolaire/planetetest.h \
        test/src/librairies/corps/systemesolaire/soleiltest.h  \
        test/src/librairies/dates/datetest.h                   \
        test/src/librairies/maths/mathstest.h                  \
        test/src/librairies/observateur/observateurtest.h      \
        test/src/librairies/systeme/decompressiontest.h        \
        test/src/previsions/evenementsorbitauxtest.h           \
        test/src/previsions/flashstest.h                       \
        test/src/previsions/previsiontest.h                    \
        test/src/previsions/telescopetest.h                    \
        test/src/previsions/transitsisstest.h                  \
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

    CONFIG += lrelease

    ICON = resources/icone.ico
    win32:RC_FILE = icone.rc
    mac:ICON = resources/icone.icns

    SOURCES += \
        src/main.cpp
}


OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR     = $$DESTDIR/moc
UI_DIR      = $$DESTDIR/ui


QMAKE_CXXFLAGS += -Wconversion -Wfloat-equal -pipe -W -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wextra -Wformat \
    -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wmain -Wmissing-declarations \
    -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wno-deprecated-declarations \
    -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wstack-protector \
    -Wswitch -Wswitch-default -Wswitch-enum -Wtrigraphs -Wundef -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunused \
    -Wunused-parameter -Wvariadic-macros -Wwrite-strings

QMAKE_CXXFLAGS_RELEASE += -Os -fbounds-check -fbranch-target-load-optimize -fcaller-saves -fcommon -fcprop-registers -fcrossjumping -floop-optimize \
    -foptimize-register-move -fpeephole -fpeephole2 -frerun-cse-after-loop -fstrength-reduce -malign-double -s

