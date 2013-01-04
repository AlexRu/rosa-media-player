/*  ROSA Media Player
    Copyright (C) 2006-2010 Ricardo Villalba <rvm@escomposlinux.org>
    Julia Mineeva, Evgeniy Augin. Copyright (c) 2011 ROSA  <support@rosalab.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _BASEGUI_H_
#define _BASEGUI_H_

#include <QMainWindow>
#include <QNetworkProxy>
#include <QBoxLayout>
#include "mediadata.h"
#include "mediasettings.h"
#include "preferences.h"
#include "core.h"
#include "config.h"
#include "guiconfig.h"

#ifdef Q_OS_WIN
/* Disable screensaver by event */
#include <windows.h>
#endif

class QWidget;
class QStackedWidget;
class QMenu;
class QComboBox;
class QStackedWidget;
class LogWindow;
class MplayerWindow;

class QLabel;
class QSplitter;
class FilePropertiesDialog;
class VideoEqualizer;
class AudioEqualizer;
class FindSubtitlesWindow;
class VideoPreview;
class Playlist;
class SplitVideo;
class ScreenCapture;
class CutAudio;
class VideoSearch;

class MyAction;
class MyActionGroup;

class PreferencesDialog;
class MyServer;

class TVList;

class FfmpegProcess;


class BaseGui : public QMainWindow
{
    Q_OBJECT

public:
    BaseGui( QWidget* parent = 0, Qt::WindowFlags flags = 0 );
    ~BaseGui();

    /* Return true if the window shouldn't show on startup */
    virtual bool startHidden()
    {
        return false;
    };

    //! Execute all actions in \a actions. The actions should be
    //! separated by spaces. Checkable actions could have a parameter:
    //! true or false.
    void runActions(QString actions);

    //! Execute all the actions after the video has started to play
    void runActionsLater(QString actions)
    {
        pending_actions_to_run = actions;
    };

    //! Saves the line from the smplayer output
    void recordSmplayerLog(QString line);

public slots:
    virtual void open(QString file); // Generic open, autodetect type.
    virtual void openFile();
    virtual void openFile(QString file);
    virtual void openFiles(QStringList files);
    virtual void openURL();
    virtual void openURL(QString url);
    virtual void openVCD();
    virtual void openAudioCD();
    virtual void openDVD();
    virtual void openDVDFromFolder();
    virtual void openDVDFromFolder(QString directory);
    virtual void openDirectory();
    virtual void openDirectory(QString directory);

    virtual void helpContents();
    virtual void helpAbout();

    virtual void loadSub();
    virtual void loadAudioFile(); // Load external audio file

    void setInitialSubtitle(const QString & subtitle_file);

    virtual void showFindSubtitlesDialog();
    virtual void openUploadSubtitlesPage(); //turbos

    virtual void showVideoPreviewDialog();

    virtual void showRightPanel();
    virtual void showRightPanel( bool );
    virtual void showPlaylist();
    virtual void showPlaylist(bool b);
    virtual void showVideoEqualizer();
    virtual void showVideoEqualizer(bool b);
    virtual void showAudioEqualizer();
    virtual void showAudioEqualizer(bool b);
    virtual void showPreferencesDialog();
    virtual void showFilePropertiesDialog();

    virtual void showAudioDelayDialog();

    virtual void exitFullscreen();
    virtual void toggleFullscreen();
    virtual void toggleFullscreen(bool);

    void recordScreen();

    void setStayOnTop(bool b);
    virtual void checkStayOnTop(Core::State);

    void setForceCloseOnFinish(int n)
    {
        arg_close_on_finish = n;
    };
    int forceCloseOnFinish()
    {
        return arg_close_on_finish;
    };

    void setForceStartInFullscreen(int n)
    {
        arg_start_in_fullscreen = n;
    };
    int forceStartInFullscreen()
    {
        return arg_start_in_fullscreen;
    };

    virtual void changeContentsRightPanel( int );

protected slots:

    void updateCodecInfo();

    virtual void changePlayOrPauseActIcon(Core::State);

    virtual void closeWindow();

    virtual void setJumpTexts();

    // Replace for setCaption (in Qt 4 it's not virtual)
    virtual void setWindowCaption(const QString & title);

    //virtual void openRecent(int item);
    virtual void openRecent();
    virtual void enterFullscreenOnPlay();
    virtual void exitFullscreenOnStop();
    virtual void exitFullscreenIfNeeded();
    virtual void playlistHasFinished();

    virtual void displayState(Core::State state);
    virtual void displayMessage(QString message);
    virtual void gotCurrentTime(double);

    virtual void initializeMenus();
    virtual void updateWidgets();
    virtual void updateVideoEqualizer();
    virtual void updateAudioEqualizer();

    virtual void newMediaLoaded();
    virtual void updateMediaInfo();

    void checkPendingActionsToRun();

#if REPORT_OLD_MPLAYER
    void checkMplayerVersion();
    void displayWarningAboutOldMplayer();
#endif

#if AUTODISABLE_ACTIONS
    virtual void enableActionsOnPlaying();
    virtual void disableActionsOnStop();
    virtual void togglePlayAction(Core::State);
#endif

    virtual void resizeWindow(int w, int h);
    virtual void hidePanel();

    /* virtual void playlistVisibilityChanged(); */

    virtual void displayGotoTime(int);
    //! You can call this slot to jump to the specified percentage in the video, while dragging the slider.
    virtual void goToPosOnDragging(int);

    virtual void showPopupMenu();
    virtual void showPopupMenu( QPoint p );
    /*
    virtual void mouseReleaseEvent( QMouseEvent * e );
    virtual void mouseDoubleClickEvent( QMouseEvent * e );
    */

    virtual void leftClickFunction();
    virtual void rightClickFunction();
    virtual void doubleClickFunction();
    virtual void middleClickFunction();
    virtual void xbutton1ClickFunction();
    virtual void xbutton2ClickFunction();
    virtual void processFunction(QString function);

    virtual void dragEnterEvent( QDragEnterEvent * ) ;
    virtual void dropEvent ( QDropEvent * );

    virtual void applyNewPreferences();
    virtual void applyFileProperties();

    virtual void clearRecentsList();

    virtual void loadActions();
    virtual void saveActions();

    // Check the mouse pos in fullscreen mode, to
    // show the controlwidget if it's moved to
    // the bottom area.
    virtual void checkMousePos( QPoint );

    // Single instance stuff
    // Another instance request open a file
    virtual void remoteOpen(QString file);
    virtual void remoteOpenFiles(QStringList files);
    virtual void remoteAddFiles(QStringList files);
    virtual void remoteLoadSubtitle(QString file);

    //! Called when core can't parse the mplayer version and there's no
    //! version supplied by the user
    void askForMplayerVersion(QString);

    void showExitCodeFromMplayer(int exit_code);
    void showErrorFromMplayer(QProcess::ProcessError);

//        /void showSidePanelSlot();

    // stylesheet
#if ALLOW_CHANGE_STYLESHEET
    virtual void loadQss(QString filename);
    virtual void changeStyleSheet(QString style);
#endif

#ifdef Q_OS_WIN
    /* Disable screensaver by event */
    void clear_just_stopped();
#endif

    //! Clears the mplayer log
    void clearMplayerLog();

    //! Saves the line from the mplayer output
    void recordMplayerLog(QString line);

    //! Saves the mplayer log to a file every time a file is loaded
    void autosaveMplayerLog();

    void updateWidgetWidth();

signals:
    void frameChanged(int);
    void ABMarkersChanged(int secs_a, int secs_b);
    void videoInfoChanged(int width, int height, double fps);
    void timeChanged(QString time_ready_to_print);

    void cursorNearTop(QPoint);
    void cursorNearBottom(QPoint);
    void cursorFarEdges();

    void wheelUp();
    void wheelDown();
    /*
    void doubleClicked();
    void leftClicked();
    void middleClicked();
    */

    //! Sent when the user wants to close the main window
    void quitSolicited();

protected:
    enum { PLAYLIST_WIDGET = 0, /*EXPORT_WIDGET,*/ SPLIT_VIDEO_WIDGET, CUT_AUDIO_WIDGET, SCREEN_CAPTURE_WIDGET };

protected:
    virtual void retranslateStrings();
    virtual void changeEvent(QEvent * event);
    virtual void hideEvent( QHideEvent * );
    virtual void showEvent( QShowEvent * );
#ifdef Q_OS_WIN
    /* Disable screensaver by event */
    virtual bool winEvent ( MSG * m, long * result );
#endif

    virtual void aboutToEnterFullscreen();
    virtual void aboutToExitFullscreen();

protected:
    void createCore();
    void createMplayerWindow();
    void createVideoEqualizer();
    void createAudioEqualizer();
    void createPlaylist();
    void createPanel();
    void createMainLayout();

    void createRightPanel();
    void createExportWidget();
    void createSplitVideoWidget();
    void createScreenCapture();
    void createCutAudioWidget();

    void createPreferencesDialog();
    void createFilePropertiesDialog();
    void createControlWidget();
    void setDataToFileProperties();
    void initializeGui();
    void createActions();
#if AUTODISABLE_ACTIONS
    void setActionsEnabled(bool);
#endif
    void createMenus();
    void updateRecents();
    void configureDiscDevices();
    /* virtual void closeEvent( QCloseEvent * e ); */
    void addYoutube();

    void get_codecs_info();
    void tellffmpeg( const QString & command );

protected:
    virtual void wheelEvent( QWheelEvent * e ) ;

protected:
    QWidget* panel;
    QWidget* rightPanel;

    // Control Panel
    QToolBar * m_control;

    QSplitter* splitter;

    // Menu File
    MyAction * openFileAct;
    MyAction * openDirectoryAct;
    MyAction * openPlaylistAct;
    MyAction * openVCDAct;
    MyAction * openAudioCDAct;
    MyAction * openDVDAct;
    MyAction * openDVDFolderAct;
    MyAction * openURLAct;
    MyAction * exitAct;
    MyAction * clearRecentsAct;

    // Menu Play
    MyAction * screenRecordAct;
    MyAction * playAct;
    MyAction * playOrPauseAct;
    MyAction * pauseAct;
    MyAction * pauseAndStepAct;
    MyAction * stopAct;
    MyAction * frameStepAct;

    MyAction * rewind1Act;
    MyAction * rewind2Act;
    MyAction * rewind3Act;
    MyAction * forward1Act;
    MyAction * forward2Act;
    MyAction * forward3Act;

    // Menu Speed
    MyAction * normalSpeedAct;
    MyAction * halveSpeedAct;
    MyAction * doubleSpeedAct;
    MyAction * decSpeed10Act;
    MyAction * incSpeed10Act;
    MyAction * decSpeed4Act;
    MyAction * incSpeed4Act;
    MyAction * decSpeed1Act;
    MyAction * incSpeed1Act;

    // Menu Video
    MyAction * recordScreenAct;
    MyAction * fullscreenAct;
    MyAction * videoEqualizerAct;
    MyAction * screenshotAct;
    MyAction * videoPreviewAct;
    MyAction * flipAct;
    MyAction * postProcessingAct;
    MyAction * phaseAct;
    MyAction * deblockAct;
    MyAction * deringAct;
    MyAction * addNoiseAct;
    MyAction * addLetterboxAct;
    MyAction * upscaleAct;

    // Menu Audio
    MyAction * audioEqualizerAct;
    MyAction * muteAct;
    MyAction * decVolumeAct;
    MyAction * incVolumeAct;
    MyAction * decAudioDelayAct;
    MyAction * incAudioDelayAct;
    MyAction * audioDelayAct; // Ask for delay
    MyAction * extrastereoAct;
    MyAction * karaokeAct;
    MyAction * volnormAct;
    MyAction * loadAudioAct;
    MyAction * unloadAudioAct;

    // Menu Subtitles
    MyAction * loadSubsAct;
    MyAction * unloadSubsAct;
    MyAction * incSubScaleAct;
    MyAction * decSubScaleAct;
    MyAction * useAssAct;
    MyAction * useClosedCaptionAct;
    MyAction * useForcedSubsOnlyAct;
    MyAction * subVisibilityAct;
    MyAction * showFindSubtitlesDialogAct;
    MyAction * openUploadSubtitlesPageAct;//turbos

    // Menu Options
    MyAction * showPlaylistAct;
    MyAction * showPropertiesAct;
    MyAction * showPreferencesAct;

    // Menu Help
    MyAction * helpAct;
    MyAction * aboutThisAct;

    // Playlist
    MyAction * playPrevAct;
    MyAction * playNextAct;

    // Actions not in menus
#if !USE_MULTIPLE_SHORTCUTS
    MyAction * decVolume2Act;
    MyAction * incVolume2Act;
#endif
    MyAction * exitFullscreenAct;
    MyAction * nextOSDAct;
    MyAction * decContrastAct;
    MyAction * incContrastAct;
    MyAction * decBrightnessAct;
    MyAction * incBrightnessAct;
    MyAction * decHueAct;
    MyAction * incHueAct;
    MyAction * decSaturationAct;
    MyAction * incSaturationAct;
    MyAction * decGammaAct;
    MyAction * incGammaAct;
    MyAction * nextVideoAct;
    MyAction * nextAudioAct;
    MyAction * nextSubtitleAct;
    MyAction * nextChapterAct;
    MyAction * prevChapterAct;
    MyAction * doubleSizeAct;
    MyAction * resetVideoEqualizerAct;
    MyAction * resetAudioEqualizerAct;
    MyAction * showContextMenuAct;
    MyAction * nextAspectAct;
    MyAction * nextWheelFunctionAct;

    MyAction * showFilenameAct;
    MyAction * toggleDeinterlaceAct;

#if USE_MPLAYER_PANSCAN
    MyAction * incPanscanAct;
    MyAction * decPanscanAct;
#endif

    // Denoise Action Group
    MyActionGroup * denoiseGroup;
    MyAction * denoiseNoneAct;
    MyAction * denoiseNormalAct;
    MyAction * denoiseSoftAct;

    // Window Size Action Group
    MyActionGroup * sizeGroup;
    MyAction * size50;
    MyAction * size75;
    MyAction * size100;
    MyAction * size125;
    MyAction * size150;
    MyAction * size175;
    MyAction * size200;
    MyAction * size300;
    MyAction * size400;

    // Deinterlace Action Group
    MyActionGroup * deinterlaceGroup;
    MyAction * deinterlaceNoneAct;
    MyAction * deinterlaceL5Act;
    MyAction * deinterlaceYadif0Act;
    MyAction * deinterlaceYadif1Act;
    MyAction * deinterlaceLBAct;
    MyAction * deinterlaceKernAct;

    // Aspect Action Group
    MyActionGroup * aspectGroup;
    MyAction * aspectDetectAct;
    MyAction * aspectNoneAct;
    MyAction * aspect11Act;		// 1:1
    MyAction * aspect32Act;		// 3:2
    MyAction * aspect43Act;		// 4:3
    MyAction * aspect54Act;		// 5:4
    MyAction * aspect149Act;	// 14:9
    MyAction * aspect1410Act;	// 14:10
    MyAction * aspect169Act;	// 16:9
    MyAction * aspect1610Act;	// 16:10
    MyAction * aspect235Act;	// 2.35:1

    // Rotate Group
    MyActionGroup * rotateGroup;
    MyAction * rotateNoneAct;
    MyAction * rotateClockwiseFlipAct;
    MyAction * rotateClockwiseAct;
    MyAction * rotateCounterclockwiseAct;
    MyAction * rotateCounterclockwiseFlipAct;

#if USE_ADAPTER
    // Screen Group
    MyActionGroup * screenGroup;
    MyAction * screenDefaultAct;
#endif

    // Audio Channels Action Group
    MyActionGroup * channelsGroup;
    /* MyAction * channelsDefaultAct; */
    MyAction * channelsStereoAct;
    MyAction * channelsSurroundAct;
    MyAction * channelsFull51Act;

    // Other groups
#if PROGRAM_SWITCH
    MyActionGroup * programTrackGroup;
#endif
    MyActionGroup * videoTrackGroup;
    MyActionGroup * audioTrackGroup;
    MyActionGroup * subtitleTrackGroup;
    MyActionGroup * titleGroup;
    MyActionGroup * angleGroup;
    MyActionGroup * chapterGroup;

#if DVDNAV_SUPPORT
    MyAction * dvdnavUpAct;
    MyAction * dvdnavDownAct;
    MyAction * dvdnavLeftAct;
    MyAction * dvdnavRightAct;
    MyAction * dvdnavMenuAct;
    MyAction * dvdnavSelectAct;
    MyAction * dvdnavPrevAct;
    MyAction * dvdnavMouseAct;
#endif

    MyAction * showRightPanelAct;

    // MENUS
    QMenu *openMenu;
    QMenu *videoMenu;
    QMenu *audioMenu;
    QMenu *subtitlesMenu;
    QMenu *browseMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;

    QMenu * subtitlestrack_menu;
#if PROGRAM_SWITCH
    QMenu * programtrack_menu;
#endif
    QMenu * videotrack_menu;
    QMenu * audiotrack_menu;
    QMenu * titles_menu;
    QMenu * chapters_menu;
    QMenu * angles_menu;
    QMenu * deinterlace_menu;
    //QMenu * denoise_menu;
    QMenu * videosize_menu;
    QMenu * audiochannels_menu;

    QMenu * speed_menu;
    QMenu * videofilter_menu;
    QMenu * audiofilter_menu;
    QMenu * rotate_menu;
#if USE_ADAPTER
    QMenu * screen_menu;
#endif

    QMenu * popup;
    QMenu * recentfiles_menu;

    LogWindow * mplayer_log_window;
    LogWindow * smplayer_log_window;
    LogWindow * clhelp_window;

    PreferencesDialog *pref_dialog;
    FilePropertiesDialog *file_dialog;
    Playlist * playlist;

    ScreenCapture *screenCapture;
    QStackedWidget *rightPanelStack;
//    QWidget* export_widget;
    SplitVideo* splitWidget;
    CutAudio* cutAudioWidget;
    VideoSearch *searchVideoWidget;
    VideoEqualizer * video_equalizer;
    AudioEqualizer * audio_equalizer;
    FindSubtitlesWindow * find_subs_dialog;
    VideoPreview * video_preview;

    Core * core;
    MplayerWindow *mplayerwindow;

    MyServer * server;

    TVList * tvlist;
    TVList * radiolist;

    QStringList actions_list;

    QString pending_actions_to_run;

    // Force settings from command line
    int arg_close_on_finish; // -1 = not set, 1 = true, 0 = false
    int arg_start_in_fullscreen; // -1 = not set, 1 = true, 0 = false

private:
    QString default_style;

    bool near_top;
    bool near_bottom;

    // Variables to restore pos and size of the window
    // when exiting from fullscreen mode.
    QPoint win_pos;
    QSize win_size;
    bool was_maximized;

#ifdef Q_OS_WIN
    /* Disable screensaver by event */
    bool just_stopped;
#endif

    QString mplayer_log;
    QString smplayer_log;

    bool ignore_show_hide_events;

    FfmpegProcess* m_ffmpegProc;

protected:
    int m_curPanelWidget;
    int sideWidgetWidth;
    bool wasSidePanel;

    QComboBox* comboboxPanel;
    QStackedWidget* m_stackedWidget;

};

#endif

