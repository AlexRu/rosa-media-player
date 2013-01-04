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


#ifndef _DEFAULTGUI_H_
#define _DEFAULTGUI_H_

#include "guiconfig.h"
#include "baseguiplus.h"
#include <QPoint>

class QToolBar;
class QPushButton;
class QResizeEvent;
class QSizeGrip;
class MyAction;
class QMenu;
class TimeSliderAction;
class VolumeSliderAction;
class FloatingWidget;
class TimeLabelAction;
class MyAction;
class MySlider;
class TimeSlider;

#if MINI_ARROW_BUTTONS
class SeekingButton;
#endif

class DefaultGui : public BaseGuiPlus
{
    Q_OBJECT

public:
    DefaultGui( QWidget* parent = 0, Qt::WindowFlags flags = 0 );
    ~DefaultGui();

#if USE_MINIMUMSIZE
    virtual QSize minimumSizeHint () const;
#endif

public slots:

protected:
    virtual void retranslateStrings();

    void createMainToolBars();
    void createControlWidgetMini();
    void createFloatingControl();
    void createActions();
    void createMenus();
    void createResizer();

    void loadConfig();
    void saveConfig();

    virtual void aboutToEnterFullscreen();
    virtual void aboutToExitFullscreen();

    virtual void resizeEvent( QResizeEvent * );
    /* virtual void closeEvent( QCloseEvent * ); */

protected slots:
    virtual void updateWidgets();
    virtual void displayTime(QString text);

    virtual void showFloatingControl(QPoint p);
    virtual void showFloatingMenu(QPoint p);
    virtual void hideFloatingControls();

    // Reimplemented:
#if AUTODISABLE_ACTIONS
    virtual void enableActionsOnPlaying();
    virtual void disableActionsOnStop();
#endif

protected:
    QLabel * m_timeLabel;
    QLabel * m_currentTimeLabel;

    QToolBar * controlwidget_mini;

    TimeSliderAction * timeslider_action;
    VolumeSliderAction * volumeslider_action;

    MySlider* m_vSlider;
    TimeSlider* m_tSlider;

#if MINI_ARROW_BUTTONS
    SeekingButton * rewindbutton_action;
    SeekingButton * forwardbutton_action;
#endif

    FloatingWidget * floating_control;
    TimeLabelAction * time_label_action;

    MyAction * viewFrameCounterAct;
    MyAction * viewVideoInfoAct;    
    int last_second;

    QSizeGrip *m_resizer;

};

#endif
