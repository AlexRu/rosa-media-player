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

#include "ytvideoitemmodel.h"

#include <QtGui/QIcon>

#include "requestmanager.h"
#include "ytvideoitem.h"

YTVideoItemModel::YTVideoItemModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int YTVideoItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items.size();
}

QVariant YTVideoItemModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_items.size())
        return QVariant();
    return m_items.at(index.row())->data(role);
}

bool YTVideoItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    YTVideoItem *item = m_items.at(index.row());
    bool result = item->setData(role, value);
    if (result) {
        emit dataChanged(index, index);
    }
    return result;
}

YTVideoItemModel::~YTVideoItemModel()
{
    clear();
}

void YTVideoItemModel::appendRow(YTVideoItem *item)
{
    appendRows(QList<YTVideoItem *>() << item);
}

void YTVideoItemModel::appendRows(const QList<YTVideoItem *> &items)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount() + items.size() - 1);
    foreach(YTVideoItem * item, items)
    m_items.append(item);
    endInsertRows();
}

void YTVideoItemModel::prependRow(YTVideoItem *item)
{
    prependRows(QList<YTVideoItem *>() << item);
}

void YTVideoItemModel::prependRows(const QList<YTVideoItem *> &items)
{
    beginInsertRows(QModelIndex(), 0, items.size() - 1);
    foreach(YTVideoItem * item, items)
        m_items.prepend(item);
    endInsertRows();
}

void YTVideoItemModel::insertRow(int row, YTVideoItem *item)
{
    beginInsertRows(QModelIndex(), row, row);
    m_items.insert(row, item);
    endInsertRows();
}

YTVideoItem *YTVideoItemModel::find(const QString &id) const
{
    foreach(YTVideoItem * item, m_items)
    if (item->data(YTVideoItem::Id) == id)
        return item;
    return 0;
}

YTVideoItem *YTVideoItemModel::itemAt(int row) const
{
    return m_items[row];
}

QModelIndex YTVideoItemModel::indexFromItem(const YTVideoItem *item) const
{
    Q_ASSERT(item);
    for (int row = 0; row < m_items.size(); ++row)
        if (m_items.at(row) == item)
            return index(row);

    return QModelIndex();
}

void YTVideoItemModel::clear()
{
    qDeleteAll(m_items);
    m_items.clear();
}

Qt::ItemFlags YTVideoItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool YTVideoItemModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row < 0 || row >= m_items.size())
        return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_items.takeAt(row);
    endRemoveRows();
    return true;
}

bool YTVideoItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row < 0 || (row + count) > m_items.size())
        return false;
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i)
        delete m_items.takeAt(row);
    endRemoveRows();
    return true;
}

void YTVideoItemModel::refreshRow(int row)
{
    QModelIndex ind = index(row);
    emit dataChanged(ind, ind);
}

YTVideoItem *YTVideoItemModel::takeRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    YTVideoItem *item = m_items.takeAt(row);
    endRemoveRows();
    return item;
}

void YTVideoItemModel::newVideoItems(QList <YTVideoItem *> list)
{
    for (int i = 0; i < list.size() ; i++) {
        YTVideoItem *newItem = list.at(i);
        appendRow(newItem);
    }
}
