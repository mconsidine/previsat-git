#
#     PreviSat, Satellite tracking software
#     Copyright (C) 2005-2016  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
# >    1er mars 2016

#-------------------------------------------------
VER_MAJ = 3.5
VERSION = 3.5.2.4
ANNEES_DEV = 2005-2016
ZLIB_DIR = $$PWD/../../externe/zlib
TRANSLATIONS = PreviSat_en.ts
#-------------------------------------------------


QT += core gui network webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += multimedia printsupport webkitwidgets widgets

TARGET = PreviSat
TEMPLATE = app

ICON = resources/icone.ico

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -Wconversion -Wfloat-equal -Wmissing-declarations
    mac {
        QMAKE_CXXFLAGS += -O
        QMAKE_CXXFLAGS -= -Wmissing-declarations
    }
}

LIBS += -L$$ZLIB_DIR/lib -lz
win32 {
    RC_FILE = icone.rc
}

mac {
    ICON = resources/icone.icns
}

VERSIONSTR = '\\"$${VERSION}\\"'
VER_MAJSTR = '\\"$${VER_MAJ}\\"'
ANNEES_DEVSTR = '\\"$${ANNEES_DEV}\\"'

DEFINES += APPVERSION=\"$${VERSIONSTR}\" \
           APPVER_MAJ=\"$${VER_MAJSTR}\" \
           APP_ANNEES_DEV=\"$${ANNEES_DEVSTR}\"

INCLUDEPATH += $$ZLIB_DIR/inc


SOURCES += main.cpp                                    \
    previsat.cpp                                       \
    afficher.cpp                                       \
    apropos.cpp                                        \
    gestionnairetle.cpp                                \
    telecharger.cpp                                    \
    threadcalculs.cpp                                  \
    librairies/corps/corps.cpp                         \
    librairies/corps/etoiles/constellation.cpp         \
    librairies/corps/etoiles/etoile.cpp                \
    librairies/corps/etoiles/ligneconstellation.cpp    \
    librairies/corps/satellite/elementsosculateurs.cpp \
    librairies/corps/satellite/satellite.cpp           \
    librairies/corps/satellite/sgp4.cpp                \
    librairies/corps/satellite/tle.cpp                 \
    librairies/corps/systemesolaire/lune.cpp           \
    librairies/corps/systemesolaire/planete.cpp        \
    librairies/corps/systemesolaire/soleil.cpp         \
    librairies/dates/date.cpp                          \
    librairies/exceptions/message.cpp                  \
    librairies/exceptions/previsatexception.cpp        \
    librairies/maths/maths.cpp                         \
    librairies/maths/matrice3d.cpp                     \
    librairies/maths/vecteur3d.cpp                     \
    librairies/observateur/observateur.cpp             \
    previsions/conditions.cpp                          \
    previsions/evenements.cpp                          \
    previsions/flashs.cpp                              \
    previsions/iridium.cpp                             \
    previsions/metop.cpp                               \
    previsions/prevision.cpp                           \
    previsions/transitiss.cpp


HEADERS += previsat.h                                  \
    afficher.h                                         \
    apropos.h                                          \
    gestionnairetle.h                                  \
    telecharger.h                                      \
    threadcalculs.h                                    \
    librairies/corps/corps.h                           \
    librairies/corps/etoiles/constellation.h           \
    librairies/corps/etoiles/etoile.h                  \
    librairies/corps/etoiles/ligneconstellation.h      \
    librairies/corps/satellite/elementsosculateurs.h   \
    librairies/corps/satellite/satellite.h             \
    librairies/corps/satellite/sgp4.h                  \
    librairies/corps/satellite/tle.h                   \
    librairies/corps/systemesolaire/lune.h             \
    librairies/corps/systemesolaire/LuneConstants.h    \
    librairies/corps/systemesolaire/planete.h          \
    librairies/corps/systemesolaire/planeteConstants.h \
    librairies/corps/systemesolaire/soleil.h           \
    librairies/corps/systemesolaire/SoleilConstants.h  \
    librairies/corps/systemesolaire/TerreConstants.h   \
    librairies/dates/date.h                            \
    librairies/dates/dateConstants.h                   \
    librairies/exceptions/message.h                    \
    librairies/exceptions/messageConstants.h           \
    librairies/exceptions/previsatexception.h          \
    librairies/maths/maths.h                           \
    librairies/maths/mathsConstants.h                  \
    librairies/maths/matrice3d.h                       \
    librairies/maths/vecteur3d.h                       \
    librairies/observateur/observateur.h               \
    previsions/conditions.h                            \
    previsions/evenements.h                            \
    previsions/flashs.h                                \
    previsions/iridium.h                               \
    previsions/metop.h                                 \
    previsions/prevision.h                             \
    previsions/previsionsConstants.h                   \
    previsions/transitiss.h                            \
    $$ZLIB_DIR/inc/zlib.h


FORMS += previsat.ui   \
    afficher.ui        \
    apropos.ui         \
    gestionnairetle.ui \
    telecharger.ui

OTHER_FILES += icone.rc

RESOURCES += resources.qrc
