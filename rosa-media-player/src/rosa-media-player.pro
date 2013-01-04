TEMPLATE = app
LANGUAGE = C++

#DEFINES += QT_NO_DEBUG_OUTPUT
#DEFINES += QT_NO_WARNING_OUTPUT

CONFIG += qt warn_on release

QT += network xml multimedia

RESOURCES = icons.qrc

INCLUDEPATH += findsubtitles videopreview
DEPENDPATH += findsubtitles videopreview

#DEFINES += USE_QXT
DEFINES += DOWNLOAD_SUBS
DEFINES += MIDI_SUPPORT

HEADERS += guiconfig.h \
        config.h \
        constants.h \
        svn_revision.h \
        version.h \
        global.h \
        paths.h \
        helper.h \
        colorutils.h \
        translator.h \
        subtracks.h \
        tracks.h \
        titletracks.h \
        discname.h \
        extensions.h \
        desktopinfo.h \
        myprocess.h \
        mplayerversion.h \
        mplayerprocess.h \
        infoprovider.h \
        mplayerwindow.h \
        mediadata.h \
        audioequalizerlist.h \
        mediasettings.h \
        assstyles.h \
        filters.h \
        preferences.h \
        filesettingsbase.h \
        filesettings.h \
        filesettingshash.h \
        tvsettings.h \
        images.h \
        inforeader.h \
        deviceinfo.h \
        recents.h \
        urlhistory.h \
        core.h \
        logwindow.h \
        infofile.h \
        seekwidget.h \
        mytablewidget.h \
        shortcutgetter.h \
        actionseditor.h \
        filechooser.h \
        preferencesdialog.h \
        mycombobox.h \
        tristatecombo.h \
        languages.h \
        selectcolorbutton.h \
        prefwidget.h \
        prefgeneral.h \
        filepropertiesdialog.h \
        playlist.h \
        playlistpreferences.h \
        playlistdock.h \
        recorder.h \
        screencapture.h \
        verticaltext.h \
        eqslider.h \
        videoequalizer.h \
        audioequalizer.h \
        myslider.h \
        timeslider.h \
        inputdvddirectory.h \
        inputurl.h \
        myaction.h \
        myactiongroup.h \
        myserver.h \
        myclient.h \
        filedialog.h \
        inputmplayerversion.h \
        about.h \
        errordialog.h \
        timedialog.h \
        simplehttp.h \
        findsubtitles/osparser.h \
        findsubtitles/findsubtitlesconfigdialog.h \
        findsubtitles/findsubtitleswindow.h \
        videopreview/videopreview.h \
        videopreview/videopreviewconfigdialog.h \
        favorites.h \
        tvlist.h \
        favoriteeditor.h \
        basegui.h \
        baseguiplus.h \
        floatingwidget.h \
        widgetactions.h \
        toolbareditor.h \
        defaultgui.h \
        smplayer.h \
    clhelp.h \
    splitvideo.h \
    splitvideoprocess.h\
    controlpanel.h \
    recorder.h \
    cutaudio.h \
    ffmpegprocess.h \
    prefsubtitles.h \
    backendbase.h \
    backendfactory.h \
    youtube/request.h \
    youtube/reply.h \
    youtube/requestmanager.h \
    youtube/ytvideoitemmodel.h \
    youtube/ytvideoitem.h \
    youtube/retrieveyoutubeurl.h \
    videosearch.h \
    videoitemdelegate.h \
    remoteimageloader.h


SOURCES	+= version.cpp \
        global.cpp \
        paths.cpp \
        helper.cpp \
        colorutils.cpp \
        translator.cpp \
        subtracks.cpp \
        tracks.cpp \
        titletracks.cpp \
        discname.cpp \
        extensions.cpp \
        desktopinfo.cpp \
        myprocess.cpp \
        mplayerversion.cpp \
        mplayerprocess.cpp \
        infoprovider.cpp \
        mplayerwindow.cpp \
        mediadata.cpp \
        mediasettings.cpp \
        assstyles.cpp \
        filters.cpp \
        preferences.cpp \
        filesettingsbase.cpp \
        filesettings.cpp \
        filesettingshash.cpp \
        tvsettings.cpp \
        images.cpp \
        inforeader.cpp \
        deviceinfo.cpp \
        recents.cpp \
        urlhistory.cpp \
        core.cpp \
        logwindow.cpp \
        infofile.cpp \
        seekwidget.cpp \
        mytablewidget.cpp \
        shortcutgetter.cpp \
        actionseditor.cpp \
        filechooser.cpp \
        preferencesdialog.cpp \
        mycombobox.cpp \
        tristatecombo.cpp \
        languages.cpp \
        selectcolorbutton.cpp \
        prefwidget.cpp \
        prefgeneral.cpp \
        filepropertiesdialog.cpp \
        playlist.cpp \
        playlistpreferences.cpp \
        playlistdock.cpp \
        recorder.cpp \
        screencapture.cpp \
        verticaltext.cpp \
        eqslider.cpp \
        videoequalizer.cpp \
        audioequalizer.cpp \
        myslider.cpp \
        timeslider.cpp \
        inputdvddirectory.cpp \
        inputurl.cpp \
        myaction.cpp \
        myactiongroup.cpp \
        myserver.cpp \
        myclient.cpp \
        filedialog.cpp \
        inputmplayerversion.cpp \
        about.cpp \
        errordialog.cpp \
        timedialog.cpp \
        simplehttp.cpp \
        findsubtitles/osparser.cpp \
        findsubtitles/findsubtitlesconfigdialog.cpp \
        findsubtitles/findsubtitleswindow.cpp \
        videopreview/videopreview.cpp \
        videopreview/videopreviewconfigdialog.cpp \
        favorites.cpp \
        tvlist.cpp \
        favoriteeditor.cpp \
        basegui.cpp \
        baseguiplus.cpp \
        floatingwidget.cpp \
        widgetactions.cpp \
        toolbareditor.cpp \
        defaultgui.cpp \
        clhelp.cpp \
        smplayer.cpp \
        main.cpp \
    splitvideo.cpp \
    splitvideoprocess.cpp\
    controlpanel.cpp \
    cutaudio.cpp \
    ffmpegprocess.cpp \
    prefsubtitles.cpp \
    backendfactory.cpp \
    youtube/request.cpp \
    youtube/reply.cpp \
    youtube/requestmanager.cpp \
    youtube/ytvideoitemmodel.cpp \
    youtube/ytvideoitem.cpp \
    youtube/retrieveyoutubeurl.cpp \
    videosearch.cpp \
    videoitemdelegate.cpp \
    remoteimageloader.cpp

#libqxt
contains(DEFINES, USE_QXT) {
        CONFIG  += qxt
        QXT     += core11a75e9840a827bdc3cf8206fe19976f46283ea5
}

FORMS = inputdvddirectory.ui \
        logwindowbase.ui filepropertiesdialog.ui \
        eqslider.ui seekwidget.ui inputurl.ui \
        preferencesdialog.ui prefgeneral.ui \
        favoriteeditor.ui \
        about.ui inputmplayerversion.ui errordialog.ui timedialog.ui \
        playlistpreferences.ui filechooser.ui \
        findsubtitles/findsubtitleswindow.ui findsubtitles/findsubtitlesconfigdialog.ui \
        videopreview/videopreviewconfigdialog.ui \
        controlpanel.ui \
        splitvideo.ui \
        cutaudio.ui \
        prefsubtitles.ui \
    videosearch.ui

TRANSLATIONS = translations/rosamp_es.ts \
               translations/rosamp_de.ts \
#               translations/rosamp_sk.ts \
               translations/rosamp_it.ts \
               translations/rosamp_fr.ts \
               translations/rosamp_zh_CN.ts \
               translations/rosamp_ru_RU.ts \
               translations/rosamp_hu.ts \
#               translations/rosamp_en_US.ts \
               translations/rosamp_pl.ts \
               translations/rosamp_ja.ts \
               translations/rosamp_nl.ts \
               translations/rosamp_uk_UA.ts \
               translations/rosamp_pt_BR.ts \
#               translations/rosamp_ka.ts \
               translations/rosamp_cs.ts \
#               translations/rosamp_bg.ts \
               translations/rosamp_tr.ts \
#               translations/rosamp_sv.ts \
#               translations/rosamp_sr.ts \
               translations/rosamp_zh_TW.ts \
               translations/rosamp_ro_RO.ts \
               translations/rosamp_pt.ts \
#               translations/rosamp_el_GR.ts \
               translations/rosamp_fi.ts \
#               translations/rosamp_ko.ts \
               translations/rosamp_mk.ts \
#               translations/rosamp_eu.ts \
#               translations/rosamp_ca.ts \
               translations/rosamp_sl.ts \
               translations/rosamp_ar_SY.ts \
               translations/rosamp_ku.ts \
               translations/rosamp_gl.ts \
               translations/rosamp_vi_VN.ts \
               translations/rosamp_et.ts \
               translations/rosamp_lt.ts

contains( DEFINES, MIDI_SUPPORT ) {
    HEADERS += midiplayerthread.h \
               wildmidibackend.h

    SOURCES += midiplayerthread.cpp \
               wildmidibackend.cpp

    LIBS += -lWildMidi \
            -lasound
}

contains( DEFINES, DOWNLOAD_SUBS ) {
        INCLUDEPATH += findsubtitles/filedownloader findsubtitles/quazip
        DEPENDPATH += findsubtitles/filedownloader findsubtitles/quazip

        HEADERS += filedownloader.h subchooserdialog.h
        SOURCES += filedownloader.cpp subchooserdialog.cpp

        FORMS += subchooserdialog.ui

        HEADERS += crypt.h \
                   ioapi.h \
                   quazip.h \
                   quazipfile.h \
                   quazipfileinfo.h \
                   quazipnewinfo.h \
                   unzip.h \
                   zip.h

        SOURCES += ioapi.c \
                   quazip.cpp \
                   quazipfile.cpp \
                   quazipnewinfo.cpp \
                   unzip.c \
                   zip.c

        LIBS += -lz

        win32 {
                INCLUDEPATH += c:\development\zlib-1.2.3
                LIBS += -Lc:\development\zlib-1.2.3
        }
}

unix {
        UI_DIR = .ui
        MOC_DIR = .moc
        OBJECTS_DIR = .obj

        DEFINES += DATA_PATH=$(DATA_PATH)
        DEFINES += DOC_PATH=$(DOC_PATH)
        DEFINES += TRANSLATION_PATH=$(TRANSLATION_PATH)
        DEFINES += THEMES_PATH=$(THEMES_PATH)
        DEFINES += SHORTCUTS_PATH=$(SHORTCUTS_PATH)
        #DEFINES += NO_DEBUG_ON_CONSOLE

        INCLUDEPATH += /usr/include/qjson/
        LIBS += -lqjson

        #DEFINES += KDE_SUPPORT
        #INCLUDEPATH += /opt/kde3/include/
        #LIBS += -lkio -L/opt/kde3/lib/

        #contains( DEFINES, KDE_SUPPORT) {
        # HEADERS += mysystemtrayicon.h
        # SOURCES += mysystemtrayicon.cpp
        #}

        #HEADERS += prefassociations.h winfileassoc.h
        #SOURCES += prefassociations.cpp winfileassoc.cpp
        #FORMS += prefassociations.ui
}

win32 {
        DEFINES += SCREENSAVER_OFF
        contains( DEFINES, SCREENSAVER_OFF ) {
                HEADERS += screensaver.h
                SOURCES += screensaver.cpp
        }

        !contains( DEFINES, PORTABLE_APP ) {
                DEFINES += USE_ASSOCIATIONS
        }

        contains( DEFINES, USE_ASSOCIATIONS ) {
                HEADERS += prefassociations.h winfileassoc.h
                SOURCES += prefassociations.cpp winfileassoc.cpp
                FORMS += prefassociations.ui
        }

        contains(TEMPLATE,vcapp) {
                LIBS += ole32.lib user32.lib
        } else {
                LIBS += libole32
        }

#	RC_FILE = smplayer.rc
        DEFINES += NO_DEBUG_ON_CONSOLE
#	debug {
#		CONFIG += console
#	}
}
