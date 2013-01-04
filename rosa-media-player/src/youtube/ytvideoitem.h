/* ROSA Media player
 * Copyright (c) 2012 ROSA  <support@rosalab.ru>
 * Authors:
 *  Sergey Borovkov <sergey.borovkov@osinit.ru>
 *  Evgeniy Auzhin <evgeniy.augin@osinit.ru>
 *  Julia Mineeva <julia.mineeva@osinit.ru>
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

#ifndef YTVIDEOITEM_H
#define YTVIDEOITEM_H

#include <QtCore/QHash>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>
#include <QtCore/QMap>

class YTVideoItem
{
public:
    enum Role {
        Id = Qt::UserRole + 1,
        Title,
        Description,
        SmallThumbnailUrl,
        SmallThumbnail,
        BigThumbnailUrl,
        VideoUrl,
        Duration
    };

    QVariant data(int role) const;
    bool setData(int role, const QVariant & value);

private:
    QMap<int, QVariant> m_data;
};

#endif // YTVIDEOITEM_H
