#
#     PreviSat, Satellite tracking software
#     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
# >    25 novembre 2023

#-------------------------------------------------
VER_MAJ      = 6.1
VERSION      = 6.1.1.1
ANNEES_DEV   = 2005-2023
TRANSLATIONS = translations/PreviSat_en.ts translations/PreviSat_ja.ts
!defined(BUILD_TEST,   var):BUILD_TEST   = false
!defined(CLEANUP_TEST, var):CLEANUP_TEST = true
!defined(COV_TEST,     var):COV_TEST     = false
#-------------------------------------------------

TARGET = PreviSat
TEMPLATE = app

!equals(QT_MAJOR_VERSION, 6) {
    error("Qt $${QT_VERSION} is not suited to compile $${TARGET}. Use Qt 6.4.3 in preference")
}

QT += concurrent multimedia printsupport widgets xml
greaterThan(QT_GCC_MAJOR_VERSION, 5) {
    QMAKE_CXXFLAGS += -std=c++17
    CONFIG += c++17
}


ORGANIZATION = Astropedia
DOMAIN = http://previsat.free.fr/
MAIL = previsat.app@gmail.com

VERSION_STR     = '\\"$${VERSION}\\"'
VER_MAJ_STR     = '\\"$${VER_MAJ}\\"'
ANNEES_DEV_STR  = '\\"$${ANNEES_DEV}\\"'
APP_NAME_STR    = '\\"$${TARGET}\\"'
ORG_NAME_STR    = '\\"$${ORGANIZATION}\\"'
DOMAIN_NAME_STR = '\\"$${DOMAIN}\\"'
MAIL_STR        = '\\"$${MAIL}\\"'
NBL_STR         = '\\"$${NB_LINES}\\"'


DEFINES += APP_VERSION=\"$${VERSION_STR}\" \
    APP_VER_MAJ=\"$${VER_MAJ_STR}\"        \
    APP_ANNEES_DEV=\"$${ANNEES_DEV_STR}\"  \
    APP_NAME=\"$${APP_NAME_STR}\"          \
    ORG_NAME=\"$${ORG_NAME_STR}\"          \
    DOMAIN_NAME=\"$${DOMAIN_NAME_STR}\"    \
    MAILTO=\"$${MAIL_STR}\"                \
    BUILD_TEST=$${BUILD_TEST}              \
    COVERAGE_TEST=$${COV_TEST}             \
    NB_LIGNES=$${NBL_STR}                  \
    QT_DISABLE_DEPRECATED_BEFORE=0x060000  \
    QT_MESSAGELOGCONTEXT

INCLUDEPATH += src


SOURCES += \
    src/configuration/configuration.cpp                            \
    src/configuration/evenementsstationspatiale.cpp                \
    src/configuration/fichierobs.cpp                               \
    src/configuration/gestionnairexml.cpp                          \
    src/interface/afficherresultats.cpp                            \
    src/interface/apropos/apropos.cpp                              \
    src/interface/carte/carte.cpp                                  \
    src/interface/carte/coordiss.cpp                               \
    src/interface/carte/itemgroup.cpp                              \
    src/interface/ciel/ciel.cpp                                    \
    src/interface/informations/informations.cpp                    \
    src/interface/logging/logging.cpp                              \
    src/interface/onglets/antenne/antenne.cpp                      \
    src/interface/onglets/donnees/informationsiss.cpp              \
    src/interface/onglets/donnees/informationssatellite.cpp        \
    src/interface/onglets/donnees/recherchesatellite.cpp           \
    src/interface/onglets/general/general.cpp                      \
    src/interface/onglets/onglets.cpp                              \
    src/interface/onglets/osculateurs/osculateurs.cpp              \
    src/interface/onglets/previsions/calculsevenementsorbitaux.cpp \
    src/interface/onglets/previsions/calculsflashs.cpp             \
    src/interface/onglets/previsions/calculsprevisions.cpp         \
    src/interface/onglets/previsions/calculsstarlink.cpp           \
    src/interface/onglets/previsions/calculstransits.cpp           \
    src/interface/onglets/telescope/ajustementdates.cpp            \
    src/interface/options/options.cpp                              \
    src/interface/options/telechargementoptions.cpp                \
    src/interface/outils/outils.cpp                                \
    src/interface/previsat.cpp                                     \
    src/interface/radar/radar.cpp                                  \
    src/librairies/corps/corps.cpp                                 \
    src/librairies/corps/etoiles/constellation.cpp                 \
    src/librairies/corps/etoiles/etoile.cpp                        \
    src/librairies/corps/etoiles/ligneconstellation.cpp            \
    src/librairies/corps/satellite/conditioneclipse.cpp            \
    src/librairies/corps/satellite/donnees.cpp                     \
    src/librairies/corps/satellite/elementsosculateurs.cpp         \
    src/librairies/corps/satellite/evenements.cpp                  \
    src/librairies/corps/satellite/gpformat.cpp                    \
    src/librairies/corps/satellite/magnitude.cpp                   \
    src/librairies/corps/satellite/phasage.cpp                     \
    src/librairies/corps/satellite/satellite.cpp                   \
    src/librairies/corps/satellite/sgp4.cpp                        \
    src/librairies/corps/satellite/signal.cpp                      \
    src/librairies/corps/satellite/tle.cpp                         \
    src/librairies/corps/systemesolaire/lune.cpp                   \
    src/librairies/corps/systemesolaire/planete.cpp                \
    src/librairies/corps/systemesolaire/soleil.cpp                 \
    src/librairies/dates/date.cpp                                  \
    src/librairies/exceptions/message.cpp                          \
    src/librairies/exceptions/previsatexception.cpp                \
    src/librairies/maths/maths.cpp                                 \
    src/librairies/maths/matrice3d.cpp                             \
    src/librairies/maths/vecteur3d.cpp                             \
    src/librairies/observateur/observateur.cpp                     \
    src/librairies/systeme/logmessage.cpp                          \
    src/librairies/systeme/telechargement.cpp                      \
    src/previsions/evenementsorbitaux.cpp                          \
    src/previsions/flashs.cpp                                      \
    src/previsions/prevision.cpp                                   \
    src/previsions/telescope.cpp                                   \
    src/previsions/transits.cpp


HEADERS += \
    src/configuration/categorieelementsorbitaux.h                \
    src/configuration/configuration.h                            \
    src/configuration/configurationconst.h                       \
    src/configuration/evenementsstation.h                        \
    src/configuration/evenementsstationspatiale.h                \
    src/configuration/fichierobs.h                               \
    src/configuration/frequencesradio.h                          \
    src/configuration/gestionnairexml.h                          \
    src/configuration/satellitesflashs.h                         \
    src/configuration/satellitesstarlink.h                       \
    src/configuration/satellitetdrs.h                            \
    src/interface/afficherresultats.h                            \
    src/interface/carte/carte.h                                  \
    src/interface/carte/coordiss.h                               \
    src/interface/carte/itemgroup.h                              \
    src/interface/ciel/ciel.h                                    \
    src/interface/listwidgetitem.h                               \
    src/interface/apropos/apropos.h                              \
    src/interface/informations/informations.h                    \
    src/interface/logging/logging.h                              \
    src/interface/onglets/antenne/antenne.h                      \
    src/interface/onglets/donnees/informationsiss.h              \
    src/interface/onglets/donnees/informationssatellite.h        \
    src/interface/onglets/donnees/recherchesatellite.h           \
    src/interface/onglets/general/general.h                      \
    src/interface/onglets/onglets.h                              \
    src/interface/onglets/osculateurs/osculateurs.h              \
    src/interface/onglets/previsions/calculsevenementsorbitaux.h \
    src/interface/onglets/previsions/calculsflashs.h             \
    src/interface/onglets/previsions/calculsprevisions.h         \
    src/interface/onglets/previsions/calculsstarlink.h           \
    src/interface/onglets/previsions/calculstransits.h           \
    src/interface/onglets/telescope/ajustementdates.h            \
    src/interface/options/options.h                              \
    src/interface/options/telechargementoptions.h                \
    src/interface/outils/outils.h                                \
    src/interface/previsat.h                                     \
    src/interface/radar/radar.h                                  \
    src/librairies/corps/corps.h                                 \
    src/librairies/corps/corpsconst.h                            \
    src/librairies/corps/ephemerides.h                           \
    src/librairies/corps/etoiles/constellation.h                 \
    src/librairies/corps/etoiles/etoile.h                        \
    src/librairies/corps/etoiles/ligneconstellation.h            \
    src/librairies/corps/satellite/conditioneclipse.h            \
    src/librairies/corps/satellite/donnees.h                     \
    src/librairies/corps/satellite/elementsorbitaux.h            \
    src/librairies/corps/satellite/elementsosculateurs.h         \
    src/librairies/corps/satellite/evenements.h                  \
    src/librairies/corps/satellite/evenementsconst.h             \
    src/librairies/corps/satellite/gpformat.h                    \
    src/librairies/corps/satellite/magnitude.h                   \
    src/librairies/corps/satellite/phasage.h                     \
    src/librairies/corps/satellite/satellite.h                   \
    src/librairies/corps/satellite/sgp4.h                        \
    src/librairies/corps/satellite/sgp4const.h                   \
    src/librairies/corps/satellite/signal.h                      \
    src/librairies/corps/satellite/tle.h                         \
    src/librairies/corps/systemesolaire/lune.h                   \
    src/librairies/corps/systemesolaire/luneconst.h              \
    src/librairies/corps/systemesolaire/planete.h                \
    src/librairies/corps/systemesolaire/planeteconst.h           \
    src/librairies/corps/systemesolaire/soleil.h                 \
    src/librairies/corps/systemesolaire/soleilconst.h            \
    src/librairies/corps/terreconst.h                            \
    src/librairies/dates/date.h                                  \
    src/librairies/dates/dateconst.h                             \
    src/librairies/exceptions/message.h                          \
    src/librairies/exceptions/messageconst.h                     \
    src/librairies/exceptions/previsatexception.h                \
    src/librairies/maths/maths.h                                 \
    src/librairies/maths/mathsconst.h                            \
    src/librairies/maths/matrice3d.h                             \
    src/librairies/maths/vecteur3d.h                             \
    src/librairies/observateur/observateur.h                     \
    src/librairies/systeme/logmessage.h                          \
    src/librairies/systeme/telechargement.h                      \
    src/previsions/evenementsorbitaux.h                          \
    src/previsions/flashs.h                                      \
    src/previsions/prevision.h                                   \
    src/previsions/previsionsconst.h                             \
    src/previsions/telescope.h                                   \
    src/previsions/transits.h


FORMS += \
    src/interface/afficherresultats.ui                            \
    src/interface/apropos/apropos.ui                              \
    src/interface/carte/carte.ui                                  \
    src/interface/carte/coordiss.ui                               \
    src/interface/ciel/ciel.ui                                    \
    src/interface/informations/informations.ui                    \
    src/interface/logging/logging.ui                              \
    src/interface/onglets/antenne/antenne.ui                      \
    src/interface/onglets/donnees/informationsiss.ui              \
    src/interface/onglets/donnees/informationssatellite.ui        \
    src/interface/onglets/donnees/recherchesatellite.ui           \
    src/interface/onglets/general/general.ui                      \
    src/interface/onglets/onglets.ui                              \
    src/interface/onglets/osculateurs/osculateurs.ui              \
    src/interface/onglets/previsions/calculsevenementsorbitaux.ui \
    src/interface/onglets/previsions/calculsflashs.ui             \
    src/interface/onglets/previsions/calculsprevisions.ui         \
    src/interface/onglets/previsions/calculsstarlink.ui           \
    src/interface/onglets/previsions/calculstransits.ui           \
    src/interface/onglets/telescope/ajustementdates.ui            \
    src/interface/options/options.ui                              \
    src/interface/options/telechargementoptions.ui                \
    src/interface/outils/outils.ui                                \
    src/interface/previsat.ui                                     \
    src/interface/radar/radar.ui

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
    test/src/interface/generaltest.cpp                              \
    test/src/interface/informationstest.cpp                         \
    test/src/interface/osculateurstest.cpp                          \
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
    test/src/previsions/evenementsorbitauxtest.cpp                  \
    test/src/previsions/flashstest.cpp                              \
    test/src/previsions/previsiontest.cpp                           \
    test/src/previsions/starlinktest.cpp                            \
    test/src/previsions/telescopetest.cpp                           \
    test/src/previsions/transitstest.cpp                            \
    test/src/testtools.cpp                                          \
    test/src/tst_previsattest.cpp

HEADERS += \
    test/src/interface/generaltest.h                              \
    test/src/interface/informationstest.h                         \
    test/src/interface/osculateurstest.h                          \
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
    test/src/previsions/evenementsorbitauxtest.h                  \
    test/src/previsions/flashstest.h                              \
    test/src/previsions/previsiontest.h                           \
    test/src/previsions/starlinktest.h                            \
    test/src/previsions/telescopetest.h                           \
    test/src/previsions/transitstest.h                            \
    test/src/testtools.h

} else {

    message("Building software configuration")

    QT += core gui network

    CONFIG(debug, debug|release) {
        DESTDIR = debug
    } else {
        DESTDIR = release
    }

    CONFIG += lrelease

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

