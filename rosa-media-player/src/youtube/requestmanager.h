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

#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H


#include <QtCore/QUrl>
#include <QtCore/QMap>
#include <QtCore/QVariant>

class QNetworkAccessManager;
class Request;
class YTVideoItem;

const QString apiYouTubeUrl = QLatin1String("https://gdata.youtube.com/feeds/api/videos");

class RequestManager : public QObject
{
    Q_OBJECT
public:
    explicit RequestManager(QObject *parent = 0);
    Request *queryVideo(const QString &searchVideo, int index);

private slots:
    void videoReply(QByteArray reply);

signals:
    void newVideoItems(QList<YTVideoItem *> items);

private:
    void fillFromMap(YTVideoItem* videoItem, QVariantMap map);

};


#endif // REQUESTMANAGER_H
