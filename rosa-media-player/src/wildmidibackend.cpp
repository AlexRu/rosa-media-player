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

#include "wildmidibackend.h"

#include <unistd.h>

#include <QtCore/QtDebug>

#include "midiplayerthread.h"


WildMidiBackend::WildMidiBackend( QObject *parent )
    : QObject( parent )
    , BackendBase()
    , m_volume( 50 )
    , m_thread( 0 )
{
    m_type = BackendFactory::WildMidi;

    initFormats();
}

WildMidiBackend::~WildMidiBackend()
{
    qDebug() << "WildMidiBackend::~WildMidiBackend()";
    stop();
}

void WildMidiBackend::initFormats()
{
    qDebug() << "WildMidiBackend::initFormats";
    m_formats << "mid" << "midi";
}

QString WildMidiBackend::path() const
{
    return m_path;
}

void WildMidiBackend::play()
{
    qDebug() << "WildMidiBackend::play()"; // ????????????????????????????????????????????????
    if ( m_thread && m_thread->isRunning() ) {
        m_thread->play();
    }
}

void WildMidiBackend::pause()
{
    qDebug() << "WildMidiBackend:   pause on/off...";
    if ( m_thread && m_thread->isRunning() ) {
        m_thread->pause();
    }
}

void WildMidiBackend::stop()
{
    qDebug() << "WildMidiBackend::stop():   shutting down Sound System...  m_thread = " << m_thread;

    if ( m_thread ) {
        if ( m_thread->isRunning() ) {
            // stop thread
            m_thread->stop();
            m_thread->wait();
            qDebug() << "WildMidiBackend::stop():   after wait() ...........";
        }

        closeConnections();

        delete m_thread;
        m_thread = 0;
    }
}

void WildMidiBackend::frameStep()
{
    qDebug() << "WildMidiBackend::frameStep():   ...";
}

void WildMidiBackend::writeToStdin( const QString &text )
{
    Q_UNUSED( text )
}

bool WildMidiBackend::isRunning()
{
    if ( m_thread ) {
        return m_thread->isRunning();
    }

    return false;
}

bool WildMidiBackend::isFinished()
{
    if ( m_thread ) {
        return m_thread->isFinished();
    }

    return true;
}

void WildMidiBackend::goToPos( double perc )
{
    qDebug() << "WildMidiBackend::goToPos:   perc = " << perc;

    if ( m_thread && m_thread->isRunning() ) {

        double sec = md.duration * perc / 100;

        qDebug() << "WildMidiBackend::goToPos:   sec = " << sec;
        goToSec( sec );
    }
}

void WildMidiBackend::goToSec( double sec )
{
    qDebug( "WildMidiBackend::goToSec" );

    if ( m_thread && m_thread->isRunning() ) {
        m_thread->goToSec( sec );
    }
}

void WildMidiBackend::seek( int secs )
{
    qDebug( "WildMidiBackend::seek" );

    if ( m_thread && m_thread->isRunning() ) {
        m_thread->seek( secs );
    }
}

void WildMidiBackend::setVolume( int volume )
{
    // volume may be from 0 to 127.... (in mplayer to 100)
    qDebug() << "WildMidiBackend::setVolume:   volume = " << volume;
    if ( m_thread && m_thread->isRunning() ) {
        m_thread->setVolume( volume );
        m_volume = volume;
    }
}

void WildMidiBackend::mute( bool b )
{
    qDebug() << "WildMidiBackend::mute";

    int volume = ( b ) ? 0 : m_volume;

    if ( m_thread && m_thread->isRunning() ) {
        m_thread->setVolume( volume );
    }
}

MediaData WildMidiBackend::getInfo( const QString &path )
{
    MediaData mdata;
    unsigned short int mixer_options = 0;

    qDebug() << "WildMidiBackend::getInfo:   path = " << path;

    mdata.reset();

    if ( WildMidi_Init( WILDMIDI_CFG, WILDMIDI_RATE, mixer_options ) >= 0 )  {
        void *midiPtr = WildMidi_Open ( qPrintable( path ) );
        if ( midiPtr ) {
            _WM_Info *wm_info = WildMidi_GetInfo( midiPtr );

            mdata.duration = ( qint64 )wm_info->approx_total_samples / WILDMIDI_RATE;
            mdata.audio_rate = WILDMIDI_RATE;
            mdata.novideo = true;
            mdata.type = TYPE_FILE;
            mdata.filename = path;
            mdata.list();

            WildMidi_Close( midiPtr );
        }
    }
    else {
        qDebug() << "WildMidiBackend::getInfo:   error when wildmidi init... ";
    }

    usleep(500);
    WildMidi_Shutdown();
    return mdata;
}

bool WildMidiBackend::start( const QString &path, int volume, double seek /* = -1 */ )
{
    qDebug() << "WildMidiBackend::start";
    m_path = path;

    stop();

    m_thread = new MidiPlayerThread( &md, this );

    qDebug() << "WildMidiBackend::start:   m_thread = " << m_thread;

    initConnections();

    return m_thread->start( m_path, volume, seek );
}

void WildMidiBackend::initConnections()
{
    if ( !m_thread ) {
        return;
    }

    connect( m_thread, SIGNAL(processExited()), SIGNAL(processExited()) );
    connect( m_thread, SIGNAL(receivedCurrentSec( double )), this, SIGNAL(receivedCurrentSec( double ) ) );
    connect( m_thread, SIGNAL(receivedPause()), SIGNAL(receivedPause()) );
    connect( m_thread, SIGNAL(receivedEndOfFile()), SIGNAL(receivedEndOfFile()) );
    connect( m_thread, SIGNAL(backendFullyLoaded()), SIGNAL(backendFullyLoaded()) );
    connect( m_thread, SIGNAL(receivedStartingTime( double )), SIGNAL(receivedStartingTime( double )) );
    connect( m_thread, SIGNAL(receivedCurrentFrame( int )), SIGNAL(receivedCurrentFrame( int )) );
    connect( m_thread, SIGNAL(receivedNoVideo()), SIGNAL(receivedNoVideo()) );
    connect( m_thread, SIGNAL(receivedNoVideo()), SIGNAL(receivedNoVideo()) );
    connect( m_thread, SIGNAL(audioInfoChanged(const Tracks &)), SIGNAL(audioInfoChanged(const Tracks &)) );
}

void WildMidiBackend::closeConnections()
{
    if ( !m_thread ) {
        return;
    }

    disconnect( m_thread, 0, 0, 0 );
}
