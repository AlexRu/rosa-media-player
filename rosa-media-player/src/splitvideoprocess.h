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

#ifndef SPLITVIDEOPROCESS_H
#define SPLITVIDEOPROCESS_H

#include <QString>
#include "myprocess.h"

class QStringList;

class SplitVideoProcess : public MyProcess
{
    Q_OBJECT

public:
    SplitVideoProcess( QObject * parent = 0 );
    ~SplitVideoProcess();

    bool start();
    void writeToStdin( QString text );

signals:
    void processExited();
    void lineAvailable( QString line );
    void receivedSplitSec( double );
    void receivedError( int );
    void receivedCutError();
    void receivedVideoMandatory();

protected slots:
    void parseLine( QByteArray ba );
    void processFinished( int exitCode, QProcess::ExitStatus exitStatus );
    void gotError( QProcess::ProcessError );

private:

};


#endif // SPLITVIDEOPROCESS_H
