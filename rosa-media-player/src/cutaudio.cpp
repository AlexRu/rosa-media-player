/*  ROSA Media Player
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

#include "cutaudio.h"

#include "ffmpegprocess.h"
#include "preferences.h"
#include "global.h"
#include "core.h"

#ifdef WIN32
#include <windows.h>
#else // linux stuff
#include <sys/vfs.h>
#include <sys/stat.h>
#endif // _WIN32

#include <QFileInfo>
#include <QLabel>
#include <QToolButton>
#include <QTime>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>

#include "ui_cutaudio.h"

using namespace Global;

#define DEFAULT_BITRATE     320

CutAudio::CutAudio( Core* core, QWidget* parent )
    : QWidget( parent )
    , m_bitrate( DEFAULT_BITRATE )
    , m_error( -1 )
    , m_isStopFfmpeg( true )
    , m_isNewOpenFile( true )
    , m_isCancel( false )
    , m_ffmpegProc( 0 )
    , m_core( core )
    , ui( new Ui::CutAudioPanel )
{
    ui->setupUi( this );

    m_ffmpegProc = new FfmpegProcess( this );

    connect( m_ffmpegProc, SIGNAL( processExited() ), this, SLOT( processFinished() ), Qt::QueuedConnection );
    connect( m_ffmpegProc, SIGNAL( receivedCurSec( int ) ), this, SLOT( updateProgressBar( int ) ) );
    connect( m_ffmpegProc, SIGNAL( receivedError( int ) ), this, SLOT( processError( int ) ) );

    connect( m_core, SIGNAL( aboutToStartPlaying() ), this, SLOT( playNewFile() ) );
    connect( m_core, SIGNAL( showTime( double ) ), this, SLOT( gotCurrentTime( double ) ) );

    connect( ui->cbFormat, SIGNAL( currentIndexChanged( int ) ), SLOT( updateControls() ) );

    QStringList formats;
    formats << "mp3" << "ogg";
    ui->cbFormat->clear();
    ui->cbFormat->addItems( formats );

    updateWidget();
    updateControls();
}

CutAudio::~CutAudio()
{
    stopFfmpeg();

    m_ffmpegProc->terminate();
    delete m_ffmpegProc;
}

void CutAudio::updateWidget()
{
    qDebug("CutAudio::updateWidget");
    if ( pref->use_extract_audio )
    {
        ui->cbFormat->show();
        ui->pbCancel->show();
        ui->pbCut->show();
        ui->progressBar->show();
        ui->labelError->show();
        ui->label->setText( tr( "Output file format:" ) );
    }
    else
    {
        ui->cbFormat->hide();
        ui->pbCancel->hide();
        ui->pbCut->hide();
        ui->progressBar->hide();
        ui->labelError->hide();
        ui->label->setText( tr( "The version of ffmpeg installed on your computer "
                                "does not support the requested codec libmp3lame. "
                                "To be able to extract audio streams please install "
                                "ffmpeg with support for libmp3lame.") );
    }
}

void CutAudio::playNewFile()
{
    qDebug("CutAudio::playNewFile()");
    m_isNewOpenFile = true;
}

void CutAudio::gotCurrentTime( double )
{
    if ( m_isNewOpenFile && m_core->mdat.duration )
    {
        m_isNewOpenFile = false;
        updateControls();
    }
}

void CutAudio::updateControls()
{
    bool canCut = true;

    // make checking for url
    if ( m_core->mdat.type != TYPE_FILE )
        canCut = false;

    if ( m_ffmpegProc->isRunning() )
    {
        canCut = false;
    }

    if ( m_core->mdat.filename.isEmpty() )
    {
        canCut = false;
    }

    ui->pbCancel->setEnabled( m_ffmpegProc->isRunning() );
    ui->pbCancel->setEnabled( !m_isStopFfmpeg );

    if ( canCut )
        ui->pbCut->setEnabled( !m_ffmpegProc->isRunning() );
    else
        ui->pbCut->setEnabled( canCut );

//    ui->cbFormat->setEnabled( !ui->pbCancel->isEnabled() );

    ui->progressBar->setValue( 0 );
    ui->labelError->setText( "" );
}


/*
The eventual file size of an audio recording can also be calculated using a similar formula:
    File Size (Bytes) = (sampling rate) x (bit depth) x (number of channels) x (seconds) / 8
E.g., a 70 minutes long CD quality recording will take up 740880000 Bytes, or 740MB:
    44100 x 16 x 2 x 4200 / 8 = 740880000 Bytes
*/
bool CutAudio::checkDiskSpace( const QString& fname, double duration  )
{
    bool isNew = false;
    if ( !QFile::exists( fname ) )
    {
        QFile f( fname );
        f.open( QIODevice::WriteOnly );
        f.close();
        isNew = true;
    }

    QFileInfo fi( fname );

    qint64 szKb = 0;

    if ( ui->cbFormat->currentText() == "mp3" )
    {
        szKb = m_bitrate * duration * 1024 / 8;
    }
    else // it's ogg ( approximate maximum )
    {
        float k_ogg = 0.82; // koefficient for ogg files (max for avi)
        szKb = m_bitrate * duration * 1024 * k_ogg / 8;
    }

    qDebug( "CutAudio::checkDiskSpace:   duration is %f sec, szKb is %lld Kb", duration,  szKb );

    double fTotal, fFree;
    bool rc = getFreeTotalSpaceKb( fi.filePath(), fTotal, fFree );
    qDebug( "CutAudio::checkDiskSpace:   getFreeTotalSpace return %d (total = %f, free = %f)", rc, fTotal, fFree );

    if ( isNew )
        QFile::remove( fname );

    if ( !rc ) return false;

    return ( ( szKb / 1024 ) <= fFree);
}

void CutAudio::startCut()
{
    m_isCancel = false;
    startFfmpeg();
}

void CutAudio::stopFfmpeg()
{
    m_isStopFfmpeg = true;
    m_isCancel = true;

    qDebug("CutAudio::stopCut:   stop ffmpeg...");
    if ( !m_ffmpegProc->isRunning() )
    {
        qDebug("Core::stopCut: ffmpeg is not running!");
    }
    else
    {
        // send "quit" command for ffmpeg
        tellffmpeg( "q" );

        qDebug( "CutAudio::stopCut:   waiting ffmpeg to finish..." );
        if ( !m_ffmpegProc->waitForFinished( 5000 ) )
        {
            qWarning( "Core::stopCut:   ffmpeg process didn't finish. Killing it..." );
            m_ffmpegProc->terminate();
            m_ffmpegProc->kill();
        }
    }
}

void CutAudio::stopCut()
{
    qDebug("CutAudio::stopCut");

    stopFfmpeg();

    updateControls();

    qDebug( "CutAudio::stopCut:   finished. (I hope)" );
}

void CutAudio::startFfmpeg()
{
    QFileInfo fi;

    QString inputFile = m_core->mdat.filename;
    QString outputFile;

    fi.setFile( m_core->mdat.filename );
    outputFile = fi.absolutePath() + "/" + fi.baseName() + "." + ui->cbFormat->currentText();
    outputFile = getNewFileName( outputFile );

    bool isFlv = false;
    isFlv = ( fi.suffix() == "flv" );
    qDebug( "CutAudio::startFfmpeg():   isFlv = %d", isFlv );

    if ( !canWriteTo( outputFile ) )
    {
        qDebug("CutAudio::startFfmpeg():   cannot extract audio track ( maybe your disk is mounted read-only? )");
        outputFile = QDesktopServices::storageLocation( QDesktopServices::MusicLocation ) + "/" +
                fi.baseName() + "." + ui->cbFormat->currentText();
        outputFile = getNewFileName( outputFile );
    }

    qDebug("CutAudio::startFfmpeg():   outputFile is %s", outputFile.toLocal8Bit().data() );

    if ( !checkDiskSpace( outputFile, m_core->mdat.duration ) )
    {
        ui->labelError->setText( tr( "Cannot extract audio track ( maybe you have no enough disk space? )" ) );
        return;
    }

    m_isStopFfmpeg = false;
    m_error = -1;
    m_inputFile = inputFile;
    m_outputFile = outputFile;

    // Use absolute path, otherwise after changing to the screenshot directory
    // the mencoder path might not be found if it's a relative path
    // (seems to be necessary only for linux)
    QString ffmpeg_bin = pref->ffmpeg_bin;
    fi.setFile( ffmpeg_bin );
    if ( fi.exists() && fi.isExecutable() && !fi.isDir() )
    {
        ffmpeg_bin = fi.absoluteFilePath();
    }
/*
  input example:
  ffmpeg -i input.avi -vn -ab 320k output.mp3
  ffmpeg -i input.avi -vn -sameq -f ogg -acodec libvorbis -ab 320k output.ogg
 */
    m_ffmpegProc->clearArguments();
    m_ffmpegProc->addArgument( ffmpeg_bin );
    m_ffmpegProc->addArgument( "-i" );
    m_ffmpegProc->addArgument( m_inputFile );
    m_ffmpegProc->addArgument( "-vn" );

    if ( ui->cbFormat->currentText() != "mp3" )
    {
        m_ffmpegProc->addArgument( "-sameq" );
        m_ffmpegProc->addArgument( "-f" );
        m_ffmpegProc->addArgument( "ogg" );
        m_ffmpegProc->addArgument( "-acodec" );
        m_ffmpegProc->addArgument( "libvorbis" );
        if ( !isFlv )
        {
//            m_ffmpegProc->addArgument( "-ab" );
//            m_ffmpegProc->addArgument( QString( "%1k" ).arg( m_bitrate ) );
        }
    }
    else
    {
        m_ffmpegProc->addArgument( "-ab" );
        m_ffmpegProc->addArgument( QString( "%1k" ).arg( m_bitrate ) );
    }
    m_ffmpegProc->addArgument( m_outputFile );

    QString commandline = m_ffmpegProc->arguments().join(" ");
    qDebug("CutAudio::startFfmpeg: command: '%s'", commandline.toUtf8().data());

    if ( !m_ffmpegProc->start() ) {
        // error handling
        qWarning("CutAudio::startFfmpeg: ffmpeg process didn't start");
    }

    updateControls();
}

void CutAudio::processFinished()
{
    qDebug("CutAudio::processFinished");

    m_isStopFfmpeg = true;
    updateControls();

    int exit_code = m_ffmpegProc->exitCode();
    qDebug("CutAudio::processFinished: exit_code: %d", exit_code);
    if ( exit_code != 0 )
    {
        ui->labelError->setText( tr( "Cannot extract audio track (code: %1)" ).arg( exit_code ) );
    }
    else
    {
        if ( m_error == (-1) )
        {
            if ( !m_isCancel )
            {
                QFileInfo fi( m_outputFile );

                QMessageBox mb( this );
                mb.setWindowTitle( tr( "Information" ) );
                mb.setText( tr( "New file \"%1\" saved in the folder %2" ).arg( fi.fileName() ).arg( fi.absolutePath() ) );
                mb.addButton( tr( "OK" ), QMessageBox::AcceptRole );
                foreach ( QAbstractButton* button, mb.buttons() )
                    button->setIcon( QIcon() );
                mb.exec();
                return;
            }
        }
        else
        {
            qDebug("CutAudio::processFinished:   error is %d", m_error );
        }
    }
    if ( QFile::exists( m_outputFile ) )
        QFile::remove( m_outputFile );
}

void CutAudio::updateProgressBar( int sec )
{
    if ( !m_isStopFfmpeg )
    {
        int pos = ( sec * 100 ) / ( int )( m_core->mdat.duration  );
        //        qDebug("CutAudio::updateProgressBar:   %d  ", pos);
        ui->progressBar->setValue( pos );
    }
    else
    {
        ui->progressBar->setValue( 0 );
    }
}

void CutAudio::processError( int err )
{
    qDebug( "CutAudio::processError:   error = %d  ", err );
    m_error = err;
}

void CutAudio::tellffmpeg( const QString& command )
{
    qDebug("CutAudio::tellffmpeg: '%s'", command.toUtf8().data());

    //qDebug("Command: '%s'", command.toUtf8().data());
    if ( m_ffmpegProc->isRunning() )
    {
        m_ffmpegProc->writeToStdin( command );
    }
    else
    {
        qWarning(" tellffmpeg: no process running: %s", command.toUtf8().data());
    }
}
