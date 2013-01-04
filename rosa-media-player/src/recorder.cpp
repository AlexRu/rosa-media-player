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

#include "recorder.h"

#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QtDebug>


Recorder::Recorder(QObject *parent) :
    QObject(parent), m_isRecording(false), m_timer(new QTimer(this))
{
    connect(&m_process, SIGNAL(error(QProcess::ProcessError)), SLOT(handleError(QProcess::ProcessError)));
}

void Recorder::startRecording(const RecordingOptions &options)
{
    const QString name = "ffmpeg";
    const QString resolution = QString::number(options.width) + "x" + QString::number(options.height);

    // need to add audio capture
    // ffmpeg -f oss -i /dev/dsp -f video4linux2 -i /dev/video0 /tmp/out.mpg
    // ffmpeg -f alsa -ac 1 -i hw:1 -f video4linux2 -i /dev/video0 /tmp/out.mpg

    QStringList arguments;
    arguments << "-f" << "x11grab" << "-r" << "25" << "-s" << resolution
              << "-i"  << ":0.0" << "-sameq" << "-map" << "0" << "-vcodec" << "libvpx" << options.path;
    qDebug() << arguments;

    bool shouldOverwrite = false;

    if(QFile::exists(options.path))
        shouldOverwrite = true;

    qDebug() << "Starting video recording";

    m_process.setProcessChannelMode(QProcess::MergedChannels);
    m_process.start(name, arguments);

    connect(m_timer, SIGNAL(timeout()), SLOT(timeout()));
    m_timer->start(500);

    if(shouldOverwrite)
    {
        m_process.waitForStarted();        
        m_process.write("y\n");
        m_process.waitForBytesWritten();
    }

    m_isRecording = true;
}

void Recorder::stopRecording()
{
    qDebug() << "Stopping video recording";
//    if(m_isRecording)
    {
        m_process.write("q");
        m_process.closeWriteChannel();
        m_process.waitForFinished();
        m_isRecording = false;
        m_timer->stop();
    }
}

bool Recorder::isRecording() const
{
    return m_isRecording;
}

void Recorder::handleError(QProcess::ProcessError err)
{
    QString errorMessage;
    switch(err)
    {
        case QProcess::FailedToStart:
            errorMessage = tr("Failed to start recording. Please check that ffmpeg is installed.");
            break;
        case QProcess::UnknownError:
            errorMessage = tr("Unknown error in recording occured");
            break;
        default:
            errorMessage = tr("Sorry, recording crashed");
            break;
    }

    m_isRecording = false;
    emit error(errorMessage);
}

void Recorder::timeout()
{
    if(m_isRecording)
    {
        QString s = m_process.readAll();
        parseOutput(s);
    }
}

void Recorder::parseOutput(const QString &output)
{
    // output example:
    // "frame=   89 fps=  6 q=0.0 size=    3239kB time=00:00:14.36 bitrate=1848.0kbits/s    "

    QRegExp rx("size=\\s+(.*)\\s+time=(.*)\\s+bitrate");
    rx.indexIn(output);

    if(rx.captureCount() != 2)
        return;
    else
    {
        QString size = rx.cap(1).trimmed();
        size.insert(size.length() - 2, " ");

        qDebug() << "Size <<" << size;
        qDebug() << "Time << " << rx.cap(2);

        if(!rx.cap(2).isEmpty() && !size.isEmpty())
            emit recordChanged(rx.cap(2), size);
    }
}
