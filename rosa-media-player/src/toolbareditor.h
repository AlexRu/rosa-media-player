/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2010 Ricardo Villalba <rvm@escomposlinux.org>

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

#ifndef _TOOLBAR_EDITOR_H_
#define _TOOLBAR_EDITOR_H_

#include <QStringList>
#include <QWidget>
#include <QList>
#include <QAction>

class ToolbarEditor
{
public:

    //! Save the widget's list of actions into a QStringList
    static QStringList save(QWidget *w);

    //! Added to the widget the actions specified in l. actions_list is
    //! the list of all available actions
    static void load(QWidget *w, QStringList l, QList<QAction *> actions_list);

protected:
    static QAction * findAction(QString s, QList<QAction *> actions_list);
};

#endif

