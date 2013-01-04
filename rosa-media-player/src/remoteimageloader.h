/*  ROSA Media Player
    Sergey Borovkov <sergey.borovkov@osinit.ru>
    Copyright (c) 2012 ROSA  <support@rosalab.ru>

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

#ifndef REMOTEIMAGELOADER_H
#define REMOTEIMAGELOADER_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtGui/QImage>
#include <QtNetwork/QNetworkAccessManager>

class QNetworkReply;

class RemoteImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit RemoteImageLoader(QObject *parent = 0);
    void addImages(const QList<QUrl> &images);
    void load();

signals:
    void imageReady(QUrl url, QImage image);

private slots:
    void imageDownloaded(QNetworkReply* reply);
    void downloadNext();

private:
    bool m_isLoading;
    QUrl m_currentUrl;
    QList<QUrl> m_images;
    QNetworkAccessManager m_accessManager;
};

#endif // REMOTEIMAGELOADER_H
