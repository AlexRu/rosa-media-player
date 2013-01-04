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
#include "splitvideoprocess.h"

#include <QRegExp>
#include <QStringList>
#include <QApplication>

#include "global.h"
#include "preferences.h"

using namespace Global;

SplitVideoProcess::SplitVideoProcess( QObject* parent )
    : MyProcess( parent )
{
    connect( this, SIGNAL( lineAvailable(QByteArray)),
             this, SLOT( parseLine(QByteArray) ) );

    connect( this, SIGNAL( finished( int, QProcess::ExitStatus ) ),
             this, SLOT( processFinished( int, QProcess::ExitStatus ) ) );

    connect( this, SIGNAL( error( QProcess::ProcessError ) ),
             this, SLOT( gotError( QProcess::ProcessError) ) );
}

SplitVideoProcess::~SplitVideoProcess()
{
}

bool SplitVideoProcess::start()
{
    MyProcess::start();
    return waitForStarted();
}

void SplitVideoProcess::writeToStdin( QString text )
{
    if ( isRunning() )
    {
        //qDebug("SplitVideoProcess::writeToStdin");
        write( text.toLocal8Bit() + "\n");
    }
    else
    {
        qWarning("SplitVideoProcess::writeToStdin: process not running");
    }
}

//static QRegExp rx_pos( "^Pos: *(\\d+)%" );
static QRegExp rx_pos( "^Pos: *([0-9,:.-]+)s" );
//static QRegExp rx_pos( "^Pos:.*([\\d+\.\\d+])\s" );
static QRegExp rx_cut_err( " is incompatible with '-oac copy', please try '-oac pcm' instead or use '-fafmttag' to override it." );
static QRegExp rx_video_mndt( "Video stream is mandatory!" );

void SplitVideoProcess::parseLine(QByteArray ba)
{
    QString line = QString::fromLocal8Bit( ba );

    // Parse other things
    qDebug("SplitVideoProcess::parseLine: '%s'", line.toUtf8().data() );

    if ( rx_pos.indexIn( line ) != -1 )
    {
        double pos = rx_pos.cap(1).toDouble();
//        qDebug("SplitVideoProcess::parseLine:   sec: %.2f", pos);
        emit receivedSplitSec( pos );
    }

    if ( rx_cut_err.indexIn( line ) != -1 )
    {
        emit receivedCutError();
    }
    else if ( rx_video_mndt.indexIn( line ) != -1 )
    {
        emit receivedVideoMandatory();
    }
    else
    {
        emit lineAvailable( line );
    }
}

// Called when the process is finished
void SplitVideoProcess::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug("SplitVideoProcess::processFinished: exitCode: %d, status: %d", exitCode, (int) exitStatus);
    // Send this signal before the endoffile one, otherwise
    // the playlist will start to play next file before all
    // objects are notified that the process has exited.
    emit processExited();
}

void SplitVideoProcess::gotError(QProcess::ProcessError error)
{
    qDebug("SplitVideoProcess::gotError: %d", (int) error);
    emit receivedError( error );
}

