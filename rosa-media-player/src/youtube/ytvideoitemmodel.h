/* ROSA Media Player
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

#ifndef YTVIDEOITEMMODEL_H
#define YTVIDEOITEMMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QMetaType>

class YTVideoItem;

class YTVideoItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit YTVideoItemModel(QObject *parent = 0);
    ~YTVideoItemModel();
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual void appendRow(YTVideoItem *item);
    virtual void appendRows(const QList<YTVideoItem *> &items);
    virtual void prependRow(YTVideoItem *item);
    virtual void prependRows(const QList<YTVideoItem *> &items);
    virtual void insertRow(int row, YTVideoItem *item);
    virtual bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void refreshRow(int row);
    YTVideoItem *takeRow(int row);
    YTVideoItem *find(const QString &id) const;
    YTVideoItem *itemAt(int row) const;
    QModelIndex indexFromItem(const YTVideoItem *item) const;
    void clear();
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void newVideoItems(QList <YTVideoItem *> list);

private:
    QList<YTVideoItem *> m_items;
};

#endif // YTVIDEOITEMMODEL_H
