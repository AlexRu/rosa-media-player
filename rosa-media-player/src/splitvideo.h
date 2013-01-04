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

#ifndef SPLITVIDEO_H
#define SPLITVIDEO_H

#include <QWidget>
#include <QObject>


namespace Ui
{
class SplitVideoPanel;
}

class SplitVideoProcess;
class Core;

class SplitVideo : public QWidget
{
    Q_OBJECT

public:
    explicit SplitVideo( Core* core, QWidget *parent = 0 );
    ~SplitVideo();

signals:

public slots:
    void startMencoder();
    void stopMencoder();

protected slots:
    // Pass a command to mencoder by stdin:
    void tellmc( const QString& command );
    void processFinished();
    void updateProgressBar( double );
    void setFromTime();
    void setToTime();
    void updateControls();
    void fromTimeChanged( const QTime & );
    void toTimeChanged( const QTime & );
    void playNewFile();
    void gotCurrentTime( double );
    void processError( int err );
    void processCutError();

protected:
//    void enableControlsOnSplitting();
//    void disableControlsOnStop();
    bool checkTime( int startTime, int endTime );
    void updateTime();
    bool checkDiskSpace( const QString& fname, int spliTime, double duration );

private:
    bool m_isNewOpenFile;
    int m_startTime;
    int m_endTime;
    int m_error;
    bool m_isStopMencoder;
    bool m_isCutError;

    QString m_inputFile;
    QString m_outputFile;

    SplitVideoProcess* m_proc;
    Core* m_core;

    Ui::SplitVideoPanel* ui;

};

#endif // SPLITVIDEO_H
