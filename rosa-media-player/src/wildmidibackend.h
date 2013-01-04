/*  ROSA Media Player
    Julia Mineeva, Evgeniy Augin, Sergey Borovkov. Copyright (c) 2011-2012 ROSA  <support@rosalab.ru>
    
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

#ifndef WILDMIDIBACKEND_H
#define WILDMIDIBACKEND_H

#include "backendbase.h"
#include "tracks.h"

#include <QtCore/QObject>

class MidiPlayerThread;

class WildMidiBackend : public QObject, public BackendBase
{
    Q_OBJECT

public:
    explicit WildMidiBackend( QObject *parent = 0 );
    ~WildMidiBackend();

    bool start( const QString &path, int volume, double seek = -1 );

    QString path() const;

    void play();
    void pause();
    void stop();

    virtual void frameStep();

    virtual bool startBackend() { return true; }
    virtual bool stopBackend() { return true; }
    virtual void writeToStdin( const QString &text );

    virtual bool isRunning();
    virtual bool isFinished();

    virtual void goToPos( double perc );
    virtual void goToSec( double sec );
    virtual void seek( int secs );

    virtual void setVolume( int volume );
    virtual void mute( bool b );

    MediaData getInfo( const QString &path );

signals:
    void processExited();
    void lineAvailable(QString line);

    void receivedCurrentSec( double sec );
    void receivedPause();
    void receivedEndOfFile();
    void backendFullyLoaded();
    void receivedStartingTime( double sec );

    void receivedCurrentFrame( int frame );
    void receivedWindowResolution( int, int );
    void receivedNoVideo();
    void receivedVO( QString );
    void receivedAO( QString );

    void receivedCacheMessage( QString );
    void receivedCreatingIndex( QString );
    void receivedConnectingToMessage( QString );
    void receivedResolvingMessage( QString );
    void receivedScreenshot( QString );
    void receivedUpdatingFontCache();
    void receivedScanningFont( QString );

    void receivedStreamTitle( QString );
    void receivedStreamTitleAndUrl( QString , QString );

    void audioInfoChanged( const Tracks & );

public slots:

protected slots:

protected:
    virtual void initFormats();
    void initConnections();
    void closeConnections();


private:
    int                 m_volume;           // last volume
    QString             m_path;             // path of midi file
    MidiPlayerThread    *m_thread;

};


#endif // WILDMIDIBACKEND_H
