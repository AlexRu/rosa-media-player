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

#include "defaultgui.h"
#include "helper.h"
#include "colorutils.h"
#include "core.h"
#include "global.h"
#include "widgetactions.h"
#include "playlist.h"
#include "mplayerwindow.h"
#include "myaction.h"
#include "images.h"
#include "floatingwidget.h"
#include "toolbareditor.h"
#include "desktopinfo.h"
#include "controlpanel.h"
#include "screencapture.h"

#if DOCK_PLAYLIST
#include "playlistdock.h"
#endif

#include <QMenu>
#include <QToolBar>
#include <QSettings>
#include <QLabel>
#include <QStatusBar>
#include <QPushButton>
#include <QToolButton>
#include <QMenuBar>
#include <QComboBox>
#include <QSplitter>
#include <QMessageBox>

#include <QApplication>
#include <QtGui/QSizeGrip>

using namespace Global;

DefaultGui::DefaultGui( QWidget * parent, Qt::WindowFlags flags )
    : BaseGuiPlus( parent, flags )
{
    connect( this, SIGNAL(timeChanged(QString)),
             this, SLOT(displayTime(QString)) );
    connect( this, SIGNAL(cursorNearBottom(QPoint)),
             this, SLOT(showFloatingControl(QPoint)) );
    connect( this, SIGNAL(cursorNearTop(QPoint)),
             this, SLOT(showFloatingMenu(QPoint)) );
    connect( this, SIGNAL(cursorFarEdges()),
             this, SLOT(hideFloatingControls()) );

    createActions();
    createMainToolBars();
    createControlWidgetMini();
    createFloatingControl();
    createMenus();
    createResizer();

    retranslateStrings();

    loadConfig();

    controlwidget_mini->hide();

    int left, right, top, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    bottom =+ 2;
    setContentsMargins(left, top, right, bottom);
}

DefaultGui::~DefaultGui()
{
    saveConfig();
}

/*
void DefaultGui::closeEvent( QCloseEvent * ) {
	qDebug("DefaultGui::closeEvent");

	//BaseGuiPlus::closeEvent(e);
	qDebug("w: %d h: %d", width(), height() );
}
*/

void DefaultGui::createActions()
{
    qDebug("DefaultGui::createActions");

    timeslider_action = createTimeSliderAction(this);
    timeslider_action->disable();

    volumeslider_action = createVolumeSliderAction(this);
    volumeslider_action->disable();

    // Create the time label
    time_label_action = new TimeLabelAction(this);
    time_label_action->setObjectName("timelabel_action");

#if MINI_ARROW_BUTTONS
    QList<QAction*> rewind_actions;
    rewindbutton_action = new SeekingButton(rewind_actions, this);
    rewindbutton_action->setObjectName("rewindbutton_action");

    QList<QAction*> forward_actions;
    forwardbutton_action = new SeekingButton(forward_actions, this);
    forwardbutton_action->setObjectName("forwardbutton_action");
#endif

    // Statusbar
    viewVideoInfoAct = new MyAction(this, "toggle_video_info" );
    viewVideoInfoAct->setIconVisibleInMenu( false );
    viewVideoInfoAct->setCheckable(true);
//    connect( viewVideoInfoAct, SIGNAL(toggled(bool)),
//             video_info_display, SLOT(setVisible(bool)) );

    viewFrameCounterAct = new MyAction( this, "toggle_frame_counter" );
    viewFrameCounterAct->setCheckable( true );
    viewFrameCounterAct->setIconVisibleInMenu( false );
}

#if AUTODISABLE_ACTIONS
void DefaultGui::enableActionsOnPlaying()
{
    qDebug("DefaultGui::enableActionsOnPlaying");
    BaseGuiPlus::enableActionsOnPlaying();

    timeslider_action->enable();
    volumeslider_action->enable();
    m_vSlider->setEnabled(true);
    m_tSlider->setEnabled(true);
}

void DefaultGui::disableActionsOnStop()
{
    qDebug("DefaultGui::disableActionsOnStop");
    BaseGuiPlus::disableActionsOnStop();

    timeslider_action->disable();
    volumeslider_action->disable();
    m_vSlider->setEnabled(false);
    m_tSlider->setEnabled(false);
}
#endif // AUTODISABLE_ACTIONS

void DefaultGui::createMenus()
{
}

void DefaultGui::createMainToolBars()
{
}


void DefaultGui::createControlWidgetMini()
{

    qDebug("DefaultGui::createControlWidgetMini");

    controlwidget_mini = new QToolBar( this );
    controlwidget_mini->setObjectName("controlwidget_mini");
    controlwidget_mini->setMovable(false);
    addToolBar(Qt::BottomToolBarArea, controlwidget_mini);

    ControlPanel* panel = new ControlPanel(this);
    m_control->addWidget(panel);

    panel->playButton()->setDefaultAction(playOrPauseAct);
    panel->prevButton()->setDefaultAction(playPrevAct);
    panel->nextButton()->setDefaultAction(playNextAct);
    panel->muteButton()->setDefaultAction(muteAct);
    panel->fullScreenButton()->setDefaultAction(fullscreenAct);
    panel->sidePanelButton()->setDefaultAction(showRightPanelAct);

    m_tSlider = panel->timeSlider();

    connect( m_tSlider, SIGNAL( posChanged(int) ),
             core, SLOT(goToPosition(int)) );

    connect( core, SIGNAL(positionChanged(int)),
             m_tSlider, SLOT(setPos(int)) );

    connect( m_tSlider, SIGNAL( draggingPos(int) ),
             this, SLOT(displayGotoTime(int)) );

    m_tSlider->setDragDelay( pref->time_slider_drag_delay );

    connect( m_tSlider, SIGNAL( delayedDraggingPos(int) ),
             this, SLOT(goToPosOnDragging(int)) );

    m_tSlider->setEnabled(false);

    m_vSlider = panel->volumeSlider();

    connect( m_vSlider, SIGNAL( valueChanged(int) ),
             core, SLOT( setVolume(int) ) );

    connect( core, SIGNAL(volumeChanged(int)),
             m_vSlider, SLOT(setValue(int)) );

    m_vSlider->setEnabled(false);

    m_timeLabel = panel->timeLabel();
    m_currentTimeLabel = panel->currentTimeLabel();

}


void DefaultGui::createFloatingControl()
{
    // Floating control
    floating_control = new FloatingWidget(this);

#if !USE_CONFIGURABLE_TOOLBARS
    floating_control->toolbar()->addAction(playOrPauseAct);

#if MINI_ARROW_BUTTONS
#else
    floating_control->toolbar()->addAction(rewind3Act);
    floating_control->toolbar()->addAction(rewind2Act);
    floating_control->toolbar()->addAction(rewind1Act);
#endif

    floating_control->toolbar()->addAction(timeslider_action);

#if MINI_ARROW_BUTTONS
#else
    floating_control->toolbar()->addAction(forward1Act);
    floating_control->toolbar()->addAction(forward2Act);
    floating_control->toolbar()->addAction(forward3Act);
#endif

    floating_control->toolbar()->addAction(fullscreenAct);
    floating_control->toolbar()->addAction(muteAct);
    floating_control->toolbar()->addAction(volumeslider_action);
    floating_control->toolbar()->addSeparator();
    floating_control->toolbar()->addAction(time_label_action);


#endif // USE_CONFIGURABLE_TOOLBARS

#ifdef Q_OS_WIN
    // To make work the ESC key (exit fullscreen) and Ctrl-X (close) in Windows
    floating_control->addAction(exitFullscreenAct);
    floating_control->addAction(exitAct);
#endif

#if !USE_CONFIGURABLE_TOOLBARS
    floating_control->adjustSize();
#endif
}

void DefaultGui::createResizer()
{
    m_resizer = new QSizeGrip(this);
    // adjustSize() processes all events, which is suboptimal
    m_resizer->resize(m_resizer->sizeHint());
    if (QApplication::isRightToLeft())
        m_resizer->move(rect().bottomLeft() - m_resizer->rect().bottomLeft());
    else
        m_resizer->move(rect().bottomRight() - m_resizer->rect().bottomRight());
    m_resizer->raise();
    m_resizer->setVisible(true);
}

void DefaultGui::retranslateStrings()
{
    BaseGuiPlus::retranslateStrings();

    viewVideoInfoAct->change(Images::icon("view_video_info"), tr("&Video info") );
    viewFrameCounterAct->change( Images::icon("frame_counter"), tr("&Frame counter") );
}


void DefaultGui::displayTime(QString text)
{
    time_label_action->setText(text);
    QStringList list = text.split("/");
    m_currentTimeLabel->setText(list.at(0));
    m_timeLabel->setText(list.at(1));
}

void DefaultGui::updateWidgets()
{
    qDebug("DefaultGui::updateWidgets");

    BaseGuiPlus::updateWidgets();

    panel->setFocus();
}

void DefaultGui::aboutToEnterFullscreen()
{
    qDebug("DefaultGui::aboutToEnterFullscreen");

    BaseGuiPlus::aboutToEnterFullscreen();

    m_control->hide();
}

void DefaultGui::aboutToExitFullscreen()
{
    qDebug("DefaultGui::aboutToExitFullscreen");

    BaseGuiPlus::aboutToExitFullscreen();

    floating_control->hide();
    m_control->show();
}

void DefaultGui::showFloatingControl(QPoint /*p*/)
{
    qDebug("DefaultGui::showFloatingControl");

#if CONTROLWIDGET_OVER_VIDEO
    floating_control->setAnimated( pref->floating_control_animated );
    floating_control->setMargin(pref->floating_control_margin);
#ifndef Q_OS_WIN
    floating_control->setBypassWindowManager(pref->bypass_window_manager);
#endif
    floating_control->showOver(panel, pref->floating_control_width);
#else
    if (!controlwidget->isVisible())
    {
        controlwidget->show();
    }
#endif
}

void DefaultGui::showFloatingMenu(QPoint /*p*/)
{
#if !CONTROLWIDGET_OVER_VIDEO
    qDebug("DefaultGui::showFloatingMenu");

    if (!menuBar()->isVisible())
        menuBar()->show();
#endif
}

void DefaultGui::hideFloatingControls()
{
    qDebug("DefaultGui::hideFloatingControls");

#if CONTROLWIDGET_OVER_VIDEO
    floating_control->hide();
#else
    if (controlwidget->isVisible())
        controlwidget->hide();

    if (menuBar()->isVisible())
        menuBar()->hide();
#endif
}

void DefaultGui::resizeEvent( QResizeEvent * )
{
    /*
    qDebug("defaultGui::resizeEvent");
    qDebug(" controlwidget width: %d", controlwidget->width() );
    qDebug(" controlwidget_mini width: %d", controlwidget_mini->width() );
    */
    if ( m_resizer ) {
        if (QApplication::isRightToLeft())
            m_resizer->move(rect().bottomLeft() - m_resizer->rect().bottomLeft());
        else
            m_resizer->move(rect().bottomRight() - m_resizer->rect().bottomRight());
    }
}

#if USE_MINIMUMSIZE
QSize DefaultGui::minimumSizeHint() const
{
    return QSize(controlwidget_mini->sizeHint().width(), 0);
}

#endif


void DefaultGui::saveConfig()
{
    qDebug("DefaultGui::saveConfig");

    QSettings * set = settings;

    set->beginGroup( "default_gui");

    set->setValue("video_info", viewVideoInfoAct->isChecked());
    set->setValue("frame_counter", viewFrameCounterAct->isChecked());

    if (pref->save_window_size_on_exit)
    {
        qDebug("DefaultGui::saveConfig: w: %d h: %d", width(), height());
        set->setValue( "pos", pos() );
        set->setValue( "size", size() );
        set->setValue( "was_side_panel", wasSidePanel );
        set->setValue( "current_panel", m_curPanelWidget );
        set->setValue( "panel_width", sideWidgetWidth );

//        qDebug() << "-->> saveConfig:   sideWidgetWidth = " << sideWidgetWidth;

    }

    set->setValue( "toolbars_state", saveState(Helper::qtVersion()) );

#if USE_CONFIGURABLE_TOOLBARS
    set->beginGroup( "actions" );
    set->setValue("controlwidget_mini", ToolbarEditor::save(controlwidget_mini) );
    set->setValue("floating_control", ToolbarEditor::save(floating_control->toolbar()) );
    set->endGroup();
#endif

    set->endGroup();
}

void DefaultGui::loadConfig()
{
    qDebug("DefaultGui::loadConfig");

    QSettings * set = settings;

    set->beginGroup( "default_gui");

    viewVideoInfoAct->setChecked(set->value("video_info", false).toBool());
    viewFrameCounterAct->setChecked(set->value("frame_counter", false).toBool());

    if (pref->save_window_size_on_exit)
    {
        QPoint p = set->value("pos", pos()).toPoint();
        QSize s = set->value("size", size()).toSize();

        if ( (s.height() < 200) && (!pref->use_mplayer_window) )
        {
            s = pref->default_size;
        }

        move(p);
        resize(s);

        if (!DesktopInfo::isInsideScreen(this))
        {
            move(0,0);
            qWarning("DefaultGui::loadConfig: window is outside of the screen, moved to 0x0");
        }

//        qDebug() << "<<-- loadConfig:   size.width() " << s.width();

        wasSidePanel = set->value( "was_side_panel" ).toBool();
        m_curPanelWidget = set->value( "current_panel" ).toInt();
        sideWidgetWidth = set->value( "panel_width" ).toInt();

        if ( m_curPanelWidget >= comboboxPanel->count() )
            comboboxPanel->setCurrentIndex( 0 );
        else
            comboboxPanel->setCurrentIndex( m_curPanelWidget );

        QList<int> list;
        list << (s.width() - sideWidgetWidth) << sideWidgetWidth;
        splitter->setSizes( list );

        showRightPanel( wasSidePanel );
        showRightPanelAct->setChecked(wasSidePanel);
    }

#if USE_CONFIGURABLE_TOOLBARS
    QList<QAction *> actions_list = findChildren<QAction *>();

    QStringList controlwidget_mini_actions;
    controlwidget_mini_actions << "play_or_pause" << "timeslider_action" << "fullscreen" << "mute" << "volumeslider_action";

    QStringList floatingcontrol_actions;
    floatingcontrol_actions << "play_or_pause" <<  "timeslider_action";
    floatingcontrol_actions << "fullscreen" << "mute" << "volumeslider_action" << "separator" << "timelabel_action";

    set->beginGroup( "actions" );
    ToolbarEditor::load(controlwidget_mini, set->value("controlwidget_mini", controlwidget_mini_actions).toStringList(), actions_list );
    ToolbarEditor::load(floating_control->toolbar(), set->value("floating_control", floatingcontrol_actions).toStringList(), actions_list );
    floating_control->adjustSize();
    set->endGroup();
#endif

    restoreState( set->value( "toolbars_state" ).toByteArray(), Helper::qtVersion() );

#if DOCK_PLAYLIST
    qDebug("DefaultGui::loadConfig: playlist visible: %d", playlistdock->isVisible());
    qDebug("DefaultGui::loadConfig: playlist position: %d, %d", playlistdock->pos().x(), playlistdock->pos().y());
    qDebug("DefaultGui::loadConfig: playlist size: %d x %d", playlistdock->size().width(), playlistdock->size().height());
#endif

    set->endGroup();

    updateWidgets();
}


#include "moc_defaultgui.cpp"
