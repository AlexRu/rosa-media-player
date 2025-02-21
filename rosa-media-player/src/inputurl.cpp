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


#include "inputurl.h"
#include "images.h"
#include <QAbstractButton>
#include <QUrl>

InputURL::InputURL( QWidget* parent, Qt::WindowFlags f )
    : QDialog(parent, f)
{
    setupUi(this);

    url_edit->setFocus();

    playlist_check->setWhatsThis(
        tr("If this option is checked, the URL will be treated as a playlist: "
           "it will be opened as text and will play the URLs in it.") );

    connect(url_edit, SIGNAL(editTextChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect(url_edit, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
    connect(playlist_check, SIGNAL(stateChanged(int)), this, SLOT(playlistChanged(int)));
    foreach (QAbstractButton* button, buttonBox->buttons())
    button->setIcon(QIcon());
}

InputURL::~InputURL()
{
}

void InputURL::setURL(QString url, bool is_playlist)
{
    url_edit->addItem(url, is_playlist);
}

QString InputURL::url()
{
    return url_edit->currentText().trimmed();
}

void InputURL::setPlaylist(bool b)
{
    playlist_check->setChecked(b);
    /*
    int pos = url_edit->currentIndex();
    url_edit->setItemData(pos, b);
    */
}

bool InputURL::isPlaylist()
{
    return playlist_check->isChecked();
}

void InputURL::indexChanged(int)
{
    qDebug("InputURL::indexChanged");
    int pos = url_edit->currentIndex();
    if (url_edit->itemText(pos) == url_edit->currentText())
    {
        playlist_check->setChecked( url_edit->itemData(pos).toBool() );
    }
}

void InputURL::textChanged(const QString & new_text)
{
    qDebug("InputURL::textChanged");
    QString s = new_text.trimmed();    
    QUrl url(s);
    QRegExp rx("\\.ram$|\\.asx$|\\.m3u$|\\.pls$", Qt::CaseInsensitive);
    setPlaylist( (rx.indexIn(url.path()) != -1) );
}

void InputURL::playlistChanged(int state)
{
    Q_UNUSED( state )
    /*
    int pos = url_edit->currentIndex();
    if (url_edit->itemText(pos) == url_edit->currentText()) {
    	bool is_playlist = (state == Qt::Checked);
    	url_edit->setItemIcon( pos, is_playlist ? Images::icon("playlist") : QIcon() );
    }
    */
}

#include "moc_inputurl.cpp"
