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


#include "prefinput.h"
#include "images.h"
#include "config.h"
#include "guiconfig.h"

PrefInput::PrefInput(QWidget * parent, Qt::WindowFlags f)
    : PrefWidget(parent, f )
{
    setupUi(this);

    retranslateStrings();
}

PrefInput::~PrefInput()
{
}

QString PrefInput::sectionName()
{
    return tr("Advanced");
}

QPixmap PrefInput::sectionIcon()
{
    return Images::icon("preferences-other");
}

void PrefInput::retranslateStrings()
{
    retranslateUi(this);

    keyboard_icon->setPixmap( Images::icon("keyboard") );

#if !USE_SHORTCUTGETTER
    actioneditor_desc->setText(
        tr("Here you can change any key shortcut. To do it double click or "
           "start typing over a shortcut cell. Optionally you can also save "
           "the list to share it with other people or load it in another "
           "computer.") );
#endif

    createHelp();
}

void PrefInput::setData(Preferences * pref)
{
    Q_UNUSED( pref )
}

void PrefInput::getData(Preferences * pref)
{
    Q_UNUSED( pref )
}

/*
void PrefInput::setActionsList(QStringList l) {
	left_click_combo->insertStringList( l );
	double_click_combo->insertStringList( l );
}
*/

void PrefInput::createHelp()
{
    clearHelp();

    addSectionTitle(tr("Keyboard"));

    setWhatsThis(actions_editor, tr("Shortcut editor"),
                 tr("This table allows you to change the key shortcuts of most "
                    "available actions. Double click or press enter on a item, or "
                    "press the <b>Change shortcut</b> button to enter in the "
                    "<i>Modify shortcut</i> dialog. There are two ways to change a "
                    "shortcut: if the <b>Capture</b> button is on then just "
                    "press the new key or combination of keys that you want to "
                    "assign for the action (unfortunately this doesn't work for all "
                    "keys). If the <b>Capture</b> button is off "
                    "then you could enter the full name of the key.") );
}

#include "moc_prefinput.cpp"
