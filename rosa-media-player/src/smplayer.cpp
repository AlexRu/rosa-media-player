/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2010 Ricardo Villalba <rvm@escomposlinux.org>

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

#include "smplayer.h"
#include "defaultgui.h"
#include "global.h"
#include "paths.h"
#include "translator.h"
#include "version.h"
#include "constants.h"
#include "myclient.h"
#include "clhelp.h"

#include <QDir>
#include <QApplication>

#include <stdio.h>

#ifdef Q_OS_WIN
#if USE_ASSOCIATIONS
#include "extensions.h"
#include "winfileassoc.h"	//required for Uninstall
#endif
#endif


using namespace Global;

SMPlayer::SMPlayer(const QString & config_path, QObject * parent )
    : QObject(parent)
{
    main_window = 0;
    gui_to_use = "DefaultGui";

    close_at_end = -1; // Not set
    start_in_fullscreen = -1; // Not set

    move_gui = false;
    resize_gui = false;

    Paths::setAppPath( qApp->applicationDirPath() );

#ifndef PORTABLE_APP
    if (config_path.isEmpty()) createConfigDirectory();
#endif
    global_init(config_path);

    // Application translations
    translator->load( pref->language );
    showInfo();
}

SMPlayer::~SMPlayer()
{
    if (main_window != 0) delete main_window;
    global_end();
}

BaseGui * SMPlayer::gui()
{
    if (main_window == 0)
    {
        // Changes to app path, so smplayer can find a relative mplayer path
        QDir::setCurrent(Paths::appPath());
        qDebug("SMPlayer::gui: changed working directory to app path");
        qDebug("SMPlayer::gui: current directory: %s", QDir::currentPath().toUtf8().data());

        main_window = new DefaultGui(0);

        if (move_gui)
        {
            qDebug("SMPlayer::gui: moving main window to %d %d", gui_position.x(), gui_position.y());
            main_window->move(gui_position);
        }
        if (resize_gui)
        {
            qDebug("SMPlayer::gui: resizing main window to %dx%d", gui_size.width(), gui_size.height());
            main_window->resize(gui_size);
        }

        main_window->setForceCloseOnFinish(close_at_end);
        main_window->setForceStartInFullscreen(start_in_fullscreen);
    }

    return main_window;
}

SMPlayer::ExitCode SMPlayer::processArgs(QStringList args)
{
    qDebug("SMPlayer::processArgs: arguments: %d", args.count());
    for (int n = 0; n < args.count(); n++)
    {
        qDebug("SMPlayer::processArgs: %d = %s", n, args[n].toUtf8().data());
    }


    QString action; // Action to be passed to running instance
    bool show_help = false;

    if (!pref->gui.isEmpty()) gui_to_use = pref->gui;
    bool add_to_playlist = false;

    bool is_playlist = false;

#ifdef Q_OS_WIN
    if (args.contains("-uninstall"))
    {
#if USE_ASSOCIATIONS
        //Called by uninstaller. Will restore old associations.
        WinFileAssoc RegAssoc;
        Extensions exts;
        QStringList regExts;
        RegAssoc.GetRegisteredExtensions(exts.multimedia(), regExts);
        RegAssoc.RestoreFileAssociations(regExts);
        printf("Restored associations\n");
#endif
        return NoError;
    }
#endif

    for (int n = 1; n < args.count(); n++)
    {
        QString argument = args[n];

        if (argument == "-send-action")
        {
            if (n+1 < args.count())
            {
                n++;
                action = args[n];
            }
            else
            {
                printf("Error: expected parameter for -send-action\r\n");
                return ErrorArgument;
            }
        }
        else if (argument == "-actions")
        {
            if (n+1 < args.count())
            {
                n++;
                actions_list = args[n];
            }
            else
            {
                printf("Error: expected parameter for -actions\r\n");
                return ErrorArgument;
            }
        }
        else if (argument == "-sub")
        {
            if (n+1 < args.count())
            {
                n++;
                QString file = args[n];
                if (QFile::exists(file))
                {
                    subtitle_file = QFileInfo(file).absoluteFilePath();
                }
                else
                {
                    printf("Error: file '%s' doesn't exists\r\n", file.toUtf8().constData());
                }
            }
            else
            {
                printf("Error: expected parameter for -sub\r\n");
                return ErrorArgument;
            }
        }
        else if (argument == "-pos")
        {
            if (n+2 < args.count())
            {
                bool ok_x, ok_y;
                n++;
                gui_position.setX( args[n].toInt(&ok_x) );
                n++;
                gui_position.setY( args[n].toInt(&ok_y) );
                if (ok_x && ok_y) move_gui = true;
            }
            else
            {
                printf("Error: expected parameter for -pos\r\n");
                return ErrorArgument;
            }
        }
        else if (argument == "-size")
        {
            if (n+2 < args.count())
            {
                bool ok_width, ok_height;
                n++;
                gui_size.setWidth( args[n].toInt(&ok_width) );
                n++;
                gui_size.setHeight( args[n].toInt(&ok_height) );
                if (ok_width && ok_height) resize_gui = true;
            }
            else
            {
                printf("Error: expected parameter for -resize\r\n");
                return ErrorArgument;
            }
        }
        else if (argument == "-playlist")
        {
            is_playlist = true;
        }
        else if ((argument == "--help") || (argument == "-help") ||
                 (argument == "-h") || (argument == "-?") )
        {
            show_help = true;
        }
        else if (argument == "-close-at-end")
        {
            close_at_end = 1;
        }
        else if (argument == "-no-close-at-end")
        {
            close_at_end = 0;
        }
        else if (argument == "-fullscreen")
        {
            start_in_fullscreen = 1;
        }
        else if (argument == "-no-fullscreen")
        {
            start_in_fullscreen = 0;
        }
        else if (argument == "-add-to-playlist")
        {
            add_to_playlist = true;
        }
        else if (argument == "-defaultgui")
        {
            gui_to_use = "DefaultGui";
        }
        else
        {
            // File
            if (QFile::exists( argument ))
            {
                argument = QFileInfo(argument).absoluteFilePath();
            }
            if (is_playlist)
            {
                argument = argument + IS_PLAYLIST_TAG;
                is_playlist = false;
            }
            files_to_play.append( argument );
        }
    }

    if (show_help)
    {
        printf("%s\n", CLHelp::help().toLocal8Bit().data());
        return NoError;
    }

    qDebug("SMPlayer::processArgs: files_to_play: count: %d", files_to_play.count() );
    for (int n=0; n < files_to_play.count(); n++)
    {
        qDebug("SMPlayer::processArgs: files_to_play[%d]: '%s'", n, files_to_play[n].toUtf8().data());
    }


    if ( pref->use_single_instance ) {
        // Single instance
        int port = pref->connection_port;
        if ( pref->use_autoport ) port = pref->autoport;

        MyClient *c = new MyClient(port);
        //c->setTimeOut(1000);
        qDebug("SMPlayer::processArgs: trying to connect to port %d", port);

        if (c->openConnection())
        {
            qDebug("SMPlayer::processArgs: found another instance");

            if (!action.isEmpty())
            {
                if (c->sendAction(action))
                {
                    qDebug("SMPlayer::processArgs: action passed successfully to the running instance");
                }
                else
                {
                    printf("Error: action couldn't be passed to the running instance");
                    return NoAction;
                }
            }
            else
            {
                if (!subtitle_file.isEmpty())
                {
                    if (c->sendSubtitleFile(subtitle_file))
                    {
                        qDebug("SMPlayer::processArgs: subtitle file sent successfully to the running instance");
                    }
                    else
                    {
                        qDebug("SMPlayer::processArgs: subtitle file couldn't be sent to another instance");
                    }
                }

                if (!files_to_play.isEmpty())
                {
                    if (c->sendFiles(files_to_play, add_to_playlist))
                    {
                        qDebug("SMPlayer::processArgs: files sent successfully to the running instance");
                        qDebug("SMPlayer::processArgs: exiting.");
                    }
                    else
                    {
                        qDebug("SMPlayer::processArgs: files couldn't be sent to another instance");
                    }
                }
            }
            c->closeConnection();
            return NoError;
        }
        else
        {
            if (!action.isEmpty())
            {
                printf("Error: no running instance found\r\n");
                return NoRunningInstance;
            }
        }
    }

    if (!pref->default_font.isEmpty())
    {
        QFont f;
        f.fromString(pref->default_font);
        qApp->setFont(f);
    }

    return SMPlayer::NoExit;
}

void SMPlayer::start()
{
    if (!gui()->startHidden() || !files_to_play.isEmpty() ) gui()->show();
    if (!files_to_play.isEmpty())
    {
        if (!subtitle_file.isEmpty()) gui()->setInitialSubtitle(subtitle_file);
        gui()->openFiles(files_to_play);
    }

    if (!actions_list.isEmpty())
    {
        if (files_to_play.isEmpty())
        {
            gui()->runActions(actions_list);
        }
        else
        {
            gui()->runActionsLater(actions_list);
        }
    }
}

#ifndef PORTABLE_APP
void SMPlayer::createConfigDirectory()
{
    // Create smplayer config directory
    if (!QFile::exists(Paths::configPath()))
    {
        QDir d;
        if (!d.mkdir(Paths::configPath()))
        {
            qWarning("SMPlayer::createConfigDirectory: can't create %s", Paths::configPath().toUtf8().data());
        }
#if 0
        QString s = Paths::configPath() + "/screenshots";
        if (!d.mkdir(s))
        {
            qWarning("SMPlayer::createHomeDirectory: can't create %s", s.toUtf8().data());
        }
#endif
    }
}
#endif

void SMPlayer::showInfo()
{
#ifdef Q_OS_WIN
    QString win_ver;
    switch (QSysInfo::WindowsVersion)
    {
    case QSysInfo::WV_32s:
        win_ver = "Windows 3.1";
        break;
    case QSysInfo::WV_95:
        win_ver = "Windows 95";
        break;
    case QSysInfo::WV_98:
        win_ver = "Windows 98";
        break;
    case QSysInfo::WV_Me:
        win_ver = "Windows Me";
        break;
    case QSysInfo::WV_NT:
        win_ver = "Windows NT";
        break;
    case QSysInfo::WV_2000:
        win_ver = "Windows 2000";
        break;
    case QSysInfo::WV_XP:
        win_ver = "Windows XP";
        break;
    case QSysInfo::WV_2003:
        win_ver = "Windows Server 2003";
        break;
    case QSysInfo::WV_VISTA:
        win_ver = "Windows Vista";
        break;
#if QT_VERSION >= 0x040501
    case QSysInfo::WV_WINDOWS7:
        win_ver = "Windows 7";
        break;
#endif
    default:
        win_ver = QString("other: %1").arg(QSysInfo::WindowsVersion);
    }
#endif
    QString s = QObject::tr("This is ROSA Media Player running on %1")
#ifdef Q_OS_LINUX
                .arg("Linux")
#else
#ifdef Q_OS_WIN
                .arg("Windows ("+win_ver+")")
#else
                .arg("Other OS")
#endif
#endif
                ;

    printf("%s\n", s.toLocal8Bit().data() );
    qDebug("%s", s.toUtf8().data() );
    qDebug("Compiled with Qt v. %s, using %s", QT_VERSION_STR, qVersion());

    qDebug(" * application path: '%s'", Paths::appPath().toUtf8().data());
    qDebug(" * data path: '%s'", Paths::dataPath().toUtf8().data());
    qDebug(" * translation path: '%s'", Paths::translationPath().toUtf8().data());
    qDebug(" * doc path: '%s'", Paths::docPath().toUtf8().data());
    qDebug(" * themes path: '%s'", Paths::themesPath().toUtf8().data());
    qDebug(" * shortcuts path: '%s'", Paths::shortcutsPath().toUtf8().data());
    qDebug(" * config path: '%s'", Paths::configPath().toUtf8().data());
    qDebug(" * ini path: '%s'", Paths::iniPath().toUtf8().data());
    qDebug(" * file for subtitles' styles: '%s'", Paths::subtitleStyleFile().toUtf8().data());
    qDebug(" * current path: '%s'", QDir::currentPath().toUtf8().data());
}
