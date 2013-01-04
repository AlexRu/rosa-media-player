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

#ifndef _PREFGENERAL_H_
#define _PREFGENERAL_H_

#include "ui_prefgeneral.h"
#include "prefwidget.h"
#include "inforeader.h"
#include "deviceinfo.h"
#include "preferences.h"

#ifdef Q_OS_WIN
#define USE_DSOUND_DEVICES 1
#else
#define USE_ALSA_DEVICES 1
#define USE_XV_ADAPTORS 1
#endif

class PrefGeneral : public PrefWidget, public Ui::PrefGeneral
{
    Q_OBJECT

public:
    PrefGeneral( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~PrefGeneral();

    // Return the name of the section
    virtual QString sectionName();
    // Return the icon of the section
    virtual QPixmap sectionIcon();

    // Pass data to the dialog
    void setData(Preferences * pref);

    // Apply changes
    void getData(Preferences * pref);

protected:
    virtual void createHelp();

    void setRememberSettings(bool b);
    bool rememberSettings();

    void setCloseOnFinish(bool b);
    bool closeOnFinish();

    void setPauseWhenHidden(bool b);
    bool pauseWhenHidden();

#ifdef Q_OS_WIN
    void setAvoidScreensaver(bool b);
    bool avoidScreensaver();

    void setTurnScreensaverOff(bool b);
    bool turnScreensaverOff();
#else
    void setDisableScreensaver(bool b);
    bool disableScreensaver();
#endif

    void setAutoSyncActivated(bool b);
    bool autoSyncActivated();

    void setAudioChannels(int ID);
    int audioChannels();

    void setAutoAddFilesToPlaylist( bool b );
    bool autoAddFilesToPlaylist();

    void setResizeMethod(int v);
    int resizeMethod();

    void setSaveSize(bool b);
    bool saveSize();

    void setInitialVolNorm(bool b);
    bool initialVolNorm();

    void setUseSingleInstance(bool b);
    bool useSingleInstance();

    void setYoutubeEnabled(bool b);
    bool youtubeEnabled() const;

protected slots:

protected:
    virtual void retranslateStrings();

    InfoList vo_list;
    InfoList ao_list;

#if USE_DSOUND_DEVICES
    DeviceList dsound_devices;
#endif

#if USE_ALSA_DEVICES
    DeviceList alsa_devices;
#endif
#if USE_XV_ADAPTORS
    DeviceList xv_adaptors;
#endif

private:

};

#endif
