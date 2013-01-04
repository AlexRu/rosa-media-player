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


#include "filepropertiesdialog.h"
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QAbstractButton>
#include <QDebug>

#include "global.h"
#include "preferences.h"
#include "images.h"
#include "infofile.h"
#include "ffmpegprocess.h"

FilePropertiesDialog::FilePropertiesDialog( QWidget* parent, Qt::WindowFlags f )
    : QDialog( parent, f )
{
    setupUi( this );

    // Setup buttons
    okButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    applyButton = buttonBox->button(QDialogButtonBox::Apply);
    connect( applyButton, SIGNAL(clicked()), this, SLOT(apply()) );

    codecs_set = FALSE;

    // Read codec info from InfoReader:
    InfoReader *i = InfoReader::obj();
    setCodecs( i->vcList(), i->acList(), i->demuxerList() );

    retranslateStrings();
    foreach (QAbstractButton* button, buttonBox->buttons())
        button->setIcon(QIcon());
}

FilePropertiesDialog::~FilePropertiesDialog()
{
}

void FilePropertiesDialog::setMediaData( MediaData md )
{
    media_data = md;

    // get metadata info with ffmpeg if has no info with mplayer
   // if ( media_data.clip_name.isEmpty() ) {
    {
        MediaData ffmpeg_md;

        FfmpegProcess *ffmpeg_proc = new FfmpegProcess;
        if ( ffmpeg_proc ) {
            QString ffmpeg_bin = Global::pref->ffmpeg_bin;

            QFileInfo fi( ffmpeg_bin );
            if ( fi.exists() && fi.isExecutable() && !fi.isDir() ) {
                ffmpeg_bin = fi.absoluteFilePath();
            }

            ffmpeg_proc->addArgument( ffmpeg_bin );
            ffmpeg_proc->addArgument( "-i" );
            ffmpeg_proc->addArgument( media_data.filename );

            ffmpeg_proc->start();
            if ( !ffmpeg_proc->waitForFinished() ) {
                qWarning("FilePropertiesDialog::setMediaData: process didn't finish. Killing it...");
                ffmpeg_proc->kill();
            }

            ffmpeg_md = ffmpeg_proc->mediaData();
            delete ffmpeg_proc;

            if ( !ffmpeg_md.clip_name.isEmpty() ) {
                media_data.clip_name = ffmpeg_md.clip_name;
            }
            if ( !ffmpeg_md.clip_track.isEmpty() ) {
                media_data.clip_track = ffmpeg_md.clip_track;
            }
            if ( !ffmpeg_md.clip_album.isEmpty() ) {
                media_data.clip_album = ffmpeg_md.clip_album;
            }
            if ( !ffmpeg_md.clip_artist.isEmpty() ) {
                media_data.clip_artist = ffmpeg_md.clip_artist;
            }
            if ( !ffmpeg_md.clip_date.isEmpty() ) {
                media_data.clip_date = ffmpeg_md.clip_date;
            }
            if ( !ffmpeg_md.clip_copyright.isEmpty() ) {
                media_data.clip_copyright = ffmpeg_md.clip_copyright;
            }
            if ( !ffmpeg_md.clip_software.isEmpty() ) {
                media_data.clip_software = ffmpeg_md.clip_software;
            }
            if ( !ffmpeg_md.clip_comment.isEmpty() ) {
                media_data.clip_comment = ffmpeg_md.clip_comment;
            }
        }
    }

    showInfo();
}

void FilePropertiesDialog::showInfo()
{
    InfoFile info;
    info_edit->setText( info.getInfo( media_data ) );
}

void FilePropertiesDialog::retranslateStrings()
{
    retranslateUi(this);

    setWindowIcon( Images::icon("logo") );

    showInfo();

    // Qt 4.2 doesn't update the buttons' text
#if QT_VERSION < 0x040300
    okButton->setText( tr("OK") );
    cancelButton->setText( tr("Cancel") );
    applyButton->setText( tr("Apply") );
#endif

}

void FilePropertiesDialog::accept()
{
    qDebug("FilePropertiesDialog::accept");

    hide();
    setResult( QDialog::Accepted );
    emit applied();
}

void FilePropertiesDialog::apply()
{
    qDebug("FilePropertiesDialog::apply");

    setResult( QDialog::Accepted );
    emit applied();
}

void FilePropertiesDialog::setCodecs(InfoList vc, InfoList ac, InfoList demuxer)
{
    vclist = vc;
    aclist = ac;
    demuxerlist = demuxer;

    InfoList::iterator it;

    for ( it = vclist.begin(); it != vclist.end(); ++it )
    {
        vc_listbox->addItem( (*it).name() +" - "+ (*it).desc() );
    }

    for ( it = aclist.begin(); it != aclist.end(); ++it )
    {
        ac_listbox->addItem( (*it).name() +" - "+ (*it).desc() );
    }

    for ( it = demuxerlist.begin(); it != demuxerlist.end(); ++it )
    {
        demuxer_listbox->addItem( (*it).name() +" - "+ (*it).desc() );
    }

    codecs_set = TRUE;
}

void FilePropertiesDialog::setDemuxer(QString demuxer, QString original_demuxer)
{
    qDebug("FilePropertiesDialog::setDemuxer");
    if (!original_demuxer.isEmpty()) orig_demuxer = original_demuxer;
    int pos = find(demuxer, demuxerlist );
    if (pos != -1) demuxer_listbox->setCurrentRow(pos);

    qDebug(" * demuxer: '%s', pos: %d", demuxer.toUtf8().data(), pos );
}

QString FilePropertiesDialog::demuxer()
{
    int pos = demuxer_listbox->currentRow();
    if ( pos < 0 )
        return "";
    else
        return demuxerlist[pos].name();
}

void FilePropertiesDialog::setVideoCodec(QString vc, QString original_vc)
{
    qDebug("FilePropertiesDialog::setVideoCodec");
    if (!original_vc.isEmpty()) orig_vc = original_vc;
    int pos = find(vc, vclist );
    if (pos != -1) vc_listbox->setCurrentRow(pos);

    qDebug(" * vc: '%s', pos: %d", vc.toUtf8().data(), pos );
}

QString FilePropertiesDialog::videoCodec()
{
    int pos = vc_listbox->currentRow();
    if ( pos < 0 )
        return "";
    else
        return vclist[pos].name();
}

void FilePropertiesDialog::setAudioCodec(QString ac, QString original_ac)
{
    qDebug("FilePropertiesDialog::setAudioCodec");
    if (!original_ac.isEmpty()) orig_ac = original_ac;
    int pos = find(ac, aclist );
    if (pos != -1) ac_listbox->setCurrentRow(pos);

    qDebug(" * ac: '%s', pos: %d", ac.toUtf8().data(), pos );
}

QString FilePropertiesDialog::audioCodec()
{
    int pos = ac_listbox->currentRow();
    if ( pos < 0 )
        return "";
    else
        return aclist[pos].name();
}

void FilePropertiesDialog::on_resetDemuxerButton_clicked()
{
    setDemuxer( orig_demuxer );
}

void FilePropertiesDialog::on_resetACButton_clicked()
{
    setAudioCodec( orig_ac );
}

void FilePropertiesDialog::on_resetVCButton_clicked()
{
    setVideoCodec( orig_vc );
}

int FilePropertiesDialog::find(QString s, InfoList &list)
{
    qDebug("FilePropertiesDialog::find");

    int n=0;
    InfoList::iterator it;

    for ( it = list.begin(); it != list.end(); ++it )
    {
        //qDebug(" * item: '%s', s: '%s'", (*it).name().toUtf8().data(), s.toUtf8().data());
        if ((*it).name() == s) return n;
        n++;
    }
    return -1;
}

void FilePropertiesDialog::setMplayerAdditionalArguments(QString args)
{
    mplayer_args_edit->setText(args);
}

QString FilePropertiesDialog::mplayerAdditionalArguments()
{
    return mplayer_args_edit->text();
}

void FilePropertiesDialog::setMplayerAdditionalVideoFilters(QString s)
{
    mplayer_vfilters_edit->setText(s);
}

QString FilePropertiesDialog::mplayerAdditionalVideoFilters()
{
    return mplayer_vfilters_edit->text();
}

void FilePropertiesDialog::setMplayerAdditionalAudioFilters(QString s)
{
    mplayer_afilters_edit->setText(s);
}

QString FilePropertiesDialog::mplayerAdditionalAudioFilters()
{
    return mplayer_afilters_edit->text();
}

// Language change stuff
void FilePropertiesDialog::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        retranslateStrings();
    }
    else
    {
        QDialog::changeEvent(e);
    }
}

#include "moc_filepropertiesdialog.cpp"
