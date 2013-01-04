/*  ROSA Media Player
    Sergey Borovkov <sergey.borovkov@osinit.ru>.
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

#include "remoteimageloader.h"

#include <QtCore/QBuffer>
#include <QtNetwork/QNetworkReply>

RemoteImageLoader::RemoteImageLoader(QObject *parent) :
    QObject(parent), m_isLoading(false)
{
    connect(&m_accessManager, SIGNAL(finished(QNetworkReply*)),
            SLOT(imageDownloaded(QNetworkReply*)));
    connect(this, SIGNAL(imageReady(QUrl,QImage)), SLOT(downloadNext()));
}

void RemoteImageLoader::addImages(const QList<QUrl> &images)
{
    m_images += images;
}

void RemoteImageLoader::load()
{
    if(!m_isLoading) {
        downloadNext();
        m_isLoading = true;
    }
}

void RemoteImageLoader::imageDownloaded(QNetworkReply *reply)
{
    const QByteArray data = reply->readAll();

    QImage image;
    image.loadFromData(data);
    emit imageReady(m_currentUrl, image);
}

void RemoteImageLoader::downloadNext()
{
    if(m_images.isEmpty()) {
        m_isLoading = false;
        return;
    }

    m_currentUrl = m_images.takeFirst();
    QNetworkRequest request(m_currentUrl);
    m_accessManager.get(request);
}
