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

#include "playlist.h"

#include <QToolBar>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QRegExp>
#include <QMenu>
#include <QDateTime>
#include <QSettings>
#include <QInputDialog>
#include <QToolButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QHeaderView>
#include <QTextCodec>
#include <QApplication>
#include <QDebug>

#include "mytablewidget.h"
#include "myaction.h"
#include "filedialog.h"
#include "helper.h"
#include "images.h"
#include "preferences.h"
#include "version.h"
#include "global.h"
#include "core.h"
#include "extensions.h"
#include "guiconfig.h"
#include "playlistpreferences.h"

#include <stdlib.h>

#if USE_INFOPROVIDER
#include "infoprovider.h"
#endif

#define DRAG_ITEMS 0

#define COL_PLAY 0
#define COL_NAME 1
#define COL_TIME 2

using namespace Global;



Playlist::Playlist( Core *c, QWidget * parent, Qt::WindowFlags f)
    : QWidget(parent,f)
{
    save_playlist_in_config = true;
    recursive_add_directory = true;
#ifdef Q_OS_WIN
    automatically_get_info = false;
#else
    automatically_get_info = true;
#endif
    play_files_from_start = true;

    automatically_play_next = true;

    row_spacing = -1; // Default height

    modified = false;

    core = c;
    playlist_path = "";
    latest_dir = "";

    createTable();
    createActions();
    createToolbar();

    connect( core, SIGNAL(mediaFinished()), this, SLOT(playNext()), Qt::QueuedConnection );
    connect( core, SIGNAL(mediaLoaded()), this, SLOT(getMediaInfo()) );

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget( listView );
    layout->addWidget( toolbar );
    setLayout(layout);

    clear();

    retranslateStrings();
#if !DOCK_PLAYLIST
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding );
    adjustSize();
#else
    //setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Expanding );
    //setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
#endif

    setAcceptDrops(true);
    setAttribute(Qt::WA_NoMousePropagation);

    // Random seed
    QTime t;
    t.start();
    srand( t.hour() * 3600 + t.minute() * 60 + t.second() );

    loadSettings();

    // Ugly hack to avoid to play next item automatically
    if (!automatically_play_next)
    {
        disconnect( core, SIGNAL(mediaFinished()), this, SLOT(playNext()) );
    }

    // Save config every 5 minutes.
    save_timer = new QTimer(this);
    connect( save_timer, SIGNAL(timeout()), this, SLOT(maybeSaveSettings()) );
    save_timer->start( 5 * 60000 );
    remove_button->hide();

    setAutoFillBackground(true);

}

Playlist::~Playlist()
{
    saveSettings();
}

void Playlist::setModified(bool mod)
{
    qDebug("Playlist::setModified: %d", mod);

    modified = mod;
    emit modifiedChanged(modified);
}

void Playlist::createTable()
{
    listView = new MyTableWidget( 0, COL_TIME + 1, this);
    listView->setObjectName("playlist_table");
    listView->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    listView->setContextMenuPolicy( Qt::CustomContextMenu );
    listView->setShowGrid(false);
    listView->setSortingEnabled(false);
    //listView->setAlternatingRowColors(true);
    listView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    listView->horizontalHeader()->setResizeMode(COL_NAME, QHeaderView::Stretch);
    /*
    listView->horizontalHeader()->setResizeMode(COL_TIME, QHeaderView::ResizeToContents);
    listView->horizontalHeader()->setResizeMode(COL_PLAY, QHeaderView::ResizeToContents);
    */
    listView->setIconSize( Images::pixmap("ok").size() );

#if DRAG_ITEMS
    listView->setSelectionMode(QAbstractItemView::SingleSelection);
    listView->setDragEnabled(true);
    listView->setAcceptDrops(true);
    listView->setDropIndicatorShown(true);
    listView->setDragDropMode(QAbstractItemView::InternalMove);
#endif

    connect( listView, SIGNAL(cellActivated(int,int)),
             this, SLOT(itemDoubleClicked(int)) );
}

void Playlist::createActions()
{
    openAct = new MyAction(this, "pl_open", false);
    openAct->setIconVisibleInMenu( false );
    connect( openAct, SIGNAL(triggered()), this, SLOT(load()) );

    saveAct = new MyAction(this, "pl_save", false);
    saveAct->setIconVisibleInMenu( false );
    connect( saveAct, SIGNAL(triggered()), this, SLOT(save()) );

    playAct = new MyAction(this, "pl_play", false);
    playAct->setIconVisibleInMenu( false );
    connect( playAct, SIGNAL(triggered()), this, SLOT(playCurrent()) );

    nextAct = new MyAction(Qt::Key_N /*Qt::Key_Greater*/, this, "pl_next", false);
    nextAct->setIconVisibleInMenu( false );
    connect( nextAct, SIGNAL(triggered()), this, SLOT(playNext()) );

    prevAct = new MyAction(Qt::Key_P /*Qt::Key_Less*/, this, "pl_prev", false);
    prevAct->setIconVisibleInMenu( false );
    connect( prevAct, SIGNAL(triggered()), this, SLOT(playPrev()) );

    moveUpAct = new MyAction(this, "pl_move_up", false);
    moveUpAct->setIconVisibleInMenu( false );
    connect( moveUpAct, SIGNAL(triggered()), this, SLOT(upItem()) );

    moveDownAct = new MyAction(this, "pl_move_down", false);
    moveDownAct->setIconVisibleInMenu( false );
    connect( moveDownAct, SIGNAL(triggered()), this, SLOT(downItem()) );

    repeatAct = new MyAction(this, "pl_repeat", false);
    repeatAct->setIconVisibleInMenu( false );
    repeatAct->setCheckable(true);

    shuffleAct = new MyAction(this, "pl_shuffle", false);
    shuffleAct->setIconVisibleInMenu( false );
    shuffleAct->setCheckable(true);

    preferencesAct = new MyAction(this, "pl_preferences", false);
    preferencesAct->setIconVisibleInMenu( false );
    connect( preferencesAct, SIGNAL(triggered()), this, SLOT(editPreferences()) );

    // Add actions
    addCurrentAct = new MyAction(this, "pl_add_current", false);
    addCurrentAct->setIconVisibleInMenu( false );
    connect( addCurrentAct, SIGNAL(triggered()), this, SLOT(addCurrentFile()) );

    addFilesAct = new MyAction(this, "pl_add_files", false);
    addFilesAct->setIconVisibleInMenu( false );
    connect( addFilesAct, SIGNAL(triggered()), this, SLOT(addFiles()) );

    addDirectoryAct = new MyAction(this, "pl_add_directory", false);
    connect( addDirectoryAct, SIGNAL(triggered()), this, SLOT(addDirectory()) );

    // Remove actions
    removeSelectedAct = new MyAction(this, "pl_remove_selected", false);
    removeSelectedAct->setIconVisibleInMenu( false );
    connect( removeSelectedAct, SIGNAL(triggered()), this, SLOT(removeSelected()) );

    removeAllAct = new MyAction(this, "pl_remove_all", false);
    removeAllAct->setIconVisibleInMenu( false );
    connect( removeAllAct, SIGNAL(triggered()), this, SLOT(removeAll()) );

    // Edit
    editAct = new MyAction(this, "pl_edit", false);
    editAct->setIconVisibleInMenu( false );
    connect( editAct, SIGNAL(triggered()), this, SLOT(editCurrentItem()) );
}

void Playlist::createToolbar()
{
    toolbar = new QToolBar(this);
    toolbar->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    toolbar->setIconSize( QSize(24, 24) );

    SpacerWidget *sW1 = new SpacerWidget(this);
    toolbar->addWidget(sW1);

    add_menu = new QMenu( this );
    add_menu->addAction(openAct);
    add_menu->addAction(addCurrentAct);
    add_menu->addAction(addFilesAct );
    add_menu->addAction(addDirectoryAct);

    add_button = new QToolButton( this );
    add_button->setMenu( add_menu );
    add_button->setPopupMode(QToolButton::InstantPopup);

    remove_menu = new QMenu( this );
    remove_menu->addAction(removeSelectedAct);
    remove_menu->addAction(removeAllAct);

    remove_button = new QToolButton( this );
    remove_button->setMenu( remove_menu );
    remove_button->setPopupMode(QToolButton::InstantPopup);

    toolbar->addWidget(add_button);

    SpacerWidget *sW = new SpacerWidget(this);
    toolbar->addWidget(sW);

    toolbar->addAction(saveAct);

    SpacerWidget *sW2 = new SpacerWidget(this);
    toolbar->addWidget(sW2);

    toolbar->addAction(repeatAct);

    SpacerWidget *sW3 = new SpacerWidget(this);
    toolbar->addWidget(sW3);

    toolbar->addAction(shuffleAct);
    //toolbar->addSeparator();

    SpacerWidget *sW4 = new SpacerWidget(this);
    toolbar->addWidget(sW4);

    toolbar->addAction(moveUpAct);

    SpacerWidget *sW5 = new SpacerWidget(this);
    toolbar->addWidget(sW5);

    toolbar->addAction(moveDownAct);
    //toolbar->addSeparator();
    //toolbar->addAction(preferencesAct);

    // Popup menu
    popup = new QMenu(this);
    popup->addAction(playAct);
    popup->addAction(removeSelectedAct);
    popup->addAction(editAct);

    // set our palette for toolbar
    QPalette palette = toolbar->palette();
    palette.setColor(QPalette::Normal, QPalette::ButtonText, QColor("#505c6c"));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#9599a1"));
    toolbar->setPalette(palette);

    connect( listView, SIGNAL(customContextMenuRequested(const QPoint &)),
             this, SLOT(showPopup(const QPoint &)) );
}

void Playlist::retranslateStrings()
{
    listView->setHorizontalHeaderLabels( QStringList() << "   " <<
                                         tr("Name") << tr("Length") );

    openAct->change( Images::icon( "open" ), tr("&Load") );
    saveAct->change( Images::icon( "save" ), tr("&Save") );

    playAct->change( tr("&Play") );

    nextAct->change( tr("&Next") );
    prevAct->change( tr("Pre&vious") );

    if (qApp->isLeftToRight())
    {
        playAct->setIcon( Images::icon("play") );
        nextAct->setIcon( Images::icon("next") );
        prevAct->setIcon( Images::icon("previous") );
    }
    else
    {
        playAct->setIcon( Images::flippedIcon("play") );
        nextAct->setIcon( Images::flippedIcon("next") );
        prevAct->setIcon( Images::flippedIcon("previous") );
    }


    moveUpAct->change( Images::icon("up"), tr("Move &up") );
    moveDownAct->change( Images::icon("down"), tr("Move &down") );

    repeatAct->change( Images::icon( "repeat" ), tr( "&Repeat" ) );
    shuffleAct->change( Images::icon( "shuffle" ), tr( "S&huffle" ) );

    preferencesAct->change( Images::icon("prefs"), tr("Preferences") );

    // Add actions
    addCurrentAct->change( tr("Add &current file") );
    addFilesAct->change( tr("Add &file(s)") );
    addDirectoryAct->change( tr("Add &directory") );

    // Remove actions
    removeSelectedAct->change( tr("Remove &selected") );
    removeAllAct->change( tr("Remove &all") );

    // Edit
    editAct->change( tr("&Edit") );

    // Tool buttons
    add_button->setIcon( Images::icon("plus") );
    add_button->setToolTip( tr("Add...") );
    remove_button->setIcon( Images::icon("minus") );
    remove_button->setToolTip( tr("Remove...") );

    // Icon
    setWindowIcon( Images::icon("logo", 64) );
    setWindowTitle( tr( "ROSA Media Player - Playlist" ) );
}

void Playlist::list()
{
    qDebug("Playlist::list");

    PlaylistItemList::iterator it;
    for ( it = pl.begin(); it != pl.end(); ++it )
    {
        qDebug( "filename: '%s', name: '%s' duration: %f",
                (*it).filename().toUtf8().data(), (*it).name().toUtf8().data(),
                (*it).duration() );
    }
}

void Playlist::updateView()
{
    qDebug("Playlist::updateView");

    listView->setRowCount( pl.count() );

    //QString number;
    QString name;
    QString time;

    for (int n=0; n < pl.count(); n++)
    {
        name = pl[n].name();
        if (name.isEmpty()) name = pl[n].filename();
        time = Helper::formatTime( (int) pl[n].duration() );

        //listView->setText(n, COL_POS, number);
        qDebug("Playlist::updateView: name: '%s'", name.toUtf8().data());
        listView->setText(n, COL_NAME, name);
        listView->setText(n, COL_TIME, time);

        if (pl[n].played())
        {
            listView->setIcon(n, COL_PLAY, Images::icon("ok") );
        }
        else
        {
            listView->setIcon(n, COL_PLAY, QPixmap() );
        }

        if (row_spacing > -1) listView->setRowHeight(n, listView->font().pointSize() + row_spacing);
        for (int i=0; i < listView->columnCount(); i++)
        {
            QTableWidgetItem * item = listView->getItem(n,i);
            if (item)
                item->setToolTip(name);
        }
    }
    //listView->resizeColumnsToContents();
    listView->resizeColumnToContents(COL_PLAY);
    listView->resizeColumnToContents(COL_TIME);

    setCurrentItem(current_item);

    //adjustSize();
}

void Playlist::setCurrentItem(int current)
{
    QIcon play_icon;
    if ( qApp->isLeftToRight() ) {
        play_icon = Images::icon( "play1" );
    }
    else {
        play_icon = Images::flippedIcon( "play1" );
    }

    int old_current = current_item;
    current_item = current;

    if ( ( current_item > -1 ) && ( current_item < pl.count() ) ) {
        pl[current_item].setPlayed( TRUE );
    }

    if ( (old_current >= 0) && (old_current < listView->rowCount() ) ) {
        listView->setIcon(old_current, COL_PLAY, QPixmap() );
    }

    if ( (current_item >= 0) && (current_item < listView->rowCount()) ) {
        listView->setIcon(current_item, COL_PLAY, play_icon );
    }
    else {
        listView->setIcon( current_item, COL_PLAY, QPixmap() );
    }

    //if (current_item >= 0) listView->selectRow(current_item);
    if ( current_item >= 0 ) {
        listView->clearSelection();
        listView->setCurrentCell( current_item, 0);
    }
}

void Playlist::clear()
{
    pl.clear();

    listView->clearContents();
    listView->setRowCount(0);

    setCurrentItem(0);

    setModified( false );
}

int Playlist::count()
{
    return pl.count();
}

bool Playlist::isEmpty()
{
    return pl.isEmpty();
}

void Playlist::addItem(QString filename, QString name, double duration)
{
    qDebug("Playlist::addItem: '%s'", filename.toUtf8().data());

#ifdef Q_OS_WIN
    filename = Helper::changeSlashes(filename);
#endif

    // Test if already is in the list
    bool exists = false;
    for ( int n = 0; n < pl.count(); n++)
    {
        if ( pl[n].filename() == filename )
        {
            exists = true;
            int last_item =  pl.count()-1;
            pl.move(n, last_item);
            qDebug("Playlist::addItem: item already in list (%d), moved to %d", n, last_item);
            if (current_item > -1)
            {
                if (current_item > n) current_item--;
                else if (current_item == n) current_item = last_item;
            }
            break;
        }
    }

    if (!exists)
    {
        if (name.isEmpty())
        {
            QFileInfo fi(filename);
            // Let's see if it looks like a file (no dvd://1 or something)
            if (filename.indexOf(QRegExp("^.*://.*")) == -1)
            {
                // Local file
                name = fi.fileName(); //fi.baseName(TRUE);
            }
            else
            {
                // Stream
                name = filename;
            }
        }
        pl.append( PlaylistItem(filename, name, duration) );
        //setModified( true ); // Better set the modified on a higher level
    }
    else
    {
        qDebug("Playlist::addItem: item not added, already in the list");
    }
}

void Playlist::load_m3u(QString file)
{
    qDebug("Playlist::load_m3u");

    bool utf8 = (QFileInfo(file).suffix().toLower() == "m3u8");

    QRegExp m3u_id("^#EXTM3U|^#M3U");
    QRegExp info("^#EXTINF:(.*),(.*)");

    QFile f( file );
    if ( f.open( QIODevice::ReadOnly ) )
    {
        playlist_path = QFileInfo(file).path();

        clear();
        QString filename="";
        QString name="";
        double duration=0;

        QTextStream stream( &f );

        if (utf8)
            stream.setCodec("UTF-8");
        else
            stream.setCodec(QTextCodec::codecForLocale());

        QString line;
        while ( !stream.atEnd() )
        {
            line = stream.readLine(); // line of text excluding '\n'
            qDebug( " * line: '%s'", line.toUtf8().data() );
            if (m3u_id.indexIn(line)!=-1)
            {
                //#EXTM3U
                // Ignore line
            }
            else if (info.indexIn(line)!=-1)
            {
                duration = info.cap(1).toDouble();
                name = info.cap(2);
                qDebug(" * name: '%s', duration: %f", name.toUtf8().data(), duration );
            }
            else if (line.startsWith("#"))
            {
                // Comment
                // Ignore
            }
            else
            {
                filename = line;
                QFileInfo fi(filename);
                if (fi.exists())
                {
                    filename = fi.absoluteFilePath();
                }
                if (!fi.exists())
                {
                    if (QFileInfo( playlist_path + "/" + filename).exists() )
                    {
                        filename = playlist_path + "/" + filename;
                    }
                }
                addItem( filename, name, duration );
                name="";
                duration = 0;
            }
        }
        f.close();
        list();
        updateView();

        setModified( false );

        startPlay();
    }
}

void Playlist::load_pls(QString file)
{
    qDebug("Playlist::load_pls");

    if (!QFile::exists(file))
    {
        qDebug("Playlist::load_pls: '%s' doesn't exist, doing nothing", file.toUtf8().constData());
        return;
    }

    playlist_path = QFileInfo(file).path();

    QSettings set(file, QSettings::IniFormat);
    set.beginGroup("playlist");

    if (set.status() == QSettings::NoError)
    {
        clear();
        QString filename;
        QString name;
        double duration;

        int num_items = set.value("NumberOfEntries", 0).toInt();

        for (int n=0; n < num_items; n++)
        {
            filename = set.value("File"+QString::number(n+1), "").toString();
            name = set.value("Title"+QString::number(n+1), "").toString();
            duration = (double) set.value("Length"+QString::number(n+1), 0).toInt();

            QFileInfo fi(filename);
            if (fi.exists())
            {
                filename = fi.absoluteFilePath();
            }
            if (!fi.exists())
            {
                if (QFileInfo( playlist_path + "/" + filename).exists() )
                {
                    filename = playlist_path + "/" + filename;
                }
            }
            addItem( filename, name, duration );
        }
    }

    set.endGroup();

    list();
    updateView();

    setModified( false );

    if (set.status() == QSettings::NoError) startPlay();
}

bool Playlist::save_m3u(QString file)
{
    qDebug("Playlist::save_m3u: '%s'", file.toUtf8().data());

    QString dir_path = QFileInfo(file).path();
    if (!dir_path.endsWith("/")) dir_path += "/";

#ifdef Q_OS_WIN
    dir_path = Helper::changeSlashes(dir_path);
#endif

    qDebug(" * dirPath: '%s'", dir_path.toUtf8().data());

    bool utf8 = (QFileInfo(file).suffix().toLower() == "m3u8");

    QFile f( file );
    if ( f.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &f );

        if (utf8)
            stream.setCodec("UTF-8");
        else
            stream.setCodec(QTextCodec::codecForLocale());

        QString filename;

        stream << "#EXTM3U" << "\n";
        stream << "# Playlist created by ROSA Media Player" << " \n";

        PlaylistItemList::iterator it;
        for ( it = pl.begin(); it != pl.end(); ++it )
        {
            filename = (*it).filename();
#ifdef Q_OS_WIN
            filename = Helper::changeSlashes(filename);
#endif
            stream << "#EXTINF:";
            stream << (*it).duration() << ",";
            stream << (*it).name() << "\n";
            // Try to save the filename as relative instead of absolute
            if (filename.startsWith( dir_path ))
            {
                filename = filename.mid( dir_path.length() );
            }
            stream << filename << "\n";
        }
        f.close();

        setModified( false );
        return true;
    }
    else
    {
        return false;
    }
}


bool Playlist::save_pls(QString file)
{
    qDebug("Playlist::save_pls: '%s'", file.toUtf8().data());

    QString dir_path = QFileInfo(file).path();
    if (!dir_path.endsWith("/")) dir_path += "/";

#ifdef Q_OS_WIN
    dir_path = Helper::changeSlashes(dir_path);
#endif

    qDebug(" * dirPath: '%s'", dir_path.toUtf8().data());

    QSettings set(file, QSettings::IniFormat);
    set.beginGroup( "playlist");

    QString filename;

    PlaylistItemList::iterator it;
    for ( int n=0; n < pl.count(); n++ )
    {
        filename = pl[n].filename();
#ifdef Q_OS_WIN
        filename = Helper::changeSlashes(filename);
#endif

        // Try to save the filename as relative instead of absolute
        if (filename.startsWith( dir_path ))
        {
            filename = filename.mid( dir_path.length() );
        }

        set.setValue("File"+QString::number(n+1), filename);
        set.setValue("Title"+QString::number(n+1), pl[n].name());
        set.setValue("Length"+QString::number(n+1), (int) pl[n].duration());
    }

    set.setValue("NumberOfEntries", pl.count());
    set.setValue("Version", 2);

    set.endGroup();

    set.sync();

    bool ok = (set.status() == QSettings::NoError);
    if (ok) setModified( false );

    return ok;
}


void Playlist::load()
{
    if (maybeSave())
    {
        Extensions e;
        QString s = MyFileDialog::getOpenFileName(
                        this, tr("Choose a file"),
                        lastDir(),
                        tr("Playlists") + e.playlist().forFilter());

        if (!s.isEmpty())
        {
            latest_dir = QFileInfo(s).absolutePath();

            if (QFileInfo(s).suffix().toLower() == "pls")
                load_pls(s);
            else
                load_m3u(s);
        }
    }
}

bool Playlist::save()
{
    Extensions e;
    QString s = MyFileDialog::getSaveFileName(
                    this, tr("Choose a filename"),
                    lastDir(),
                    tr("Playlists") + e.playlist().forFilter());

    if (!s.isEmpty())
    {
        // If filename has no extension, add it
        if (QFileInfo(s).suffix().isEmpty())
        {
            s = s + ".m3u";
        }
        if (QFileInfo(s).exists())
        {
            int res = QMessageBox::question( this,
                                             tr("Confirm overwrite?"),
                                             tr("The file %1 already exists.\n"
                                                "Do you want to overwrite?").arg(s),
                                             QMessageBox::Yes,
                                             QMessageBox::No,
                                             QMessageBox::NoButton);
            if (res == QMessageBox::No )
            {
                return false;
            }
        }
        latest_dir = QFileInfo(s).absolutePath();

        if (QFileInfo(s).suffix().toLower() == "pls")
            return save_pls(s);
        else
            return save_m3u(s);

    }
    else
    {
        return false;
    }
}

bool Playlist::maybeSave()
{
    if (!isModified()) return true;

    int res = QMessageBox::question( this,
                                     tr("Playlist modified"),
                                     tr("There are unsaved changes, do you want to save the playlist?"),
                                     QMessageBox::Yes,
                                     QMessageBox::No,
                                     QMessageBox::Cancel);

    switch (res)
    {
    case QMessageBox::No :
        return true; // Discard changes
    case QMessageBox::Cancel :
        return false; // Cancel operation
    default :
        return save();
    }
}

void Playlist::playCurrent()
{
    int current = listView->currentRow();
    if (current > -1)
    {
        playItem(current);
    }
}

void Playlist::itemDoubleClicked(int row)
{
    qDebug("Playlist::itemDoubleClicked: row: %d", row );
    playItem(row);
}

void Playlist::showPopup(const QPoint & pos)
{
    qDebug("Playlist::showPopup: x: %d y: %d", pos.x(), pos.y() );

    if (!popup->isVisible())
    {
        popup->move( listView->viewport()->mapToGlobal(pos) );
        popup->show();
    }
}

void Playlist::startPlay()
{
    // Start to play
    if ( shuffleAct->isChecked() )
        playItem( chooseRandomItem() );
    else
        playItem(0);
}

void Playlist::playItem( int n )
{
    qDebug("Playlist::playItem: %d (count:%d)", n, pl.count());

    if ( (n >= pl.count()) || (n < 0) )
    {
        qDebug("Playlist::playItem: out of range");
        emit playlistEnded();
        return;
    }

    qDebug(" playlist_path: '%s'", playlist_path.toUtf8().data() );

    QString filename = pl[n].filename();
    QString filename_with_path = playlist_path + "/" + filename;

    if (!filename.isEmpty())
    {
        //pl[n].setPlayed(TRUE);
        setCurrentItem(n);
        if (play_files_from_start)
            core->open(filename, 0);
        else
            core->open(filename);
    }

}

void Playlist::playNext()
{
    qDebug("Playlist::playNext");

    if (shuffleAct->isChecked())
    {
        // Shuffle
        int chosen_item = chooseRandomItem();
        if (chosen_item == -1)
        {
            clearPlayedTag();
            if (repeatAct->isChecked()) chosen_item = chooseRandomItem();
        }
        playItem( chosen_item );
    }
    else
    {
        bool finished_list = (current_item+1 >= pl.count());
        if (finished_list) clearPlayedTag();

        if ( (repeatAct->isChecked()) && (finished_list) )
        {
            playItem(0);
        }
        else
        {
            playItem( current_item+1 );
        }
    }
}

void Playlist::playPrev()
{
    qDebug("Playlist::playPrev");
    if (current_item > 0)
    {
        playItem( current_item-1 );
    }
    else
    {
        if (pl.count() > 1) playItem( pl.count() -1 );
    }
}

void Playlist::getMediaInfo()
{
    qDebug("Playlist:: getMediaInfo");

    QString filename = core->mdat.filename;
    double duration = core->mdat.duration;
    QString name = core->mdat.clip_name;
    QString artist = core->mdat.clip_artist;

#ifdef Q_OS_WIN
    filename = Helper::changeSlashes(filename);
#endif

    if (name.isEmpty())
    {
        QFileInfo fi(filename);
        if (fi.exists())
        {
            // Local file
            name = fi.fileName();
        }
        else
        {
            // Stream
            name = filename;
        }
    }
    if (!artist.isEmpty()) name = artist + " - " + name;

    int pos=0;
    PlaylistItemList::iterator it;
    for ( it = pl.begin(); it != pl.end(); ++it )
    {
        if ( (*it).filename() == filename )
        {
            if ((*it).duration()<1)
            {
                if (!name.isEmpty())
                {
                    (*it).setName(name);
                }
                (*it).setDuration(duration);
                //setModified( true );
            }
            else
                // Edited name (sets duration to 1)
                if ((*it).duration()==1)
                {
                    (*it).setDuration(duration);
                    //setModified( true );
                }
            setCurrentItem(pos);
        }
        pos++;
    }
    updateView();
}

// Add current file to playlist
void Playlist::addCurrentFile()
{
    qDebug("Playlist::addCurrentFile");
    if ( !core->mdat.filename.isEmpty() ) {
        addItem( core->mdat.filename, "", 0 );
        getMediaInfo();
    }
}

void Playlist::addFiles()
{
    Extensions e;
    QStringList files = MyFileDialog::getOpenFileNames(
                            this, tr( "Select one or more files to open" ),
                            lastDir(),
                            tr( "Multimedia" ) + e.multimedia().forFilter() + ";;" +
                            tr( "All files" ) + " (*.*)" );

    if ( files.count() != 0 ) addFiles( files );
}

void Playlist::addFiles(QStringList files, AutoGetInfo auto_get_info)
{
    qDebug("Playlist::addFiles");

#if USE_INFOPROVIDER
    bool get_info = ( auto_get_info == GetInfo );
    if ( auto_get_info == UserDefined ) {
        get_info = automatically_get_info;
    }

    MediaData data;
    setCursor( Qt::WaitCursor );
#endif

    QStringList::Iterator it = files.begin();
    while( it != files.end() )
    {
#if USE_INFOPROVIDER
        if ( (get_info) && (QFile::exists((*it))) )
        {
            data = InfoProvider::getInfo( (*it) );
            addItem( (*it), data.displayName(), data.duration );
            //updateView();
            //qApp->processEvents();
        }
        else
        {
            addItem( (*it), "", 0 );
        }
#else
        addItem( (*it), "", 0 );
#endif

        if (QFile::exists(*it))
        {
            latest_dir = QFileInfo((*it)).absolutePath();
        }

        ++it;
    }
#if USE_INFOPROVIDER
    unsetCursor();
#endif
    updateView();

    qDebug( "Playlist::addFiles: latest_dir: '%s'", latest_dir.toUtf8().constData() );
}

void Playlist::addFile(QString file, AutoGetInfo auto_get_info)
{
    addFiles( QStringList() << file, auto_get_info );
}

void Playlist::addDirectory()
{
    QString s = MyFileDialog::getExistingDirectory(
                    this, tr("Choose a directory"),
                    lastDir() );

    if (!s.isEmpty())
    {
        addDirectory(s);
        latest_dir = s;
    }
}

void Playlist::addOneDirectory(QString dir)
{
    QStringList filelist;

    Extensions e;
    QRegExp rx_ext(e.multimedia().forRegExp());
    rx_ext.setCaseSensitivity(Qt::CaseInsensitive);

    QStringList dir_list = QDir(dir).entryList();

    QString filename;
    QStringList::Iterator it = dir_list.begin();
    while( it != dir_list.end() )
    {
        filename = dir;
        if (filename.right(1)!="/") filename += "/";
        filename += (*it);
        QFileInfo fi(filename);
        if (!fi.isDir())
        {
            if (rx_ext.indexIn(fi.suffix()) > -1)
            {
                filelist << filename;
            }
        }
        ++it;
    }
    addFiles(filelist);
}

void Playlist::addDirectory(QString dir)
{
    addOneDirectory(dir);

    if (recursive_add_directory)
    {
        QFileInfoList dir_list = QDir(dir).entryInfoList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
        for (int n=0; n < dir_list.count(); n++)
        {
            if (dir_list[n].isDir())
            {
                qDebug("Playlist::addDirectory: adding directory: %s", dir_list[n].filePath().toUtf8().data());
                addDirectory(dir_list[n].filePath());
            }
        }
    }
}

// Remove selected items
void Playlist::removeSelected()
{
    qDebug("Playlist::removeSelected");

    int first_selected = -1;
    int number_previous_item = 0;

    bool played_file_erased = false;

    for (int n=0; n < listView->rowCount(); n++)
    {
        if (listView->isSelected(n, 0))
        {
            qDebug(" row %d selected", n);
            pl[n].setMarkForDeletion(TRUE);
            number_previous_item++;
            if (first_selected == -1) first_selected = n;
        }
    }

    PlaylistItemList::iterator it;
    for ( it = pl.begin(); it != pl.end(); ++it )
    {
        if ( (*it).markedForDeletion() )
        {
            qDebug("Remove '%s'", (*it).filename().toUtf8().data());

            if ( (*it).played() ) {
                played_file_erased = true;
            }

            it = pl.erase(it);
            it--;
            setModified( true );

        }
    }

    if ( first_selected < current_item ) {
        current_item -= number_previous_item;
    }

    if ( played_file_erased ) {
        current_item = -1;
    }

    if (isEmpty()) setModified(false);
    updateView();

    if (first_selected >= listView->rowCount())
        first_selected = listView->rowCount() - 1;

    if ( ( first_selected > -1) && ( first_selected < listView->rowCount() ) )
    {
        listView->clearSelection();
        listView->setCurrentCell( first_selected, 0);
        //listView->selectRow( first_selected );
    }
}

void Playlist::removeAll()
{
    /*
    pl.clear();
    updateView();
    setModified( false );
    */
    clear();
}

void Playlist::clearPlayedTag()
{
    PlaylistItemList::iterator it;
    for ( it = pl.begin(); it != pl.end(); ++it )
    {
        (*it).setPlayed(FALSE);
    }
    updateView();
}

int Playlist::chooseRandomItem()
{
    qDebug( "Playlist::chooseRandomItem");
    QList <int> fi; //List of not played items (free items)

    int n=0;
    PlaylistItemList::iterator it;
    for ( it = pl.begin(); it != pl.end(); ++it )
    {
        if (! (*it).played() ) fi.append(n);
        n++;
    }

    qDebug(" * free items: %d", fi.count() );

    if (fi.count()==0) return -1; // none free

    qDebug(" * items: ");
    for (int i=0; i < fi.count(); i++)
    {
        qDebug("   * item: %d", fi[i]);
    }

    int selected = (int) ((double) fi.count() * rand()/(RAND_MAX+1.0));
    qDebug(" * selected item: %d (%d)", selected, fi[selected]);
    return fi[selected];
}

void Playlist::swapItems(int item1, int item2 )
{
    PlaylistItem it1 = pl[item1];
    pl[item1] = pl[item2];
    pl[item2] = it1;
    setModified( true );
}


void Playlist::upItem()
{
    qDebug("Playlist::upItem");

    int current = listView->currentRow();
    qDebug(" currentRow: %d", current );

    if (current >= 1)
    {
        swapItems( current, current-1 );
        if (current_item == (current-1)) current_item = current;
        else if (current_item == current) current_item = current-1;
        updateView();
        listView->clearSelection();
        listView->setCurrentCell( current-1, 0);
    }
}

void Playlist::downItem()
{
    qDebug("Playlist::downItem");

    int current = listView->currentRow();
    qDebug(" currentRow: %d", current );

    if ( (current > -1) && (current < (pl.count()-1)) )
    {
        swapItems( current, current+1 );
        if (current_item == (current+1)) current_item = current;
        else if (current_item == current) current_item = current+1;
        updateView();
        listView->clearSelection();
        listView->setCurrentCell( current+1, 0);
    }
}

void Playlist::editCurrentItem()
{
    int current = listView->currentRow();
    if (current > -1) editItem(current);
}

void Playlist::editItem(int item)
{
    QString current_name = pl[item].name();
    if (current_name.isEmpty()) current_name = pl[item].filename();

    bool ok;
    QString text = QInputDialog::getText( this,
                                          tr("Edit name"),
                                          tr("Type the name that will be displayed in the playlist for this file:"),
                                          QLineEdit::Normal,
                                          current_name, &ok );
    if ( ok && !text.isEmpty() )
    {
        // user entered something and pressed OK
        pl[item].setName(text);

        // If duration == 0 the name will be overwritten!
        if (pl[item].duration()<1) pl[item].setDuration(1);
        updateView();

        setModified( true );
    }
}

void Playlist::editPreferences()
{
    PlaylistPreferences d(this);

    d.setDirectoryRecursion(recursive_add_directory);
    d.setAutoGetInfo(automatically_get_info);
    d.setSavePlaylistOnExit(save_playlist_in_config);
    d.setPlayFilesFromStart(play_files_from_start);

    if (d.exec() == QDialog::Accepted)
    {
        recursive_add_directory = d.directoryRecursion();
        automatically_get_info = d.autoGetInfo();
        save_playlist_in_config = d.savePlaylistOnExit();
        play_files_from_start = d.playFilesFromStart();
    }
}

// Drag&drop
void Playlist::dragEnterEvent( QDragEnterEvent *e )
{
    qDebug("Playlist::dragEnterEvent");

    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void Playlist::dropEvent( QDropEvent *e )
{
    qDebug("Playlist::dropEvent");

    QStringList files;

    if (e->mimeData()->hasUrls())
    {
        QList <QUrl> l = e->mimeData()->urls();
        QString s;
        for (int n=0; n < l.count(); n++)
        {
            if (l[n].isValid())
            {
                qDebug("Playlist::dropEvent: scheme: '%s'", l[n].scheme().toUtf8().data());
                if (l[n].scheme() == "file")
                    s = l[n].toLocalFile();
                else
                    s = l[n].toString();
                /*
                qDebug(" * '%s'", l[n].toString().toUtf8().data());
                qDebug(" * '%s'", l[n].toLocalFile().toUtf8().data());
                */
                qDebug("Playlist::dropEvent: file: '%s'", s.toUtf8().data());
                files.append(s);
            }
        }
    }


    QStringList only_files;
    for (int n = 0; n < files.count(); n++)
    {
        if ( QFileInfo( files[n] ).isDir() )
        {
            addDirectory( files[n] );
        }
        else
        {
            only_files.append( files[n] );
        }
    }
    addFiles( only_files );
}


void Playlist::hideEvent( QHideEvent * )
{
    emit visibilityChanged(false);
}

void Playlist::showEvent( QShowEvent * )
{
    emit visibilityChanged(true);
}

void Playlist::closeEvent( QCloseEvent * e )
{
    saveSettings();
    e->accept();
}


void Playlist::maybeSaveSettings()
{
    qDebug("Playlist::maybeSaveSettings");
    if (isModified()) saveSettings();
}

void Playlist::saveSettings()
{
    qDebug("Playlist::saveSettings");

    QSettings * set = settings;

    set->beginGroup( "playlist");

    set->setValue( "repeat", repeatAct->isChecked() );
    set->setValue( "shuffle", shuffleAct->isChecked() );

    set->setValue( "auto_get_info", automatically_get_info );
//    set->setValue( "recursive_add_directory", recursive_add_directory );
    set->setValue( "save_playlist_in_config", save_playlist_in_config );
    set->setValue( "play_files_from_start", play_files_from_start );
    set->setValue( "automatically_play_next", automatically_play_next );

    set->setValue( "row_spacing", row_spacing );

#if !DOCK_PLAYLIST
    set->setValue( "size", size() );
#endif
    set->setValue( "latest_dir", latest_dir );

    set->endGroup();

    if (save_playlist_in_config)
    {
        //Save current list
        set->beginGroup( "playlist_contents");

        set->setValue( "count", (int) pl.count() );
        for ( int n=0; n < pl.count(); n++ )
        {
            set->setValue( QString("item_%1_filename").arg(n), pl[n].filename() );
            set->setValue( QString("item_%1_duration").arg(n), pl[n].duration() );
            set->setValue( QString("item_%1_name").arg(n), pl[n].name() );
        }
        set->setValue( "current_item", current_item );
        set->setValue( "modified", modified );

        set->endGroup();
    }
}

void Playlist::loadSettings()
{
    qDebug("Playlist::loadSettings");

    QSettings * set = settings;

    set->beginGroup( "playlist");

    repeatAct->setChecked( set->value( "repeat", repeatAct->isChecked() ).toBool() );
    shuffleAct->setChecked( set->value( "shuffle", shuffleAct->isChecked() ).toBool() );

    automatically_get_info = set->value( "auto_get_info", automatically_get_info ).toBool();
//    recursive_add_directory = set->value( "recursive_add_directory", recursive_add_directory ).toBool();
    save_playlist_in_config = set->value( "save_playlist_in_config", save_playlist_in_config ).toBool();
    play_files_from_start = set->value( "play_files_from_start", play_files_from_start ).toBool();
    automatically_play_next = set->value( "automatically_play_next", automatically_play_next ).toBool();

    row_spacing = set->value( "row_spacing", row_spacing ).toInt();

#if !DOCK_PLAYLIST
    resize( set->value("size", size()).toSize() );
#endif

    latest_dir = set->value( "latest_dir", latest_dir ).toString();

    set->endGroup();

    if (save_playlist_in_config)
    {
        //Load latest list
        set->beginGroup( "playlist_contents");

        int count = set->value( "count", 0 ).toInt();
        QString filename, name;
        double duration;
        for ( int n=0; n < count; n++ )
        {
            filename = set->value( QString("item_%1_filename").arg(n), "" ).toString();
            duration = set->value( QString("item_%1_duration").arg(n), -1 ).toDouble();
            name = set->value( QString("item_%1_name").arg(n), "" ).toString();
            addItem( filename, name, duration );
        }
        setCurrentItem( set->value( "current_item", -1 ).toInt() );
        setModified( set->value( "modified", false ).toBool() );
        updateView();

        set->endGroup();
    }
}

QString Playlist::lastDir()
{
    QString last_dir = latest_dir;
    if (last_dir.isEmpty()) last_dir = pref->latest_dir;
    return last_dir;
}

// Language change stuff
void Playlist::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        retranslateStrings();
    }
    else
    {
        QWidget::changeEvent(e);
    }
}

#include "moc_playlist.cpp"
