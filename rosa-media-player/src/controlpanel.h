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

#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>

class QToolButton;
class MySlider;
class TimeSlider;
class QLabel;

namespace Ui
{
class ControlPanel;
}

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = 0);
    ~ControlPanel();
    QToolButton*  playButton();
    QToolButton*  prevButton();
    QToolButton*  nextButton();
    QToolButton*  muteButton();
    QToolButton*  fullScreenButton();
    QToolButton*  sidePanelButton();
    QToolButton*  menuButton();

    MySlider*     volumeSlider();
    TimeSlider*   timeSlider();
    QLabel*       currentTimeLabel();
    QLabel*       timeLabel();


protected:
    void resizeEvent(QResizeEvent *);
    void initVolumeSlider();

signals:
    void valueChanged(int);

private:
    Ui::ControlPanel *ui;
};

#endif // CONTROLPANEL_H
