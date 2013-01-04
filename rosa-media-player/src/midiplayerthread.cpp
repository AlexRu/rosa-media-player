/*  ROSA Media Player
    Julia Mineeva, Evgeniy Augin, Sergey Borovkov. Copyright (c) 2011 ROSA  <support@rosalab.ru>

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

#include "midiplayerthread.h"

#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QtDebug>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>

#include "mediadata.h"

#include <alsa/asoundlib.h>

/*
 ==============================
 Audio Output Functions
 ------------------------------
 ==============================
*/

unsigned int rate = WILDMIDI_RATE;


char  *pcmname = NULL;

int (*send_output) (char * output_data, int output_size);
void (*close_output) ( void );


int alsa_first_time = 1;
static snd_pcm_t  *pcm;

static int write_alsa_output (char * output_data, int output_size);
static void close_alsa_output ( void );

static int open_alsa_output()
{
    snd_pcm_hw_params_t     *hw;
    snd_pcm_sw_params_t     *sw;
    int err;
    unsigned int alsa_buffer_time;
    unsigned int alsa_period_time;

    qDebug() << "open_alsa_output():   pcmname = " << pcmname;
    if (!pcmname) {
        pcmname = (char*) malloc( 8 );
        strcpy(pcmname,"default\0");
    }

    if ((err = snd_pcm_open (&pcm, pcmname, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        printf("Error: audio open error: %s\r\n", snd_strerror (-err));
        qDebug() << "Error: audio open error: " << QString::fromLocal8Bit(snd_strerror (-err)) << ",   err = " << err;
        return -1;
    }

    snd_pcm_hw_params_alloca (&hw);

    if ((err = snd_pcm_hw_params_any(pcm, hw)) < 0) {
        printf("ERROR: No configuration available for playback: %s\r\n", snd_strerror(-err));

        return -1;
    }

    if ((err = snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        printf("Cannot set mmap'ed mode: %s.\r\n", snd_strerror(-err));
        return -1;
    }

    if (snd_pcm_hw_params_set_format (pcm, hw, SND_PCM_FORMAT_S16_LE) < 0) {
        printf("ALSA does not support 16bit signed audio for your soundcard\r\n");
        close_alsa_output();
        return -1;
    }

    if (snd_pcm_hw_params_set_channels (pcm, hw, 2) < 0) {
        printf("ALSA does not support stereo for your soundcard\r\n");
        close_alsa_output();
        return -1;
    }

    if (snd_pcm_hw_params_set_rate_near(pcm, hw, &rate, 0) < 0) {
        printf("ALSA does not support %iHz for your soundcard\r\n",rate);
        close_alsa_output();
        return -1;
    }

    alsa_buffer_time = 500000;
    alsa_period_time = 50000;

    if ((err = snd_pcm_hw_params_set_buffer_time_near(pcm, hw, &alsa_buffer_time, 0)) < 0)
    {
        printf("Set buffer time failed: %s.\r\n", snd_strerror(-err));
        return -1;
    }

    if ((err = snd_pcm_hw_params_set_period_time_near(pcm, hw, &alsa_period_time, 0)) < 0)
    {
        printf("Set period time failed: %s.\r\n", snd_strerror(-err));
        return -1;
    }

    if (snd_pcm_hw_params(pcm, hw) < 0)
    {
        printf("Unable to install hw params\r\n");
        return -1;
    }

    snd_pcm_sw_params_alloca(&sw);
    snd_pcm_sw_params_current(pcm, sw);
    if (snd_pcm_sw_params(pcm, sw) < 0)
    {
        printf("Unable to install sw params\r\n");
        return -1;
    }

    send_output = write_alsa_output;
    close_output = close_alsa_output;
    if (pcmname != NULL) {
        free (pcmname);
        pcmname = NULL;
    }
    return 0;
}

static int
write_alsa_output (char * output_data, int output_size) {
    int err;
    snd_pcm_uframes_t frames;

    while (output_size > 0) {
        frames = snd_pcm_bytes_to_frames(pcm, output_size);
        if ((err = snd_pcm_writei(pcm, output_data, frames)) < 0) {
            if (snd_pcm_state(pcm) == SND_PCM_STATE_XRUN) {
                if ((err = snd_pcm_prepare(pcm)) < 0)
                    printf("snd_pcm_prepare() failed.\r\n");
                alsa_first_time = 1;
                continue;
            }
            return err;
        }

        output_size -= snd_pcm_frames_to_bytes(pcm, err);
        output_data += snd_pcm_frames_to_bytes(pcm, err);
        if (alsa_first_time) {
            alsa_first_time = 0;
            snd_pcm_start(pcm);
        }
    }
    return 0;
}

static void
close_alsa_output ( void ) {
    int rc = snd_pcm_close (pcm);
    qDebug() << "close_alsa_output():    rc = " << rc;
    pcm = 0;
}


MidiPlayerThread::MidiPlayerThread( MediaData *md, QObject *parent )
    : QThread( parent )
    , m_md( md )
{
    m_midiPtr = 0;
    m_totalTime = 0;

    m_configFile = WILDMIDI_CFG;
    m_inPause = false;
    m_isLoop = false;
    m_isStop = false;
    m_isFinished = false;

    m_sample_rate = rate;
    m_received_end_of_file = false;

    connect( this, SIGNAL( finished() ), SLOT( processFinished() ) );
}

MidiPlayerThread::~MidiPlayerThread()
{
}

void MidiPlayerThread::run()
{
    qDebug() << "MidiPlayerThread::run()";
   // here start play
    play();
}

void MidiPlayerThread::play()
{
    qDebug() << "MidiPlayerThread::play";

    const unsigned long int buf_size = 4096;

    int output_result = 0;
    unsigned long int count_diff;
    unsigned long int cur_sec, prev_sec;

    m_inPause = false;
    m_received_end_of_file = false;
    cur_sec = 0;

    QByteArray buf;
    buf.resize( buf_size );

    _WM_Info *wm_info = WildMidi_GetInfo( m_midiPtr );

    m_md->list();
    emit receivedStartingTime( 0 );
    emit backendFullyLoaded();
    emit receivedNoVideo();

    qDebug() << "approx_total_samples is " << wm_info->approx_total_samples;
    while ( 1 ) {
        if ( m_isStop ) {
            qDebug() << "MidiPlayerThread::play:   m_isStop = " << m_isStop;
            break;
        }
        count_diff = wm_info->approx_total_samples - wm_info->current_sample;

        if ( count_diff == 0 ) {
            qDebug() << "MidiPlayerThread::play:   it is count_diff == 0";
            break;
        }

        if ( m_inPause ) {
            usleep( 1000 );
            continue;
        }

        buf.fill( 0 );

        if ( count_diff < buf_size ) {
            buf.resize( count_diff * 4 );
            output_result = WildMidi_GetOutput ( m_midiPtr, buf.data(), ( count_diff * 4 ) );
        } else {
            output_result = WildMidi_GetOutput ( m_midiPtr, buf.data(), buf_size );
        }

        if ( output_result <= 0 ) {
            qDebug() << "MidiPlayerThread::play:   output_result <= 0";
            break;
        }
        prev_sec =  wm_info->current_sample / m_sample_rate;
        if ( prev_sec != cur_sec ) {
            cur_sec = prev_sec;
            emit receivedCurrentSec( cur_sec );
//            qDebug() << "cur_sec is " << cur_sec;
        }

        send_output( buf.data(), output_result );

        wm_info = WildMidi_GetInfo( m_midiPtr );
    }

//    buf.fill( 0, 16384 );
//    send_output( buf.data(), 16384);

//    buf.fill( 0, 16384 );
//    send_output( buf.data(), 16384);
//    send_output( buf.data(), 16384);

//    usleep( 5000 );

    if ( m_midiPtr ) {
        // file was played
        if ( WildMidi_Close( m_midiPtr ) == ( -1 ) ) {
            qDebug() << "oops!";
        }
        m_midiPtr = 0;
    }
    WildMidi_Shutdown();

    close_output();

//    emit processExited();
    m_received_end_of_file = true;

    qDebug() << "MidiPlayerThread::play:   end of play";
}

void MidiPlayerThread::pause()
{
    qDebug() << "MidiPlayerThread:   pause on...";
    m_inPause = !m_inPause;
    if ( m_inPause ) {
        emit receivedPause();
    }
}

void MidiPlayerThread::stop()
{
    qDebug() << "MidiPlayerThread::stop():   shutting down Sound System...";
    m_isStop = true;
}

void MidiPlayerThread::frameStep()
{
//    if ( !m_midiPtr ) {
//        qDebug( "MidiPlayerThread::frameStep:   WildMidi library is not initialized" );
//        return;
//    }

//    m_inPause = false;

//    int samples = 100;
//    usleep( 5000 );

//    _WM_Info *wm_info = WildMidi_GetInfo( m_midiPtr );
//    qDebug() << "MidiPlayerThread::frameStep:   current sample = " << wm_info->current_sample << ",   total samples = " << wm_info->approx_total_samples;

//    ulong sample = wm_info->current_sample + samples;

//    qDebug() << "MidiPlayerThread::frameStep:   sample = " << sample;
//    WildMidi_FastSeek( m_midiPtr, &sample );


//    m_inPause = true;
//    emit receivedPause();
}

bool MidiPlayerThread::isRunning()
{
    return QThread::isRunning();
}

bool MidiPlayerThread::isFinished()
{
    return m_isFinished;
}

void MidiPlayerThread::goToSec( double sec )
{
    if ( !m_midiPtr ) {
        qDebug( "MidiPlayerThread::goToSec:   WildMidi library is not initialized" );
        return;
    }

    _WM_Info *wm_info = WildMidi_GetInfo( m_midiPtr );
    qDebug() << "MidiPlayerThread::goToSec:   current sample = " << wm_info->current_sample << ",   total samples = " << wm_info->approx_total_samples;

    ulong sample = ( ulong )m_sample_rate * sec;
    qDebug() << "MidiPlayerThread::goToSec:   sec = " << sec << ",   sample = " << sample;
    WildMidi_FastSeek( m_midiPtr, &sample );
}

void MidiPlayerThread::seek( int secs )
{
    if ( !m_midiPtr ) {
        qDebug( "MidiPlayerThread::seek:   WildMidi library is not initialized" );
        return;
    }

    _WM_Info *wm_info = WildMidi_GetInfo( m_midiPtr );
    qDebug() << "MidiPlayerThread::seek:   current sample = " << wm_info->current_sample << ",   total samples = " << wm_info->approx_total_samples;

    ulong sample = ( ulong )m_sample_rate * secs + wm_info->current_sample;
    qDebug() << "MidiPlayerThread::seek:   secs = " << secs << ",   sample = " << sample;
    WildMidi_FastSeek( m_midiPtr, &sample );
}

void MidiPlayerThread::setVolume( int volume )
{
    // volume may be from 0 to 127.... (in mplayer to 100)
    qDebug() << "MidiPlayerThread::setVolume:   volume = " << volume;
    WildMidi_MasterVolume( volume );
}

void MidiPlayerThread::processFinished()
{
    qDebug() << "MidiPlayerThread::processFinished";

    m_isFinished = true;
    // Send this signal before the endoffile one, otherwise
    // the playlist will start to play next file before all
    // objects are notified that the process has exited.
    emit processExited();
    if ( m_received_end_of_file ) {
        emit receivedEndOfFile();
    }
}

bool MidiPlayerThread::start( const QString &path, int volume, double seek /* = -1 */ )
{
    qDebug() << "MidiPlayerThread::start";
    unsigned short int mixer_options = 0;
    m_path = path;

    if ( open_alsa_output() == ( -1 ) ) {
        qDebug() << "MidiPlayerThread::start:   error of open_alsa_output()...";
        return false;
    }

    if ( WildMidi_Init( qPrintable( m_configFile ), m_sample_rate, mixer_options ) < 0 )  {
        qWarning( "MidiPlayerThread::start:   unable to initialize WildMidi library" );
        close_output();
        return false;
    }

    m_midiPtr = WildMidi_Open ( qPrintable( m_path ) );
    if ( !m_midiPtr ) {
        qWarning("MidiPlayerThread::start:   unable to open file");
        WildMidi_Shutdown();
        close_output();
        return false;
    }

    Tracks audios;
    audios.addID( 0 );

    _WM_Info *wm_info = WildMidi_GetInfo( m_midiPtr );
    m_totalTime = ( qint64 )wm_info->approx_total_samples / m_sample_rate;
    qDebug() << "total time is " << m_totalTime << " sec";

    m_md->reset();
    m_md->duration = m_totalTime;
    m_md->audio_rate = m_sample_rate;
    m_md->novideo = true;
    m_md->audios = audios;
//    m_md->type = TYPE_FILE;

    setVolume( volume );

    if ( seek != ( -1 ) ) {
        goToSec( seek );
    }

    QThread::start();

    qDebug( "MidiPlayerThread: thread started" );

    emit audioInfoChanged( audios );

    return true;
}
