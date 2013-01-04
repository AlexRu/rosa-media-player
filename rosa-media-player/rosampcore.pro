TEMPLATE = lib
LANGUAGE = c++
CONFIG += qt warn_on release
QT += network

INCLUDEPATH += .. src/
DEPENDPATH = .. src/

DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

DEFINES += MINILIB NO_USE_INI_FILES

HEADERS += src/backendfactory.h \
	src/config.h \
        src/constants.h \
        src/global.h \
        src/helper.h \
        src/colorutils.h \
        src/paths.h \
        src/filters.h \
        src/ffmpegprocess.h \
        src/assstyles.h \
        src/recents.h \
        src/discname.h \
        src/urlhistory.h \
        src/subtracks.h \
        src/tracks.h \
        src/titletracks.h \
        src/mediadata.h \
        src/mediasettings.h \
        src/preferences.h \
        src/myprocess.h \
        src/mplayerversion.h \
        src/mplayerprocess.h \
        src/infoprovider.h \
        src/desktopinfo.h \
        src/mplayerwindow.h \
        src/youtube/retrieveyoutubeurl.h \
        src/core.h \
        src/corelib/smplayercorelib.h \
        src/simplehttp.h


SOURCES += src/backendfactory.cpp \ 
	src/global.cpp \
        src/helper.cpp \
        src/colorutils.cpp \
        src/paths.cpp \
        src/filters.cpp \
        src/ffmpegprocess.cpp \
        src/assstyles.cpp \
        src/recents.cpp \
        src/discname.cpp \
        src/urlhistory.cpp \
        src/subtracks.cpp \
        src/tracks.cpp \
        src/titletracks.cpp \
        src/mediadata.cpp \
        src/mediasettings.cpp \
        src/preferences.cpp \
        src/myprocess.cpp \
        src/mplayerversion.cpp \
        src/mplayerprocess.cpp \
        src/infoprovider.cpp \
        src/desktopinfo.cpp \
        src/mplayerwindow.cpp \
        src/core.cpp \
        src/corelib/smplayercorelib.cpp \
        src/youtube/retrieveyoutubeurl.cpp \
        src/simplehttp.cpp