/*  ROSA Media Player
    Copyright (C) 2006-2010 Ricardo Villalba <rvm@escomposlinux.org>
    Julia Mineeva, Evgeniy Augin. Copyright (c) 2011 ROSA  <support@rosalab.ru>

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

#ifndef _RECENTS_H_
#define _RECENTS_H_

#include <QStringList>

class Recents
{

public:
    Recents();
    virtual ~Recents();

    virtual void clear();

    virtual void addItem(QString s);
    virtual QString item(int n);

    virtual int count();

    virtual void setMaxItems(int n_items);
    virtual int maxItems()
    {
        return max_items;
    };

    virtual void fromStringList(QStringList list);
    virtual QStringList toStringList();

    virtual void list();

protected:
    int max_items;
    QStringList l;
};

#endif
