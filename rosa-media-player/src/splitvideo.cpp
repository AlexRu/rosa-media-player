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

#include "splitvideo.h"

#include "splitvideoprocess.h"
#include "preferences.h"
#include "global.h"
#include "core.h"

#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimeEdit>
#include <QLabel>
#include <QToolButton>
#include <QTime>
#include <QMessageBox>
#include <QDesktopServices>

#include "ui_splitvideo.h"

using namespace Global;

SplitVideo::SplitVideo( Core* core, QWidget* parent )
    : QWidget( parent )
    , m_isNewOpenFile( true )
    , m_startTime( 0 )
    , m_endTime( 0 )
    , m_error( -1 )
    , m_isStopMencoder( true )
    , m_isCutError( false )
    , m_inputFile( "" )
    , m_outputFile( "" )
    , m_proc( 0 )
    , m_core( core )
    , ui( new Ui::SplitVideoPanel )
{
    ui->setupUi(this);

    m_proc = new SplitVideoProcess( this );

    connect( m_proc, SIGNAL( processExited() ), this, SLOT( processFinished() ), Qt::QueuedConnection );
    connect( m_proc, SIGNAL( receivedSplitSec( double ) ), this, SLOT( updateProgressBar( double ) ) );
    connect( m_proc, SIGNAL( receivedCutError() ), this, SLOT( processCutError() ) );
    connect( m_proc, SIGNAL( receivedError( int ) ), this, SLOT( processError( int ) ) );
    connect( m_core, SIGNAL( aboutToStartPlaying() ), this, SLOT( playNewFile() ) );
    connect( m_core, SIGNAL( showTime( double ) ), this, SLOT( gotCurrentTime( double ) ) );

    updateControls();
}

SplitVideo::~SplitVideo()
{
    if ( m_proc->isRunning() )
        stopMencoder();

    m_proc->terminate();

    delete m_proc;
}

void SplitVideo::gotCurrentTime( double )
{
    if ( m_isNewOpenFile && m_core->mdat.duration )
    {
        m_isNewOpenFile = false;

        m_startTime = 0;
        m_endTime = m_core->mdat.duration;

        updateTime();

        updateControls();
    }
}

void SplitVideo::playNewFile()
{
    qDebug("SplitVideo::playNewFile()");
    m_isNewOpenFile = true;
}

bool SplitVideo::checkTime( int startTime, int endTime )
{
    ui->labelError->setText( "" );
    if ( startTime >= endTime )
        ui->labelError->setText( tr( "Start time must be before than the end time" ) );

    return ( ( startTime < endTime ) &&
             ( startTime < m_core->mdat.duration ) &&
             ( endTime <= m_core->mdat.duration ) );
}

void SplitVideo::setFromTime()
{
    m_startTime = m_core->mset.current_sec;
    qDebug( "SplitVideo::setFromTime:    m_startTime = %d", m_startTime );

    // correct time
    if ( m_startTime > m_endTime )
        m_endTime = m_startTime;

    updateTime();
}

void SplitVideo::setToTime()
{
    m_endTime = m_core->mset.current_sec;
    qDebug( "SplitVideo::setToTime:    m_endTime = %d", m_endTime );

    // correct time
    if ( m_startTime > m_endTime )
        m_startTime = m_endTime;

    updateTime();
}

void SplitVideo::fromTimeChanged( const QTime & )
{
    QTime t;
    m_startTime = t.secsTo( ui->teFrom->time() );

    qDebug( "SplitVideo::fromTimeChanged:    m_startTime = %d", m_startTime );

    // correct time
    if ( m_startTime > m_endTime )
        m_endTime = m_startTime;

    updateTime();
}

void SplitVideo::toTimeChanged( const QTime & )
{
    QTime t;
    m_endTime = t.secsTo( ui->teTo->time() );

    qDebug( "SplitVideo::toTimeChanged:    m_endTime = %d", m_endTime );

    // correct time
    if ( m_startTime > m_endTime )
        m_startTime = m_endTime;

    updateTime();
}

void SplitVideo::updateTime()
{
    qDebug( "SplitVideo::updateTime():   start time = %d,    end time = %d", m_startTime, m_endTime);
    QTime tFrom( 0, 0, 0 );
    tFrom = tFrom.addSecs( m_startTime );

    QTime tTo( 0, 0, 0 );
    tTo = tTo.addSecs( m_endTime );

    QTime tAll( 0, 0, 0 );
    tAll = tAll.addSecs( m_core->mdat.duration );

    ui->teFrom->setMinimumTime( QTime( 0, 0, 0 ) );
    ui->teFrom->setMaximumTime( tTo );
    ui->teFrom->setTime( tFrom );

    ui->teTo->setMinimumTime( tFrom );
    ui->teTo->setMaximumTime( tAll );
    ui->teTo->setTime( tTo );
}

void SplitVideo::updateControls()
{
    bool canSplit = true;

    // make checking for url
    if ( m_core->mdat.type == TYPE_STREAM ||
         m_core->mdat.type == TYPE_AUDIO_CD ||
         m_core->mdat.type == TYPE_TV ||
         m_core->mdat.type == TYPE_UNKNOWN )
    {
        canSplit = false;
    }

    if ( m_proc->isRunning() )
    {
        canSplit = false;
    }

    if ( m_core->mdat.filename.isEmpty() )
    {
        canSplit = false;
    }

    ui->pbCancel->setEnabled( m_proc->isRunning() );
    ui->pbCancel->setEnabled( !m_isStopMencoder );

    if ( canSplit )
        ui->pbSplit->setEnabled( !m_proc->isRunning() );
    else
        ui->pbSplit->setEnabled( canSplit );
    ui->progressBar->setValue( 0 );
    ui->teFrom->setEnabled( canSplit );
    ui->teTo->setEnabled( canSplit );
    ui->tbFrom->setEnabled( canSplit );
    ui->tbTo->setEnabled( canSplit );
    ui->labelError->setText( "" );
}

bool SplitVideo::checkDiskSpace( const QString& fname, int splitTime, double duration  )
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

    // it/s maximum factor for mencoder (for flv)
    // need get factors for all types video files
    double kmax = ( m_isCutError ) ? 3.03 : 1.1;

    qint64 szAll = fi.size();
    qint64 szPerSec = szAll / ( qint64 )duration; // size of 1 sec
    qint64 szKb = szPerSec * splitTime * kmax / 1024;

    double fTotal, fFree;
    bool rc = getFreeTotalSpaceKb( fi.filePath(), fTotal, fFree );
    qDebug( "SplitVideo::checkDiskSpace:   getFreeTotalSpace return %d ", rc );

    if ( isNew )
        QFile::remove( fname );

    if ( !rc ) return false;

    return ( szKb <= fFree);
}

void SplitVideo::startMencoder()
{
    QTime t;

    int startTime = t.secsTo( ui->teFrom->time() );
    int endTime = t.secsTo( ui->teTo->time() );

    QFileInfo fi;

    QString inputFile = m_core->mdat.filename;
    QString outputFile;

    fi.setFile( m_core->mdat.filename );

    if ( m_core->mdat.type == TYPE_DVD || m_core->mdat.type == TYPE_VCD )
    {
        outputFile = QDesktopServices::storageLocation( QDesktopServices::MoviesLocation ) + tr( "/Movie_" ) +
                QString::number( startTime ) + "_" +
                QString::number( endTime ) + ".avi";
    }
    else
    {
        outputFile = fi.absolutePath() + "/" + fi.baseName() + "_" +
                QString::number( startTime ) + "_" +
                QString::number( endTime ) + "." + fi.suffix();
    }
    outputFile = getNewFileName( outputFile );

    if ( !canWriteTo( outputFile ) )
    {
        qDebug("SplitVideo::startMencoder():   cannot trim video ( maybe your disk is mounted read-only? )");
        outputFile = QDesktopServices::storageLocation( QDesktopServices::MoviesLocation ) + "/" +
                fi.baseName() + "_" +
                QString::number( startTime ) + "_" +
                QString::number( endTime ) + "." + fi.suffix();
        outputFile = getNewFileName( outputFile );
    }

    qDebug("SplitVideo::startMencoder():   outputFile is %s", outputFile.toLocal8Bit().data() );

    // we cannot splitting if time is not valid
    if ( !checkTime( startTime, endTime ) )
        return;

    if ( !checkDiskSpace( outputFile, endTime - startTime, m_core->mdat.duration ) )
    {
        ui->labelError->setText( tr( "Cannot trim video ( maybe you have no enough disk space? )" ) );
        return;
    }

    m_isStopMencoder = false;
    m_error = -1;
    m_startTime = startTime;
    m_endTime = endTime;
    m_inputFile = inputFile;
    m_outputFile = outputFile;

    // Use absolute path, otherwise after changing to the screenshot directory
    // the mencoder path might not be found if it's a relative path
    // (seems to be necessary only for linux)
    QString mencoder_bin = pref->mencoder_bin;
    fi.setFile( mencoder_bin );
    if ( fi.exists() && fi.isExecutable() && !fi.isDir() )
    {
        mencoder_bin = fi.absoluteFilePath();
    }

    m_proc->clearArguments();
    m_proc->addArgument( mencoder_bin );
    m_proc->addArgument( m_inputFile );
    m_proc->addArgument( "-oac" );
    if ( m_isCutError )
        m_proc->addArgument( "pcm" );
    else
        m_proc->addArgument( "copy" );
    m_proc->addArgument( "-ovc" );
    m_proc->addArgument( "copy" );
    m_proc->addArgument( "-ss" );
    m_proc->addArgument( QString::number( m_startTime ) );
    m_proc->addArgument( "-endpos" );
    m_proc->addArgument( QString::number( m_endTime - m_startTime ) );
    m_proc->addArgument( "-o" );
    m_proc->addArgument( m_outputFile );

    QString commandline = m_proc->arguments().join(" ");
    qDebug("SplitVideo::startMencoder: command: '%s'", commandline.toUtf8().data());

    if ( !m_proc->start() ) {
        // error handling
        qWarning("SplitVideo::startMencoder: mencoder process didn't start");
    }

    updateControls();
}

void SplitVideo::stopMencoder()
{
    qDebug("SpliVideo::SplitVideo");

    m_isStopMencoder = true;

    if ( !m_proc->isRunning() )
    {
        qWarning("Core::stopMencoder: mencoder is not running!");
        updateControls();
        return;
    }

    qDebug( "SplitVideo::stopMencoder: Waiting mencoder to finish..." );
    if ( !m_proc->waitForFinished( 0 ) )
    {
        qWarning( "Core::stopMencoder: process didn't finish. Killing it..." );
        m_proc->terminate();
        m_proc->kill();
    }

    updateControls();
    qDebug( "SplitVideo::stopMencoder: Finished. (I hope)" );
}

void SplitVideo::tellmc( const QString & command )
{
    qDebug("SplitVideo::tellmc: '%s'", command.toUtf8().data());

    //qDebug("Command: '%s'", command.toUtf8().data());
    if ( m_proc->isRunning() )
    {
        m_proc->writeToStdin( command );
    } else
    {
        qWarning(" tellmc: no process running: %s", command.toUtf8().data());
    }
}

void SplitVideo::processFinished()
{
    qDebug("SplitVideo::processFinished");

    m_isStopMencoder = true;
    updateControls();

    int exit_code = m_proc->exitCode();
    qDebug("SplitVideo::processFinished: exit_code: %d", exit_code);
    if ( exit_code != 0 )
    {
        if ( m_isCutError )
        {
            if ( QFile::exists( m_outputFile ) )
                QFile::remove( m_outputFile );
            startMencoder();
            return;
        }

        ui->labelError->setText( tr( "Cannot trim video (code: %1)" ).arg( exit_code ) );
        if ( QFile::exists( m_outputFile ) )
            QFile::remove( m_outputFile );
    }
    else
    {
        m_isCutError = false;

        if ( m_error == (-1) )
        {
            QFileInfo fi( m_outputFile );

            QMessageBox mb( this );
            mb.setWindowTitle( tr( "Information" ) );
            mb.setText( tr( "New file \"%1\" saved in the folder %2" ).arg( fi.fileName() ).arg( fi.absolutePath() ) );
            mb.addButton( tr( "OK" ), QMessageBox::AcceptRole );
            foreach ( QAbstractButton* button, mb.buttons() )
                button->setIcon( QIcon() );
            mb.exec();
        }
        else
        {
            if ( QFile::exists( m_outputFile ) )
                QFile::remove( m_outputFile );
        }
    }

}

void SplitVideo::updateProgressBar( double sec )
{
    if ( !m_isStopMencoder )
    {
        int pos = ( sec * 100 ) / m_endTime;
//        qDebug("SplitVideo::updateProgressBar:   %d  ", pos);
        ui->progressBar->setValue( pos );
    }
    else
    {
        ui->progressBar->setValue( 0 );
    }
}

void SplitVideo::processError( int err )
{
    qDebug( "SplitVideo::processError:   error = %d  ", err );
    m_error = err;
}

void SplitVideo::processCutError()
{
    qDebug("SplitVideo::processCutError: ");
    m_isCutError = true;
}

