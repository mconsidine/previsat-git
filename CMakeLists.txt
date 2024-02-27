#
#     PreviSat, Satellite tracking software
#     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
# >    CMakeLists.txt
#
# Localisation
# >
#
# Description
# >    Fichier de configuration du projet, en remplacement de PreviSat.pro
#
# Auteur
# >    Astropedia
#
# Date de creation
# >    21 juin 2023
#
# Date de revision
# >

cmake_minimum_required(VERSION 3.24)


#----------------------------------
set(VERSION 7.0.0.0)
set(QT_PREFERRED_VERSION 6.6.2)
set(ANNEES_DEV 2005-2024)
set(NB_COL_DONNEES 249)

if(NOT PORTABLE_BUILD)
    set(PORTABLE_BUILD false)
endif()

if(NOT BUILD_TEST)
    set(BUILD_TEST false)
endif()

if(NOT CLEANUP_TEST)
    set(CLEANUP_TEST true)
endif()

if(NOT COVERAGE_TEST)
    set(COVERAGE_TEST false)
endif()
#----------------------------------


set(TARGET PreviSat)
set(DOMAIN_NAME http://previsat.free.fr/)
set(ORG_NAME Astropedia)
set(MAILTO previsat.app@gmail.com)

# Definition du projet
project(${TARGET} VERSION ${VERSION} LANGUAGES CXX)

set(VER_MAJ ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Icone
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(app_icon "${CMAKE_CURRENT_SOURCE_DIR}/icone.rc")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX_BUNDLE_ICON_FILE icone.icns)
    set(app_icon "${CMAKE_CURRENT_SOURCE_DIR}/resources/interface/icone.icns")
    set_source_files_properties(${app_icon}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
endif()

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Concurrent Multimedia Network PrintSupport Widgets Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Core Multimedia Network PrintSupport Widgets Xml)

# Verification de la version de Qt
string(SUBSTRING ${QT_PREFERRED_VERSION} 0 1 QT_PREFERRED_VERSION_MAJOR)

if(NOT ${QT_VERSION_MAJOR} EQUAL ${QT_PREFERRED_VERSION_MAJOR})
    message(FATAL_ERROR "Qt ${QT_VERSION} is not suited to compile ${TARGET}. Use Qt ${QT_PREFERRED_VERSION} in preference")
endif()


# Ajout des definitions
add_compile_definitions(
    APP_NAME="${TARGET}"
    DOMAIN_NAME="${DOMAIN_NAME}"
    ORG_NAME="${ORG_NAME}"
    MAILTO="${MAILTO}"
    VERSION="${VERSION}"
    VER_MAJ="${VER_MAJ}"
    ANNEES_DEV="${ANNEES_DEV}"
    NB_LIGNES="${NB_LIGNES}"
    NB_COL_DONNEES="${NB_COL_DONNEES}"
)

add_definitions(
    -DPORTABLE_BUILD=${PORTABLE_BUILD}
    -DBUILD_TEST=${BUILD_TEST}
    -DCLEANUP_TEST=${CLEANUP_TEST}
    -DCOVERAGE_TEST=${COVERAGE_TEST}
)

include_directories(src)

# Definition des librairies
file(GLOB_RECURSE
    LIBRARY_SOURCES
    src/librairies/*.cpp
    src/librairies/*.h
)

# Definition des classes de configuration
file(GLOB_RECURSE
    CONFIGURATION_SOURCES
    src/configuration/*.cpp
    src/configuration/*.h
)

# Definition des classes de previsions
file(GLOB_RECURSE
    PREVISIONS_SOURCES
    src/previsions/*.cpp
    src/previsions/*.h
)

if(${BUILD_TEST})

    message("Build test configuration")

    enable_testing()

    if(${COVERAGE_TEST})
        set(COV_FLAGS -g -fprofile-arcs -ftest-coverage)
        set(COV_LINK_FLAGS -coverage -lgcov)
    endif()

    set(TARGET ${TARGET}Test)

    include_directories(test/src)

    find_package(QT NAMES Qt6 REQUIRED COMPONENTS Test)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

    file(GLOB_RECURSE
        TEST_SOURCES
        test/src/*.cpp
        test/src/*.h
    )

    add_executable(${TARGET}
        ${TEST_SOURCES}
        ${PREVISIONS_SOURCES}
        ${CONFIGURATION_SOURCES}
        ${LIBRARY_SOURCES}
        src/interface/afficherresultats.cpp
        src/interface/afficherresultats.h
        src/interface/ciel/ciel.cpp
        src/interface/ciel/ciel.h
        src/interface/onglets/general/general.cpp
        src/interface/onglets/general/general.h
        src/interface/onglets/osculateurs/osculateurs.cpp
        src/interface/onglets/osculateurs/osculateurs.h
        src/interface/onglets/donnees/informationssatellite.cpp
        src/interface/onglets/donnees/informationssatellite.h
        src/interface/onglets/donnees/recherchesatellite.cpp
        src/interface/onglets/donnees/recherchesatellite.h
        src/interface/onglets/previsions/calculsflashs.cpp
        src/interface/onglets/previsions/calculsflashs.h
    )

    add_test(NAME ${TARGET} COMMAND ${TARGET})

    target_link_libraries(
        ${TARGET}
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Xml
        ${COV_LINK_FLAGS}
    )

else()

    if(${PORTABLE_BUILD})
        message("Build portable software configuration")
    else()
        message("Build software configuration")
    endif()

    # Definition des classes d'interface graphique
    file(GLOB_RECURSE
        INTERFACE_SOURCES
        src/interface/*.cpp
        src/interface/*.h
        src/interface/*.ui
    )

    qt_add_resources(${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

    qt_add_executable(${TARGET}
        MANUAL_FINALIZATION
        src/main.cpp
        ${INTERFACE_SOURCES}
        ${PREVISIONS_SOURCES}
        ${CONFIGURATION_SOURCES}
        ${LIBRARY_SOURCES}
        ${app_icon}
        resources.qrc
    )

    target_link_libraries(${TARGET}
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::PrintSupport
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Xml
    )

    qt_finalize_executable(${TARGET})

    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS LinguistTools)
    qt_add_translations(${TARGET}
        TS_FILES translations/PreviSat_en.ts translations/PreviSat_ja.ts
    )

    add_dependencies(${TARGET} ${TARGET}_lupdate)
    add_dependencies(${TARGET} update_translations)

    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(RELEASE_FLAGS
            -Os
            -fbounds-check
            -fbranch-target-load-optimize
            -fcaller-saves
            -fcommon
            -fcprop-registers
            -fcrossjumping
            -floop-optimize
            -foptimize-register-move
            -fpeephole
            -fpeephole2
            -frerun-cse-after-loop
            -fstrength-reduce
            -malign-double
            -s
        )
    endif()
endif()

target_compile_options(${TARGET}
    PUBLIC
    ${COV_FLAGS}
    ${RELEASE_FLAGS}
    -W
    -Wall
    -Wcast-align
    -Wcast-qual
    -Wchar-subscripts
    -Wcomment
    -Wconversion
    -Wextra
    -Wfloat-equal
    -Wformat
    -Wformat-nonliteral
    -Wformat-security
    -Wformat-y2k
    -Wformat=2
    -Wignored-qualifiers
    -Wimport
    -Winit-self
    -Winvalid-pch
    -Wmain
    -Wmissing-declarations
    -Wmissing-field-initializers
    -Wmissing-format-attribute
    -Wmissing-noreturn
    -Wno-deprecated-declarations
    -Wpacked
    -Wparentheses
    -Wpointer-arith
    -Wredundant-decls
    -Wreturn-type
    -Wsequence-point
    -Wshadow
    -Wsign-compare
    -Wstack-protector
    -Wswitch
    -Wswitch-default
    -Wswitch-enum
    -Wtrigraphs
    -Wundef
    -Wuninitialized
    -Wunknown-pragmas
    -Wunreachable-code
    -Wunused
    -Wunused-parameter
    -Wvariadic-macros
    -Wwrite-strings
    -pipe
)

get_target_property(MAIN_CFLAGS ${TARGET} COMPILE_OPTIONS)
message("-- Compiler flags are ${MAIN_CFLAGS}")

add_custom_command(TARGET ${TARGET} POST_BUILD
COMMAND echo Compiler flags are ${MAIN_CFLAGS})
