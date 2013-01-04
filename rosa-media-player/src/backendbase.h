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

#ifndef BACKENDBASE_H
#define BACKENDBASE_H


#include "mediadata.h"

#include "backendfactory.h"

class BackendBase
{
public:
    explicit BackendBase() {}

    virtual bool startBackend() = 0;
    virtual bool stopBackend() = 0;
    virtual void writeToStdin( const QString &text ) = 0;

    virtual bool isRunning() = 0;

    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;

    virtual void frameStep() = 0;

    virtual QStringList supportedFormats() { return m_formats; }
    virtual bool isFormatSupported( const QString & format ) {
        return ( m_formats.contains( format, Qt::CaseInsensitive ) );
    }

    virtual void goToPos( double perc ) = 0;
    virtual void goToSec( double sec ) = 0;
    virtual void seek( int secs ) = 0;

    virtual void setVolume( int volume ) = 0;
    virtual void mute( bool b ) = 0;

    virtual MediaData mediaData() {
        return md;
    }

    BackendFactory::BackendType type() {
        return m_type;
    }
    
//signals:
    void processExited();
    virtual void lineAvailable( QString line ) = 0;

    virtual void receivedCurrentSec( double sec ) = 0;
    virtual void receivedCurrentFrame( int frame ) = 0;
    virtual void receivedPause() = 0;
    virtual void receivedWindowResolution( int, int ) = 0;
    virtual void receivedNoVideo() = 0;
    virtual void receivedVO( QString ) = 0;
    virtual void receivedAO( QString ) = 0;
    virtual void receivedEndOfFile() = 0;
    virtual void backendFullyLoaded() = 0;
    virtual void receivedStartingTime( double sec ) = 0;

    virtual void receivedCacheMessage( QString ) = 0;
    virtual void receivedCreatingIndex( QString ) = 0;
    virtual void receivedConnectingToMessage( QString ) = 0;
    virtual void receivedResolvingMessage( QString ) = 0;
    virtual void receivedScreenshot( QString ) = 0;
    virtual void receivedUpdatingFontCache() = 0;
    virtual void receivedScanningFont( QString ) = 0;

    virtual void receivedStreamTitle( QString ) = 0;
    virtual void receivedStreamTitleAndUrl( QString , QString ) = 0;

protected:
    virtual void initFormats() = 0;

protected:
    MediaData md;
    QStringList m_formats;

    BackendFactory::BackendType m_type;
    
};

#endif // BACKENDBASE_H
