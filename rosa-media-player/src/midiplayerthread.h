/*  ROSA Media Player
    Julia Mineeva, Evgeniy Augin, Sergey Borovkov. Copyright (c) 2011 ROSA  <support@rosalab.ru>

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

#ifndef MIDIPLAYERTHREAD_H
#define MIDIPLAYERTHREAD_H


#include <QtCore/QThread>

#include "tracks.h"

extern "C" {
#include <wildmidi_lib.h>
}

#define WILDMIDI_CFG "/etc/timidity/timidity.cfg"
#define WILDMIDI_RATE 44100


class MediaData;

class MidiPlayerThread : public QThread
{
    Q_OBJECT

public:
    explicit MidiPlayerThread( MediaData *md, QObject *parent = 0 );
    ~MidiPlayerThread();

    bool start( const QString &path, int volume, double seek = -1 );

    void play();
    void pause();
    void stop();

    virtual void frameStep();

    virtual bool isRunning();
    virtual bool isFinished();

    virtual void goToSec( double sec );
    virtual void seek( int secs );

    virtual void setVolume( int volume );

signals:
    void processExited();

    void receivedCurrentSec( double sec );
    void receivedPause();
    void receivedEndOfFile();
    void backendFullyLoaded();
    void receivedStartingTime( double sec );

    void receivedCurrentFrame( int frame );
    void receivedWindowResolution( int, int );
    void receivedNoVideo();

    void audioInfoChanged( const Tracks & );

public slots:
    void processFinished();

protected slots:

protected:
    virtual void run();

private:
    MediaData          *m_md;
    void               *m_midiPtr;

    qint64              m_totalTime;        // total time of midi file
    quint32             m_sample_rate;      // sample rate of midi
    QString             m_path;             // path of midi file
    QString             m_configFile;       // config file of WildMidi

    bool m_inPause;
    bool m_isLoop;
    bool m_received_end_of_file;
    bool m_isStop;
    bool m_isFinished;

};

#endif // MIDIPLAYERTHREAD_H
