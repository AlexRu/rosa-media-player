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

#ifndef CUTAUDIO_H
#define CUTAUDIO_H

#include <QWidget>
#include <QObject>

namespace Ui
{
class CutAudioPanel;
}

class FfmpegProcess;
class Core;

class CutAudio : public QWidget
{
    Q_OBJECT

public:
    explicit CutAudio( Core* core, QWidget *parent = 0 );
    ~CutAudio();

signals:

public slots:
    void startCut();
    void stopCut();
    void updateWidget();

protected slots:
    void processFinished();
    void updateProgressBar( int );
    void updateControls();
    void processError( int err );
    void playNewFile();
    void gotCurrentTime( double );

protected:
    bool checkDiskSpace( const QString& fname, double duration );

    void startFfmpeg();
    void tellffmpeg( const QString & command );

    void stopFfmpeg();

private:
    int m_bitrate;
    int m_error;
    bool m_isStopFfmpeg;
    bool m_isNewOpenFile;
    bool m_isCancel;

    QString m_inputFile;
    QString m_outputFile;

    FfmpegProcess* m_ffmpegProc;
    Core* m_core;

    Ui::CutAudioPanel* ui;

};

#endif // CUTAUDIO_H


