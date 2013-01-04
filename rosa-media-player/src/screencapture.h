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

#ifndef SCREENCAPTURE
#define SCREENCAPTURE

#include <QObject>
#include <QtGui/QSystemTrayIcon>

class Recorder;

class ScreenCapture : public QObject
{
    Q_OBJECT
public:
    explicit ScreenCapture(QObject *parent = 0);

signals:

public slots:
    void startCapture();
    void stopCapture();

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void recordChanged(QString length, QString size);
private:
    QString getNewFileName(const QString& fname);

    QSystemTrayIcon *trayIcon;
    Recorder *recorder;
    QString message;
    QString path;
};

#endif // SCREENCAPTUREOBJECT_H
