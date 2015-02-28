TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += .
INCLUDEPATH += src
INCLUDEPATH += senjo/src

unix:QMAKE_CXXFLAGS += -std=c++11
unix::LIBS += -lpthread

CONFIG(release, debug|release) {
  message(Release build!)
  DEFINES += NDEBUG
}

# deploy epd files with each build
include(epd.pri)

# add GITREV to QMAKE_CXXFLAGS
include(gitrev.pri)

SOURCES += \
    src/ClubFoot.cpp \
    src/HashTable.cpp \
    src/main.cpp \
    senjo/src/BackgroundCommand.cpp \
    senjo/src/ChessEngine.cpp \
    senjo/src/EngineOption.cpp \
    senjo/src/MoveFinder.cpp \
    senjo/src/Output.cpp \
    senjo/src/Threading.cpp \
    senjo/src/UCIAdapter.cpp

HEADERS += \
    src/ClubFoot.h \
    src/Move.h \
    src/Types.h \
    src/HashTable.h \
    senjo/src/BackgroundCommand.h \
    senjo/src/ChessEngine.h \
    senjo/src/ChessMove.h \
    senjo/src/EngineOption.h \
    senjo/src/MoveFinder.h \
    senjo/src/Output.h \
    senjo/src/Platform.h \
    senjo/src/Square.h \
    senjo/src/Threading.h \
    senjo/src/UCIAdapter.h
