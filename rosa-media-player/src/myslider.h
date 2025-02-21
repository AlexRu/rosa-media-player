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

#ifndef _MYSLIDER_H_
#define _MYSLIDER_H_

#include <QSlider>
#include "config.h"

#define CODE_FOR_CLICK 1 // 0 = old code, 1 = code copied from QSlider, 2 = button swap

class QTimer;

class MySlider : public QSlider
{
    Q_OBJECT

public:
    MySlider( QWidget * parent );
    ~MySlider();

protected:
    void mousePressEvent ( QMouseEvent * event );
#if CODE_FOR_CLICK == 1
    inline int pick(const QPoint &pt) const;
    int pixelPosToRangeValue(int pos) const;
#if QT_VERSION < 0x040300
    void initStyleOption(QStyleOptionSlider *option) const;
#endif
#endif
};

#endif

