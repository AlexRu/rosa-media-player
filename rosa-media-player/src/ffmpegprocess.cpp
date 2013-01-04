/*  ROSA Media Player
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

#include "ffmpegprocess.h"

#include <QRegExp>
#include <QStringList>
#include <QApplication>
#include <QTime>
#include <QDebug>

#include "global.h"
#include "preferences.h"

using namespace Global;

FfmpegProcess::FfmpegProcess( QObject* parent )
    : MyProcess( parent )
    , m_hasMetadata( false )
{
    connect( this, SIGNAL( lineAvailable( QByteArray ) ),
             this, SLOT( parseLine( QByteArray ) ) );

    connect( this, SIGNAL( finished( int, QProcess::ExitStatus ) ),
             this, SLOT( processFinished( int, QProcess::ExitStatus ) ) );

    connect( this, SIGNAL( error( QProcess::ProcessError ) ),
             this, SLOT( gotError( QProcess::ProcessError) ) );
}

FfmpegProcess::~FfmpegProcess()
{
}

bool FfmpegProcess::start()
{
    MyProcess::start();

    md.reset();

    return waitForStarted();
}

void FfmpegProcess::writeToStdin( const QString& text )
{
    if ( isRunning() )
    {
        //qDebug("FfmpegProcess::writeToStdin");
        write( text.toLocal8Bit() + "\n");
    }
    else
    {
        qWarning("FfmpegProcess::writeToStdin: process not running");
    }
}

MediaData FfmpegProcess::mediaData()
{
    return md;
}

static QRegExp rx_cur_sec("size=\\s+(.*)\\s+time=(.*)\\s+bitrate");
static QRegExp rx_cut_ok( "^video: *([0-9]+)kB\\s" );
static QRegExp rx_libmp3lame( "\\s*D?E?V?S?D?T?\\slibmp3lame\\s+" );
static QRegExp rx_metadata( "Metadata:" );
static QRegExp rx_track( "\\s*track\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_title( "\\s*title\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_artist( "\\s*artist\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_album( "\\s*album\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_composer( "\\s*composer\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_copyright( "\\s*copyright\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_genre( "\\s*genre\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_date( "\\s*date\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_disk( "\\s*disk\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_TOPE( "\\s*TOPE\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_encoded_by( "\\s*encoded_by\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_encoder( "\\s*encoder\\s*:\\s+(.*)", Qt::CaseInsensitive );
static QRegExp rx_comment( "\\s*comment\\s*:\\s+(.*)", Qt::CaseInsensitive );

/*
  output metadata info example:
 Metadata:
    track           : 09
    title           : "Title"
    album           : Album name
    composer        : Composer Name
    genre           : Classical
    major_brand     : qt
    minor_version   : 537199360
    compatible_brands: qt
    creation_time   : 2010-06-29 08:48:42
    comment         : www.apple.com
    comment-eng     : www.apple.com
    copyright       : ©2010 Apple, Inc.
    copyright-eng   : ©2010 Apple, Inc.
    ENCODER         : Lavf52.110.0"
 */

/*
output example:
size=    7876kB time=00:03:21.62 bitrate= 320.0kbits/s
video:0kB audio:7876kB global headers:0kB muxing overhead 0.002480%
*/
void FfmpegProcess::parseLine( QByteArray ba )
{
    QString line = QString::fromLocal8Bit( ba );

    // Parse other things
    //    qDebug("FfmpegProcess::parseLine: '%s'", line.toUtf8().data() );

    if ( rx_cur_sec.indexIn( line ) != -1 ) {
        QString str = rx_cur_sec.cap( 2 );
        QTime t;
        int sec = t.secsTo ( QTime::fromString( str, "hh:mm:ss.z" ) );
        qDebug("FfmpegProcess::parseLine:   sec: %s (%d)", str.toLocal8Bit().data(), sec );
        emit receivedCurSec( sec );
    }

    qDebug() << "FfmpegProcess:     line -" << line;

    if ( rx_metadata.indexIn( line ) != ( -1 ) ) {
        m_hasMetadata = true;
    }

    if ( m_hasMetadata ) {
        if ( rx_track.indexIn( line ) != ( -1 ) ) {
            md.clip_track = rx_track.cap( 1 );
            //        qDebug() << "FfmpegProcess:     track =" << md.clip_track;
        }
        if ( rx_title.indexIn( line ) != ( -1 ) ) {
            md.clip_name = rx_title.cap( 1 );
            //        qDebug() << "FfmpegProcess:     title =" << md.clip_name;
        }
        if ( rx_album.indexIn( line ) != ( -1 ) ) {
            md.clip_album = rx_album.cap( 1 );
            //        qDebug() << "FfmpegProcess:     album =" << rx_album.cap( 1 );
        }
        if ( rx_genre.indexIn( line ) != ( -1 ) ) {
            md.clip_genre = rx_genre.cap( 1 );
            //        qDebug() << "FfmpegProcess:     genre =" << md.clip_genre;
        }
        if ( rx_artist.indexIn( line ) != ( -1 ) ) {
            md.clip_artist = rx_artist.cap( 1 );
            //        qDebug() << "FfmpegProcess:     artist =" << md.clip_artist;
        }
        if ( rx_date.indexIn( line ) != ( -1 ) ) {
            md.clip_date = rx_date.cap( 1 );
            //        qDebug() << "FfmpegProcess:     date =" << md.clip_date;
        }
        if ( rx_copyright.indexIn( line ) != ( -1 ) ) {
            md.clip_copyright = rx_copyright.cap( 1 );
            //        qDebug() << "FfmpegProcess:     copyright =" << md.clip_copyright;
        }
        if ( rx_encoded_by.indexIn( line ) != ( -1 ) ) {
            md.clip_software = rx_encoded_by.cap( 1 );
            //        qDebug() << "FfmpegProcess:     encoded_by =" << md.clip_software;
        }
        if ( rx_encoder.indexIn( line ) != ( -1 ) ) {
            md.clip_software = rx_encoder.cap( 1 );
            //        qDebug() << "FfmpegProcess:     encoder =" << md.clip_software;
        }
        if ( rx_comment.indexIn( line ) != ( -1 ) ) {
            md.clip_comment = rx_comment.cap( 1 );
            //        qDebug() << "FfmpegProcess:     comment =" << md.clip_comment;
        }
    }

    if ( rx_cut_ok.indexIn( line ) != -1 ) {
        qDebug( "FfmpegProcess::parseLine:   receivedCutFinished" );
        emit receivedCutFinished();
    }
    else if ( rx_libmp3lame.indexIn( line ) != (-1) ) {
        qDebug( "FfmpegProcess::parseLine:   libmp3lameAvailable" );
        emit libmp3lameAvailable();
    }
    else
    {
        emit lineAvailable( line );
    }
}

// Called when the process is finished
void FfmpegProcess::processFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    qDebug("FfmpegProcess::processFinished: exitCode: %d, status: %d", exitCode, (int) exitStatus);
    // Send this signal before the endoffile one, otherwise
    // the playlist will start to play next file before all
    // objects are notified that the process has exited.
    emit processExited();
}

void FfmpegProcess::gotError(QProcess::ProcessError error)
{
    qDebug("FfmpegProcess::gotError: %d", (int) error);
    emit receivedError( error );
}

