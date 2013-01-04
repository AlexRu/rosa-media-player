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


#include "prefgeneral.h"
#include "preferences.h"
#include "filedialog.h"
#include "images.h"
#include "mediasettings.h"
#include "paths.h"

#if USE_ALSA_DEVICES || USE_DSOUND_DEVICES
#include "deviceinfo.h"
#endif

PrefGeneral::PrefGeneral(QWidget * parent, Qt::WindowFlags f)
    : PrefWidget(parent, f )
{
    setupUi(this);

    // Read driver info from InfoReader:
    InfoReader * i = InfoReader::obj();
    vo_list = i->voList();
    ao_list = i->aoList();

#if USE_DSOUND_DEVICES
    dsound_devices = DeviceInfo::dsoundDevices();
#endif

#if USE_ALSA_DEVICES
    alsa_devices = DeviceInfo::alsaDevices();
#endif
#if USE_XV_ADAPTORS
    xv_adaptors = DeviceInfo::xvAdaptors();
#endif

    // Screensaver
#ifdef Q_OS_WIN
    screensaver_check->hide();
#endif

#ifdef Q_OS_WIN
    vdpau_filters_check->hide();
#endif

    // Channels combo
    channels_combo->addItem( "2", MediaSettings::ChStereo );
    channels_combo->addItem( "4", MediaSettings::ChSurround );
    channels_combo->addItem( "6", MediaSettings::ChFull51 );

    retranslateStrings();
}

PrefGeneral::~PrefGeneral()
{
}

QString PrefGeneral::sectionName()
{
    return tr("General");
}

QPixmap PrefGeneral::sectionIcon()
{
    return Images::pixmap("logo");
}

void PrefGeneral::retranslateStrings()
{
    int mainwindow_resize = mainwindow_resize_combo->currentIndex();

    retranslateUi(this);

    mainwindow_resize_combo->setCurrentIndex(mainwindow_resize);

    channels_combo->setItemText(0, tr("2 (Stereo)") );
    channels_combo->setItemText(1, tr("4 (4.0 Surround)") );
    channels_combo->setItemText(2, tr("6 (5.1 Surround)") );

    // Icons
    /*
    resize_window_icon->setPixmap( Images::icon("resize_window") );
    volume_icon->setPixmap( Images::icon("speaker") );
    */

    createHelp();
}

void PrefGeneral::setData(Preferences * pref)
{
    setRememberSettings( !pref->dont_remember_media_settings );
    setCloseOnFinish( pref->close_on_finish );
    setPauseWhenHidden( pref->pause_when_hidden );

#ifdef Q_OS_WIN
    setAvoidScreensaver( pref->avoid_screensaver );
    setTurnScreensaverOff( pref->turn_screensaver_off );
#else
    setDisableScreensaver( pref->disable_screensaver );
#endif

    setAudioChannels( pref->initial_audio_channels );
    setAutoSyncActivated( pref->autosync );

    setAutoAddFilesToPlaylist( pref->auto_add_to_playlist );

    setResizeMethod( pref->resize_method );
    setSaveSize( pref->save_window_size_on_exit );

    setUseSingleInstance( pref->use_single_instance );

    setInitialVolNorm( pref->initial_volnorm );

    setYoutubeEnabled(pref->youtubeEnabled);
}

void PrefGeneral::getData(Preferences * pref)
{
    requires_restart = false;

    bool dont_remember_ms = !rememberSettings();
    TEST_AND_SET(pref->dont_remember_media_settings, dont_remember_ms);

    pref->close_on_finish = closeOnFinish();
    pref->pause_when_hidden = pauseWhenHidden();

#ifdef Q_OS_WIN
    pref->avoid_screensaver = avoidScreensaver();
    TEST_AND_SET(pref->turn_screensaver_off, turnScreensaverOff());
#else
    TEST_AND_SET(pref->disable_screensaver, disableScreensaver());
#endif

    pref->initial_audio_channels = audioChannels();

    TEST_AND_SET(pref->autosync, autoSyncActivated());

    pref->auto_add_to_playlist = autoAddFilesToPlaylist();

    pref->resize_method = resizeMethod();
    pref->save_window_size_on_exit = saveSize();

    pref->use_single_instance = useSingleInstance();

    pref->initial_volnorm = initialVolNorm();

    pref->youtubeEnabled = youtubeEnabled();
}

void PrefGeneral::setRememberSettings(bool b)
{
    remember_all_check->setChecked(b);
    //rememberAllButtonToggled(b);
}

bool PrefGeneral::rememberSettings()
{
    return remember_all_check->isChecked();
}

void PrefGeneral::setCloseOnFinish(bool b)
{
    close_on_finish_check->setChecked(b);
}

bool PrefGeneral::closeOnFinish()
{
    return close_on_finish_check->isChecked();
}

void PrefGeneral::setPauseWhenHidden(bool b)
{
    pause_if_hidden_check->setChecked(b);
}

bool PrefGeneral::pauseWhenHidden()
{
    return pause_if_hidden_check->isChecked();
}

void PrefGeneral::setAutoSyncActivated(bool b)
{
    autosync_check->setChecked(b);
}

bool PrefGeneral::autoSyncActivated()
{
    return autosync_check->isChecked();
}

void PrefGeneral::setAudioChannels(int ID)
{
    int pos = channels_combo->findData(ID);
    if (pos != -1)
    {
        channels_combo->setCurrentIndex(pos);
    }
    else
    {
        qWarning("PrefGeneral::setAudioChannels: ID: %d not found in combo", ID);
    }
}

int PrefGeneral::audioChannels()
{
    if (channels_combo->currentIndex() != -1)
    {
        return channels_combo->itemData( channels_combo->currentIndex() ).toInt();
    }
    else
    {
        qWarning("PrefGeneral::audioChannels: no item selected");
        return 0;
    }
}

void PrefGeneral::setInitialVolNorm(bool b)
{
    volnorm_check->setChecked(b);
}

bool PrefGeneral::initialVolNorm()
{
    return volnorm_check->isChecked();
}

void PrefGeneral::setUseSingleInstance(bool b)
{
    single_instance_check->setChecked(b);
    //singleInstanceButtonToggled(b);
}

bool PrefGeneral::useSingleInstance()
{
    return single_instance_check->isChecked();
}

void PrefGeneral::setYoutubeEnabled(bool b)
{
    youtube_enabled->setChecked(b);
}

bool PrefGeneral::youtubeEnabled() const
{
    return youtube_enabled->isChecked();
}

#ifdef Q_OS_WIN
void PrefGeneral::setAvoidScreensaver(bool b)
{
    avoid_screensaver_check->setChecked(b);
}

bool PrefGeneral::avoidScreensaver()
{
    return avoid_screensaver_check->isChecked();
}

void PrefGeneral::setTurnScreensaverOff(bool b)
{
    turn_screensaver_off_check->setChecked(b);
}

bool PrefGeneral::turnScreensaverOff()
{
    return turn_screensaver_off_check->isChecked();
}
#else
void PrefGeneral::setDisableScreensaver(bool b)
{
    screensaver_check->setChecked(b);
}

bool PrefGeneral::disableScreensaver()
{
    return screensaver_check->isChecked();
}
#endif

void PrefGeneral::setAutoAddFilesToPlaylist(bool b)
{
    auto_add_to_playlist_check->setChecked(b);
}

bool PrefGeneral::autoAddFilesToPlaylist()
{
    return auto_add_to_playlist_check->isChecked();
}

void PrefGeneral::setResizeMethod(int v)
{
    mainwindow_resize_combo->setCurrentIndex(v);
}

int PrefGeneral::resizeMethod()
{
    return mainwindow_resize_combo->currentIndex();
}

void PrefGeneral::setSaveSize(bool b)
{
    save_size_check->setChecked(b);
}

bool PrefGeneral::saveSize()
{
    return save_size_check->isChecked();
}

void PrefGeneral::createHelp()
{
    clearHelp();

    addSectionTitle(tr("Media settings"));

    setWhatsThis(remember_all_check, tr("Remember settings"),
                 tr("Usually ROSA Media Player will remember the settings for each file you "
                    "play (audio track selected, volume, filters...). Disable this "
                    "option if you don't like this feature.") );

    setWhatsThis(close_on_finish_check, tr("Close when finished"),
                 tr("If this option is checked, the main window will be automatically "
                    "closed when the current file/playlist finishes.") );

    setWhatsThis(pause_if_hidden_check, tr("Pause when minimized"),
                 tr("If this option is enabled, the file will be paused when the "
                    "main window is hidden. When the window is restored, playback "
                    "will be resumed.") );

    setWhatsThis(auto_add_to_playlist_check, tr("Automatically add files to playlist"),
                 tr("If this option is enabled, every time a file is opened, ROSA Media Player "
                    "will first clear the playlist and then add the file to it. In "
                    "case of DVDs, CDs and VCDs, all titles in the disc will be added "
                    "to the playlist.") );

#ifdef Q_OS_WIN
    setWhatsThis(turn_screensaver_off_check, tr("Switch screensaver off"),
                 tr("This option switches the screensaver off just before starting to "
                    "play a file and switches it on when playback finishes. If this "
                    "option is enabled, the screensaver won't appear even if playing "
                    "audio files or when a file is paused."));

    setWhatsThis(avoid_screensaver_check, tr("Avoid screensaver"),
                 tr("When this option is checked, ROSA Media Player will try to prevent the "
                    "screensaver to be shown when playing a video file. The screensaver "
                    "will be allowed to be shown if playing an audio file or in pause "
                    "mode. This option only works if the ROSA Media Player window is in "
                    "the foreground."));
#else
    setWhatsThis(screensaver_check, tr("Disable screensaver"),
                 tr("Check this option to disable the screensaver while playing.<br>"
                    "The screensaver will enabled again when play finishes.")
                 );
#endif

    setWhatsThis(autosync_check, tr("Audio/video auto synchronization"),
                 tr("Gradually adjusts the A/V sync based on audio delay "
                    "measurements.") );

    setWhatsThis(channels_combo, tr("Channels by default"),
                 tr("Requests the number of playback channels. MPlayer "
                    "asks the decoder to decode the audio into as many channels as "
                    "specified. Then it is up to the decoder to fulfill the "
                    "requirement. This is usually only important when playing "
                    "videos with AC3 audio (like DVDs). In that case liba52 does "
                    "the decoding by default and correctly downmixes the audio "
                    "into the requested number of channels. "
                    "<b>Note</b>: This option is honored by codecs (AC3 only), "
                    "filters (surround) and audio output drivers (OSS at least).") );

    addSectionTitle(tr("Main window"));

    setWhatsThis(mainwindow_resize_combo, tr("Autoresize"),
                 tr("The main window can be resized automatically. Select the option "
                    "you prefer.") );

    setWhatsThis(save_size_check, tr("Remember position and size"),
                 tr("If you check this option, the position and size of the main "
                    "window will be saved and restored when you run ROSA Media Player again.") );

    addSectionTitle(tr("Instances"));

    setWhatsThis(single_instance_check,
                 tr("Use only one running instance of ROSA Media Player"),
                 tr("Check this option if you want to use an already running instance "
                    "of ROSA Media Player when opening other files.") );

    addSectionTitle(tr("Audio"));

    setWhatsThis(volnorm_check, tr("Volume normalization by default"),
                 tr("Maximizes the volume without distorting the sound.") );

}

#include "moc_prefgeneral.cpp"
