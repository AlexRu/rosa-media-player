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

#ifndef RECORDINGTHREAD_H
#define RECORDINGTHREAD_H

#include <QtCore/QThread>
#include <QtCore/QObject>
#include <QtCore/QProcess>


class QTimer;

struct RecordingOptions
{
    RecordingOptions()
        : width( 0 )
        , height( 0 )       
    {
    }

    int width;
    int height;    
    QString path;
};

class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(QObject *parent = 0);
    bool isRecording() const;

public slots:
    void startRecording(const RecordingOptions &options);
    void stopRecording();    

private slots:
    void handleError(QProcess::ProcessError error);
    void timeout();

signals:
    void error(QString err);
    void recordChanged(QString length, QString size);

private:
    void parseOutput(const QString& output);

    QProcess m_process;
    bool m_isRecording;
    QTimer *m_timer;
};

#endif // RECORDINGTHREAD_H
