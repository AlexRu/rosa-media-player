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


#include "infoprovider.h"
#include "global.h"
#include "preferences.h"
#include "mplayerprocess.h"
#include "ffmpegprocess.h"
#ifdef MIDI_SUPPORT
#include "wildmidibackend.h"
#endif

#include <QFileInfo>
#include <QDebug>

MediaData InfoProvider::getInfo( BackendFactory::BackendType backend_type, QString filename )
{
    BackendFactory f;
    MediaData md;

#ifdef MIDI_SUPPORT
    if ( backend_type == BackendFactory::WildMidi ) {
        WildMidiBackend *midi_proc = dynamic_cast<WildMidiBackend*>( f.createBackend( backend_type ) );
        if ( midi_proc ) {
            md = midi_proc->getInfo( filename );
        }
        delete midi_proc;
    }
    else {
#else
    if ( backend_type == BackendFactory::Mplayer ) {
#endif
        MplayerProcess *mplayer_proc = dynamic_cast<MplayerProcess*>( f.createBackend( backend_type ) );

        if ( mplayer_proc ) {

            QString mplayer_bin = Global::pref->mplayer_bin;

            QFileInfo fi( mplayer_bin );
            if ( fi.exists() && fi.isExecutable() && !fi.isDir() ) {
                mplayer_bin = fi.absoluteFilePath();
            }

            mplayer_proc->addArgument( mplayer_bin );
            mplayer_proc->addArgument( "-identify" );
            mplayer_proc->addArgument( "-frames" );
            mplayer_proc->addArgument( "0" );
            mplayer_proc->addArgument( "-vo" );
            mplayer_proc->addArgument( "null" );
            mplayer_proc->addArgument( "-ao" );
            mplayer_proc->addArgument( "null" );
            mplayer_proc->addArgument( filename );

            mplayer_proc->start();
            if ( !mplayer_proc->waitForFinished() ) {
                qWarning("InfoProvider::getInfo: process didn't finish. Killing it...");
                mplayer_proc->kill();
            }

            md = mplayer_proc->mediaData();
            delete mplayer_proc;

            // get metadata info with ffmpeg if has no info with mplayer
//            if ( md.displayName().isEmpty() ) {
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
                    ffmpeg_proc->addArgument( filename );

                    ffmpeg_proc->start();
                    if ( !ffmpeg_proc->waitForFinished() ) {
                        qWarning("InfoProvider::getInfo: process didn't finish. Killing it...");
                        ffmpeg_proc->kill();
                    }

                    ffmpeg_md = ffmpeg_proc->mediaData();
                    delete ffmpeg_proc;

                    if ( !ffmpeg_md.clip_name.isEmpty() ) {
                        md.clip_name = ffmpeg_md.clip_name;
                    }
                    if ( !ffmpeg_md.clip_track.isEmpty() ) {
                        md.clip_track = ffmpeg_md.clip_track;
                    }
                    if ( !ffmpeg_md.clip_album.isEmpty() ) {
                        md.clip_album = ffmpeg_md.clip_album;
                    }
                    if ( !ffmpeg_md.clip_artist.isEmpty() ) {
                        md.clip_artist = ffmpeg_md.clip_artist;
                    }
                    if ( !ffmpeg_md.clip_date.isEmpty() ) {
                        md.clip_date = ffmpeg_md.clip_date;
                    }
                    if ( !ffmpeg_md.clip_copyright.isEmpty() ) {
                        md.clip_copyright = ffmpeg_md.clip_copyright;
                    }
                    if ( !ffmpeg_md.clip_software.isEmpty() ) {
                        md.clip_software = ffmpeg_md.clip_software;
                    }
                    if ( !ffmpeg_md.clip_comment.isEmpty() ) {
                        md.clip_comment = ffmpeg_md.clip_comment;
                    }
                }
            }
        }
    }

    return md;
}

MediaData InfoProvider::getInfo( const QString &filename )
{
    QFileInfo fi( filename );

#ifdef MIDI_SUPPORT
    bool isMidi = ( fi.exists() && ( ( fi.suffix().toLower() == "mid" ) || ( fi.suffix().toLower() == "midi" ) ) );

    BackendFactory::BackendType backend_type = isMidi ? BackendFactory::WildMidi : BackendFactory::Mplayer;
#else
    BackendFactory::BackendType backend_type = BackendFactory::Mplayer;
#endif

    return getInfo( backend_type, filename );
}
