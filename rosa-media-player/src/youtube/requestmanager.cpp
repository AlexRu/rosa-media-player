/* ROSA Media Player
 * Copyright (c) 2012 ROSA  <support@rosalab.ru>
 * Authors:
 *  Julia Mineeva <julia.mineeva@osinit.ru>
 *  Sergey Borovkov <sergey.borovkov@osinit.ru>
 * License: GPLv3
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 3,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "request.h"
#include "requestmanager.h"
#include "ytvideoitem.h"

#include <qjson/parser.h>
#include <QtCore/QStringList>
#include <QtCore/QTime>
#include <QtCore/QDebug>

RequestManager::RequestManager(QObject *parent)
    : QObject(parent)
{
}

Request *RequestManager::queryVideo(const QString &searchVideo, int index)
{
    QUrl url = apiYouTubeUrl;
    url.addQueryItem(QLatin1String("q"), searchVideo);
    url.addQueryItem(QLatin1String("v"), QLatin1String("2"));
    url.addQueryItem(QLatin1String("alt"), QLatin1String("jsonc"));
    url.addQueryItem(QLatin1String("start-index"), QString("%1").arg(index + 1));
    url.addQueryItem(QLatin1String("max-results"), QLatin1String("10"));

    Request *request = new Request(this);
    connect(request, SIGNAL(replyReady(QByteArray)), SLOT(videoReply(QByteArray)));

    request->setUrl(url);
    return request;
}

void RequestManager::videoReply(QByteArray reply)
{
    QJson::Parser parser;
    QVariantMap result = parser.parse(reply).toMap();

    if (!result.contains(QLatin1String("data")))
        return;

    result = result.value(QLatin1String("data")).toMap();
    const int videoCount = result.value(QLatin1String("totalItems")).toInt();

    if (videoCount == 0)
        return;

    QVariantList list = result.value(QLatin1String("items")).toList();

    QList<YTVideoItem *> videoItems;
    foreach(QVariant item, list) {
        QVariantMap map = item.toMap();
        YTVideoItem *videoItem = new YTVideoItem;
        fillFromMap(videoItem, map);
        videoItems.append(videoItem);
    }

    emit newVideoItems(videoItems);
}

void RequestManager::fillFromMap(YTVideoItem *videoItem, QVariantMap map)
{
    if (map.contains("id")) {
        videoItem->setData(YTVideoItem::Id, map.value("id").toString());
    }
    if (map.contains("title")) {
        videoItem->setData(YTVideoItem::Title, map.value("title").toString());
    }
    if (map.contains("description")) {
        videoItem->setData(YTVideoItem::Description, map.value("description").toString());
    }
    if (map.contains("duration")) {
        videoItem->setData(YTVideoItem::Duration, map.value("duration").toInt());
    }

    if (map.contains("thumbnail")) {
        QVariantMap thumbnailMap = map.value("thumbnail").toMap();
        videoItem->setData(YTVideoItem::SmallThumbnailUrl, thumbnailMap.value("sqDefault").toString());
        videoItem->setData(YTVideoItem::BigThumbnailUrl, thumbnailMap.value("hqDefault").toString());
    }
    if (map.contains("player")) {
        QVariantMap videoMap = map.value("player").toMap();
        videoItem->setData(YTVideoItem::VideoUrl, videoMap.value("default").toString());
    }
}
