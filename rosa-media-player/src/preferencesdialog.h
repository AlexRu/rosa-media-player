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


#ifndef _PREFERENCESDIALOG_H_
#define _PREFERENCESDIALOG_H_

#include "ui_preferencesdialog.h"

/*
#ifdef Q_OS_WIN
#define USE_ASSOCIATIONS 1
#endif
*/

class QTextBrowser;
class QPushButton;

class PrefWidget;
class PrefGeneral;
class PrefSubtitles;
class PrefAssociations;
class Preferences;

class SectionItem : public QWidget
{
    Q_OBJECT

public:
    SectionItem( const QString &text, const QPixmap &pixmap, QWidget *parent = 0 );

    void setText( const QString &text );
    void setIcon( const QPixmap &icon );

private:
    QLabel *m_icon;
    QLabel *m_text;
};

class PreferencesDialog : public QDialog, public Ui::PreferencesDialog
{
    Q_OBJECT

public:
    enum Section { General=0, View=1 };

    PreferencesDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~PreferencesDialog();

    PrefGeneral * mod_general()
    {
        return page_general;
    }

    void addSection(PrefWidget *w);

    // Pass data to the standard dialogs
    void setData(Preferences * pref);

    // Apply changes
    void getData(Preferences * pref);

    // Return true if the mplayer process should be restarted.
    bool requiresRestart();

public slots:
    void showSection(Section s);

    virtual void accept(); // Reimplemented to send a signal
    virtual void reject();

signals:
    void applied();

protected:
    virtual void retranslateStrings();
    virtual void changeEvent ( QEvent * event ) ;

protected slots:
    void apply();
    void showHelp();
    void updateSection( int );

protected:
    PrefGeneral * page_general;
    PrefSubtitles * page_subtitles;

#if USE_ASSOCIATIONS
    PrefAssociations* page_associations;
#endif

    QTextBrowser * help_window;

private:
    QPushButton * closeButton;
    QPushButton * applyButton;
};

#endif
