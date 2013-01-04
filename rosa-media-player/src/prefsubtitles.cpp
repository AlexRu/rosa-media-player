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

#include "prefsubtitles.h"
#include "images.h"
#include "preferences.h"
#include "paths.h"
#include "config.h"
#include "languages.h"
#include "recents.h"

#include <QDir>
#include <QStyleFactory>
#include <QFontDialog>

PrefSubtitles::PrefSubtitles(QWidget * parent, Qt::WindowFlags f)
    : PrefWidget(parent, f )
{
    setupUi(this);

    retranslateStrings();
}

PrefSubtitles::~PrefSubtitles()
{
}

QString PrefSubtitles::sectionName()
{
    return tr("Subtitles");
}

QPixmap PrefSubtitles::sectionIcon()
{
    return Images::pixmap("pref_subtitles" );
}

void PrefSubtitles::retranslateStrings()
{
    int font_autoload_item = font_autoload_combo->currentIndex();

    retranslateUi(this);

    font_autoload_combo->setCurrentIndex(font_autoload_item);

    // Encodings combo
    //int font_encoding_item = font_encoding_combo->currentIndex();
    QString current_encoding = fontEncoding();
    font_encoding_combo->clear();

    QMap<QString,QString> l = Languages::encodings();
    QMapIterator<QString, QString> i(l);
    while (i.hasNext())
    {
        i.next();
        font_encoding_combo->addItem( i.value() + " (" + i.key() + ")", i.key() );
    }
    font_encoding_combo->model()->sort(0);
    //font_encoding_combo->setCurrentIndex(font_encoding_item);
    setFontEncoding(current_encoding);

    createHelp();
}

void PrefSubtitles::setData(Preferences * pref)
{
    setAudioLang( pref->audio_lang );
    setSubtitleLang( pref->subtitle_lang );
    setAudioTrack( pref->initial_audio_track );
    setSubtitleTrack( pref->initial_subtitle_track );

    setFontEncoding( pref->subcp );
}

void PrefSubtitles::getData(Preferences * pref)
{
    pref->audio_lang = audioLang();
    pref->subtitle_lang = subtitleLang();

    pref->initial_audio_track = audioTrack();
    pref->initial_subtitle_track = subtitleTrack();

    TEST_AND_SET( pref->subcp, fontEncoding());
}

void PrefSubtitles::setFontEncoding(QString s)
{
    int i = font_encoding_combo->findData(s);
    font_encoding_combo->setCurrentIndex(i);
}

QString PrefSubtitles::fontEncoding()
{
    int index = font_encoding_combo->currentIndex();
    return font_encoding_combo->itemData(index).toString();
}

void PrefSubtitles::setFontFuzziness(int n)
{
    font_autoload_combo->setCurrentIndex(n);
}

int PrefSubtitles::fontFuzziness()
{
    return font_autoload_combo->currentIndex();
}

void PrefSubtitles::setAudioLang(QString lang)
{
    audio_lang_edit->setText(lang);
}

QString PrefSubtitles::audioLang()
{
    return audio_lang_edit->text();
}

void PrefSubtitles::setSubtitleLang( QString lang )
{
    subtitle_lang_edit->setText( lang );
}

QString PrefSubtitles::subtitleLang()
{
    return subtitle_lang_edit->text();
}

void PrefSubtitles::setAudioTrack( int track )
{
    audio_track_spin->setValue( track );
}

int PrefSubtitles::audioTrack()
{
    return audio_track_spin->value();
}

void PrefSubtitles::setSubtitleTrack(int track)
{
    subtitle_track_spin->setValue( track );
}

int PrefSubtitles::subtitleTrack()
{
    return subtitle_track_spin->value();
}

void PrefSubtitles::createHelp()
{
    clearHelp();

    addSectionTitle(tr("Preferred audio and subtitles"));

    setWhatsThis( audio_lang_edit, tr("Preferred audio language"),
                  tr("Here you can type your preferred language for the audio streams. "
                     "When a media with multiple audio streams is found, ROSA Media Player will "
                     "try to use your preferred language.<br>"
                     "This only will work with media that offer info about the language "
                     "of the audio streams, like DVDs or mkv files.<br>"
                     "This field accepts regular expressions. Example: <b>es|esp|spa</b> "
                     "will select the audio track if it matches with <i>es</i>, "
                     "<i>esp</i> or <i>spa</i>.") );

    setWhatsThis( subtitle_lang_edit, tr("Preferred subtitle language"),
                  tr("Here you can type your preferred language for the subtitle stream. "
                     "When a media with multiple subtitle streams is found, ROSA Media Player will "
                     "try to use your preferred language.<br>"
                     "This only will work with media that offer info about the language "
                     "of the subtitle streams, like DVDs or mkv files.<br>"
                     "This field accepts regular expressions. Example: <b>es|esp|spa</b> "
                     "will select the subtitle stream if it matches with <i>es</i>, "
                     "<i>esp</i> or <i>spa</i>.") );

    setWhatsThis( audio_track_spin, tr("Audio track"),
                  tr("Specifies the default audio track which will be used when playing "
                     "new files. If the track doesn't exist, the first one will be used. "
                     "<br><b>Note:</b> the <i>\"preferred audio language\"</i> has "
                     "preference over this option.") );

    setWhatsThis( subtitle_track_spin, tr("Subtitle track"),
                  tr("Specifies the default subtitle track which will be used when "
                     "playing new files. If the track doesn't exist, the first one "
                     "will be used. <br><b>Note:</b> the <i>\"preferred subtitle "
                     "language\"</i> has preference over this option.") );

    addSectionTitle(tr("Autoload"));

    setWhatsThis(font_autoload_combo, tr("Autoload"),
                 tr("Select the subtitle autoload method.") );

    addSectionTitle(tr("Encoding"));

    setWhatsThis(font_encoding_combo, tr("Default subtitle encoding"),
                 tr("Select the encoding which will be used for subtitle files "
                    "by default.") );
}

#include "moc_prefsubtitles.cpp"
