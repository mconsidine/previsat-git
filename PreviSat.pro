#
#     PreviSat, Satellite tracking software
#     Copyright (C) 2005-2019  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
# >    1er mai 2019

#-------------------------------------------------
VER_MAJ = 5.0
VERSION = 5.0.0.0
ANNEES_DEV = 2005-2019
ZLIB_DIR = $$PWD/../../externe/zlib
TRANSLATIONS = PreviSat_en.ts
BUILDTEST = false
DEFINES -= QT_NO_DEBUG_OUTPUT
#-------------------------------------------------


QT += core gui network #webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += multimedia printsupport widgets
greaterThan(QT_GCC_MAJOR_VERSION, 4): QMAKE_CXXFLAGS += -std=c++11

TARGET = PreviSat
TEMPLATE = app

VERSIONSTR = '\\"$${VERSION}\\"'
VER_MAJSTR = '\\"$${VER_MAJ}\\"'
ANNEES_DEVSTR = '\\"$${ANNEES_DEV}\\"'

DEFINES += APPVERSION=\"$${VERSIONSTR}\" \
    APPVER_MAJ=\"$${VER_MAJSTR}\"        \
    APP_ANNEES_DEV=\"$${ANNEES_DEVSTR}\" \
    QT_DEPRECATED_WARNINGS               \
    BUILDTEST=$$BUILDTEST

INCLUDEPATH += src $$ZLIB_DIR/inc
CONFIG += c++11
LIBS += -L$$ZLIB_DIR/lib -lz


SOURCES += \
    src/configuration/configuration.cpp                    \
    src/interface/apropos.cpp                              \
    src/interface/carte.cpp                                \
    src/interface/onglets.cpp                              \
    src/interface/previsat.cpp                             \
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
    src/librairies/systeme/telechargement.cpp


HEADERS += \
    src/configuration/configuration.h                    \
    src/interface/apropos.h                              \
    src/interface/carte.h                                \
    src/interface/listwidgetitem.h                       \
    src/interface/onglets.h                              \
    src/interface/previsat.h                             \
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
    src/librairies/systeme/telechargement.h              \
    $$ZLIB_DIR/inc/zlib.h


FORMS += \
#    src/interface/afficher.ui        \
    src/interface/carte.ui           \
    src/interface/apropos.ui         \
#    src/interface/gestionnairetle.ui \
#    src/interface/informations.ui    \
    src/interface/onglets.ui         \
    src/interface/previsat.ui        \
    src/interface/telecharger.ui

OTHER_FILES += icone.rc

RESOURCES += resources.qrc


equals(BUILDTEST, true) {

    message("Building test configuration")

    QT += testlib
    TARGET = PreviSatTest

    CONFIG += qt warn_on depend_includepath testcase

    CONFIG(debug, debug|release) {
        DESTDIR = TestPreviSat/debug
    } else {
        DESTDIR = TestPreviSat/release
    }


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
        test/src/testtools.h


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
        test/src/testtools.cpp                                   \
        test/src/tst_previsattest.cpp


} else {

    message("Building software configuration")

    QT += core
    TARGET = PreviSat

    CONFIG(debug, debug|release) {
        DESTDIR = debug
    } else {
        DESTDIR = release
    }

    ICON = resources/icone.ico
    win32:RC_FILE = icone.rc
    mac:ICON = resources/icone.icns

    SOURCES += \
        src/main.cpp
}


OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/moc
UI_DIR = $$DESTDIR/ui


#CONFIG(debug, debug|release) {
#    QMAKE_CXXFLAGS += -Wmissing-declarations
#    mac {
#        QMAKE_CXXFLAGS += -O
#        QMAKE_CXXFLAGS -= -Wmissing-declarations
#    }
#}

QMAKE_CXXFLAGS += -std=c++0x -Wconversion -Wfloat-equal -pipe -W -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment \
    -Wextra -Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wmain \
    -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wno-deprecated-declarations \
    -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wstack-protector -Wswitch \
    -Wswitch-default -Wswitch-enum -Wtrigraphs -Wundef -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunused -Wunused-parameter \
    -Wvariadic-macros -Wwrite-strings

QMAKE_CXXFLAGS_RELEASE += -Os -fbounds-check -fbranch-target-load-optimize -fcaller-saves -fcommon -fcprop-registers -fcrossjumping -floop-optimize \
    -foptimize-register-move -fpeephole -fpeephole2 -frerun-cse-after-loop -fstrength-reduce -malign-double -s

