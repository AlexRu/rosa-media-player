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

#ifndef FFMEGPROCESS_H
#define FFMEGPROCESS_H

#include <QString>

#include "myprocess.h"
#include "mediadata.h"

class FfmpegProcess : public MyProcess
{
    Q_OBJECT

public:
    FfmpegProcess( QObject * parent = 0 );
    ~FfmpegProcess();

    bool start();
    void writeToStdin( const QString& text );

    virtual MediaData mediaData();

signals:
    void processExited();
    void lineAvailable( QString line );
    void receivedCurSec( int );
    void receivedError( int );
    void receivedCutFinished();
    void libmp3lameAvailable();

protected slots:
    void parseLine( QByteArray ba );
    void processFinished( int exitCode, QProcess::ExitStatus exitStatus );
    void gotError( QProcess::ProcessError );

private:
    MediaData md;

    bool m_hasMetadata;


};

#endif // FFMEGPROCESS_H
