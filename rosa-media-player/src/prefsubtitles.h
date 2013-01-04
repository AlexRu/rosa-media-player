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

#ifndef _PREFINTERFACE_H_
#define _PREFINTERFACE_H_

#include "ui_prefsubtitles.h"
#include "prefwidget.h"

class Preferences;

class PrefSubtitles : public PrefWidget, public Ui::PrefSubtitles
{
    Q_OBJECT

public:
    PrefSubtitles( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~PrefSubtitles();

    virtual QString sectionName();
    virtual QPixmap sectionIcon();

    // Pass data to the dialog
    void setData( Preferences *pref );

    // Apply changes
    void getData( Preferences *pref );

protected:
    virtual void createHelp();

    void setFontEncoding( QString s );
    QString fontEncoding();

    void setFontFuzziness( int n );
    int fontFuzziness();

    void setAudioLang( QString lang );
    QString audioLang();

    void setSubtitleLang( QString lang );
    QString subtitleLang();

    void setAudioTrack( int track );
    int audioTrack();

    void setSubtitleTrack( int track );
    int subtitleTrack();

protected slots:

protected:
    virtual void retranslateStrings();

private:

};

#endif
