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


#ifndef _MPLAYERPROCESS_H_
#define _MPLAYERPROCESS_H_

#include <QString>
#include "myprocess.h"
//#include "mediadata.h"
#include "config.h"

#include "backendbase.h"

#define NOTIFY_SUB_CHANGES 1
#define NOTIFY_AUDIO_CHANGES 1

class QStringList;

class MplayerProcess : public MyProcess, public BackendBase
{
    Q_OBJECT

public:
    MplayerProcess(QObject * parent = 0);
    ~MplayerProcess();

    bool start();
    virtual void writeToStdin( const QString & );

//    MediaData mediaData()
//    {
//        return md;
//    };

    virtual bool startBackend() { return true; }
    virtual bool stopBackend() { return true; }

    virtual bool isRunning();

    virtual void play();
    virtual void stop();
    virtual void pause();

    virtual void frameStep();

    virtual void goToPos( double perc );
    virtual void goToSec( double sec );
    virtual void seek( int secs );

    virtual void setVolume( int volume );
    virtual void mute( bool b );

    // Pass a command to mplayer by stdin:
    void tellcmd( const QString &command );

    QString pausing_prefix();
signals:
    void processExited();
    void lineAvailable( QString line );

    void receivedCurrentSec(double sec);
    void receivedCurrentFrame(int frame);
    void receivedPause();
    void receivedWindowResolution(int,int);
    void receivedNoVideo();
    void receivedVO(QString);
    void receivedAO(QString);
    void receivedEndOfFile();
    void backendFullyLoaded();
    void receivedStartingTime(double sec);

    void receivedCacheMessage(QString);
    void receivedCreatingIndex(QString);
    void receivedConnectingToMessage(QString);
    void receivedResolvingMessage(QString);
    void receivedScreenshot(QString);
    void receivedUpdatingFontCache();
    void receivedScanningFont(QString);

    void receivedStreamTitle(QString);
    void receivedStreamTitleAndUrl(QString,QString);

    void failedToParseMplayerVersion(QString line_with_mplayer_version);

#if NOTIFY_SUB_CHANGES
    //! Emitted if a new subtitle has been added or an old one changed
    void subtitleInfoChanged(const SubTracks &);

    //! Emitted when subtitle info has been received but there wasn't anything new
    void subtitleInfoReceivedAgain(const SubTracks &);
#endif
#if NOTIFY_AUDIO_CHANGES
    //! Emitted if a new audio track been added or an old one changed
    void audioInfoChanged(const Tracks &);
#endif

#if DVDNAV_SUPPORT
    void receivedDVDTitle(int);
    void receivedDuration(double);
    void receivedTitleIsMenu();
    void receivedTitleIsMovie();
#endif

protected:
    virtual void initFormats();

protected slots:
    void parseLine(QByteArray ba);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void gotError(QProcess::ProcessError);

private:
    bool notified_mplayer_is_running;
    bool received_end_of_file;

//    MediaData md;

    int last_sub_id;

    int mplayer_svn;

#if NOTIFY_SUB_CHANGES
    SubTracks subs;

    bool subtitle_info_received;
    bool subtitle_info_changed;
#endif

#if NOTIFY_AUDIO_CHANGES
    Tracks audios;
    bool audio_info_changed;
#endif

#if GENERIC_CHAPTER_SUPPORT
    int dvd_current_title;
#endif

    QStringList m_formats;
};


#endif
