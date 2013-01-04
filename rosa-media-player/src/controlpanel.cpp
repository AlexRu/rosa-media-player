/*  ROSA Media Player
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

#include "images.h"

#include "controlpanel.h"
#include "ui_controlpanel.h"

#include <QResizeEvent>
#include <QDebug>
#include <QGridLayout>
#include <QMargins>

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel)
{
    ui->setupUi(this);
    initVolumeSlider();
    ui->playOrPauseButton->setAutoRaise(true);
    ui->muteButton->setAutoRaise(true);
    ui->prevActButton->setAutoRaise(true);
    ui->nextActButton->setAutoRaise(true);
    ui->fullScreenButton->setAutoRaise(true);
    ui->sidePanelButton->setAutoRaise(true);
    ui->menuButton->setAutoRaise(true);
    ui->menuButton->hide();

    ui->muteButton->setIconSize(QSize(28,24));
    ui->sidePanelButton->setIconSize(QSize(28,24));
    ui->prevActButton->setIconSize(QSize(28,24));
    ui->nextActButton->setIconSize(QSize(28,24));
    ui->fullScreenButton->setIconSize(QSize(28,24));
    ui->sidePanelButton->setIconSize(QSize(28,24));
}

ControlPanel::~ControlPanel()
{
    delete ui;
}

void ControlPanel::resizeEvent(QResizeEvent * e)
{
    QGridLayout* l = dynamic_cast<QGridLayout*>(layout());
    if (!l)
        return;
    QMargins m = l->contentsMargins();
    QSize s = e->size();
    int w = s.width()/2 - (l->horizontalSpacing() )*4 - ui->muteButton->width() - ui->prevActButton->width()
            - ui->volumeSlider->width() - ui->playOrPauseButton->width()/2 - m.left() + 2;
    l->invalidate();
    ui->horizontalSpacer->changeSize(w,20, QSizePolicy::Fixed, QSizePolicy::Fixed);
    e->accept();
}

void ControlPanel::initVolumeSlider()
{
    ui->volumeSlider->setMinimum(0);
    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setValue(50);
    ui->volumeSlider->setOrientation( Qt::Horizontal );
    ui->volumeSlider->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    ui->volumeSlider->setFocusPolicy( Qt::NoFocus );
    ui->volumeSlider->setTickPosition( QSlider::TicksBelow );
    ui->volumeSlider->setTickInterval( 10 );
    ui->volumeSlider->setSingleStep( 1 );
    ui->volumeSlider->setPageStep( 10 );
    ui->volumeSlider->setToolTip( tr("Volume") );
    //ui->volumeSlider->setEnabled( isEnabled() );
    ui->volumeSlider->setAttribute(Qt::WA_NoMousePropagation);

    //connect( ui->volumeSlider,  SIGNAL(valueChanged(int)),
    //   this, SIGNAL(valueChanged(int)) );

}

QToolButton* ControlPanel::playButton()
{
    return ui->playOrPauseButton;
}

QToolButton* ControlPanel::prevButton()
{
    return ui->prevActButton;
}
QToolButton* ControlPanel::nextButton()
{
    return ui->nextActButton;
}

QToolButton* ControlPanel::muteButton()
{
    return ui->muteButton;
}

QToolButton* ControlPanel::fullScreenButton()
{
    return ui->fullScreenButton;
}


QToolButton* ControlPanel::sidePanelButton()
{
    return ui->sidePanelButton;
}

QToolButton *ControlPanel::menuButton()
{
    return ui->menuButton;
}

MySlider* ControlPanel::volumeSlider()
{
    return ui->volumeSlider;
}

TimeSlider* ControlPanel::timeSlider()
{
    return ui->timeSlider;
}

QLabel* ControlPanel::currentTimeLabel()
{
    return ui->currentTimeLabel;
}

QLabel* ControlPanel::timeLabel()
{
    return ui->timeLabel;
}


