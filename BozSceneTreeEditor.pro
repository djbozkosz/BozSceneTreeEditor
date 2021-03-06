# qt
QT                 += core gui widgets
TEMPLATE            = app

TARGET              = BozSceneTreeEditor
VERSION             = 1.0.1
RC_ICONS            = images/icons/icon.ico

CONFIG(release, debug|release) {
	DEFINES        += RELEASE_BUILD
}

DEFINES            += VERSION=\"\\\"$${VERSION}\\\"\"

# compiler options
QMAKE_CXXFLAGS     += -std=c++11

QMAKE_CXXFLAGS     += -Wall
QMAKE_CXXFLAGS     += -Wextra
QMAKE_CXXFLAGS     += -pedantic

CONFIG(release, debug|release) {
	QMAKE_CXXFLAGS += -O3
} else {
	QMAKE_CXXFLAGS += -O0
}

# includes
INCLUDEPATH +=\
	src

include(src/Application.pri)
include(src/GUI.pri)
include(src/Scene.pri)
include(src/Utility.pri)
