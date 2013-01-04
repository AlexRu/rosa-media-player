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

#include "screencapture.h"
#include "recorder.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QtGui/QMessageBox>
#include <QDir>
#include <QDateTime>
#include <QtCore/QtDebug>
#include <QDesktopServices>

ScreenCapture::ScreenCapture(QObject *parent) :
    QObject(parent)
{
    trayIcon = new QSystemTrayIcon(QIcon(":/icons-png/stop-record.png"), this);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    recorder = new Recorder(this);
    connect(recorder, SIGNAL(recordChanged(QString,QString)), SLOT(recordChanged(QString,QString)));
}

void ScreenCapture::startCapture()
{
    trayIcon->show();

    const QRect &rect = QApplication::desktop()->screenGeometry();

    RecordingOptions options;

    path = QDesktopServices::storageLocation(QDesktopServices::MoviesLocation) + tr("/Screencast - ")
            + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + ".webm";
    path = getNewFileName(path);

    options.path = path;
    options.height = rect.height();
    options.width = rect.width();

    static_cast<QWidget *>(parent())->hide();
    recorder->startRecording(options);
    qDebug() << "recording started";
}

void ScreenCapture::stopCapture()
{
    trayIcon->hide();
    recorder->stopRecording();    
    static_cast<QWidget *>(parent())->show();
    qDebug() << "recording stopped";
    QMessageBox::information(0, tr("Information"), tr("Screen capture was successfully saved to %1").arg(path));
}

void ScreenCapture::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::Trigger:
            if(recorder->isRecording())
                stopCapture();
            break;
        default:
            break;
    }
}

void ScreenCapture::recordChanged(QString length, QString size)
{
    message = tr("Length: %1\nSize: %2\n").arg(length, size);
    trayIcon->setToolTip(message);
}

QString ScreenCapture::getNewFileName(const QString& fname)
{
    QRegExp rx( "\\s+\\(([0-9]+)\\)$" );
    QString fn = fname;

    if ( QFile::exists( fname ) )
    {
        QFileInfo fi( fname );
        fn = fi.baseName();
        if ( rx.indexIn( fn ) != -1 ) // file copy exist already, create new file name increasing index
        {
            int idx = rx.cap( 1 ).toInt();
            qDebug("ScreenCapture::getNewFileName:   index: %d", idx);

            fn = fn.replace( rx, QString( " (%1)" ).arg( idx + 1 ) );
            fn = fi.absolutePath() + "/" + fn + "." + fi.suffix();
        }
        else // it's 1-st file copy, create new file name with " (1)"
        {
            fn = fi.absolutePath() + "/" + fi.baseName() + " (1)." + fi.suffix();
        }

        qDebug("ScreenCapture::getNewFileName:   fn: %s", fn.toLocal8Bit().data());
        return getNewFileName( fn );
    }
    else // file with such name not exist, it's ok
    {
        return fn;
    }
}
