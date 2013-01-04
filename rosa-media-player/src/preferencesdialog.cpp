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



#include "preferencesdialog.h"

#include "prefwidget.h"
#include "prefgeneral.h"
#include "prefsubtitles.h"
#include "colorutils.h"

#if USE_ASSOCIATIONS
#include "prefassociations.h"
#endif

#include "preferences.h"

#include <QVBoxLayout>
#include <QTextBrowser>
#include <QAbstractButton>
#include <QPushButton>

#include <QDebug>

#include "images.h"


SectionItem::SectionItem( const QString &text, const QPixmap &pixmap, QWidget *parent /*= 0*/ )
    : QWidget( parent )
{
    m_icon = new QLabel( this );
    m_icon->setPixmap( pixmap );
    m_icon->setScaledContents( true );
    m_icon->setFixedSize( 40, 40 );
    m_icon->setAlignment( Qt::AlignCenter );

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( m_icon );
    hlayout->addStretch();

    m_text = new QLabel( "<h4>" + text + "</h4>" );
    m_text->setAlignment( Qt::AlignCenter );

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setAlignment( Qt::AlignCenter );
    vlayout->addLayout( hlayout );
    vlayout->addWidget( m_text );
    setLayout( vlayout );
}

void SectionItem::setText( const QString &text )
{
    m_text->setText( "<h4>" + text + "</h4>" );
}

void SectionItem::setIcon( const QPixmap &icon )
{
    m_icon->setPixmap( icon );
}


PreferencesDialog::PreferencesDialog(QWidget * parent, Qt::WindowFlags f)
    : QDialog(parent, f )
{
    setupUi(this);

    // Setup buttons
    closeButton = buttonBox->button(QDialogButtonBox::Close);
    applyButton = buttonBox->button(QDialogButtonBox::Apply);
    connect( applyButton, SIGNAL( clicked() ), this, SLOT( apply() ) );
    connect( sections, SIGNAL( currentRowChanged( int ) ), SLOT( updateSection( int ) ) );

    setWindowIcon( Images::icon("logo") );

    help_window = new QTextBrowser(this);
    help_window->setWindowFlags(Qt::Window);
    help_window->resize( 300, 450 );
    //help_window->adjustSize();
    help_window->setWindowTitle( tr( "ROSA Media Player - Help" ) );
    help_window->setWindowIcon( Images::icon( "logo" ) );

    page_general = new PrefGeneral;
    addSection( page_general );

    page_subtitles = new PrefSubtitles(this);
    addSection( page_subtitles );

#if USE_ASSOCIATIONS
    page_associations = new PrefAssociations;
    addSection(page_associations);
#endif

    sections->setCurrentRow(General);

    //adjustSize();
    retranslateStrings();

    foreach (QAbstractButton* button, buttonBox->buttons())
        button->setIcon(QIcon());

    section_frame->setAutoFillBackground(TRUE);

    ColorUtils::setBackgroundColor( section_frame, QColor( "white" ) );
}

PreferencesDialog::~PreferencesDialog()
{
}

void PreferencesDialog::showSection(Section s)
{
    qDebug("PreferencesDialog::showSection: %d", s);

    sections->setCurrentRow( s );
}

void PreferencesDialog::updateSection( int s )
{
    PrefWidget *w = 0;
    switch ( s )
    {
    case General: w = page_general ; break;
    case View: w = page_subtitles; break;
    default: break;
    }

    if ( w ) {
        section_item_label->setText( "<h4>"+ w->sectionName() + "</h4>" );
        section_item_icon->setPixmap( w->sectionIcon() );
    }
}

void PreferencesDialog::retranslateStrings()
{
    retranslateUi( this );

    for ( int n = 0; n < pages->count(); n++)
    {
        PrefWidget *w = (PrefWidget*) pages->widget( n );
        SectionItem *item = qobject_cast<SectionItem*>( sections->itemWidget( sections->item( n ) ) );
        if ( item ) {
            item->setText( w->sectionName() );
            item->setIcon( w->sectionIcon() );
        }
    }

    updateSection( sections->currentRow() );

    if ( help_window->isVisible() ) {
        // Makes the help to retranslate
        showHelp();
    }

    help_window->setWindowTitle( tr("ROSA Media Player - Help") );

    // Qt 4.2 doesn't update the buttons' text
#if QT_VERSION < 0x040300
    okButton->setText( tr("OK") );
    cancelButton->setText( tr("Close") );
    applyButton->setText( tr("Apply") );
    helpButton->setText( tr("Help") );
#endif
}

void PreferencesDialog::accept()
{
    hide();
    help_window->hide();
    setResult( QDialog::Accepted );
    emit applied();
}

void PreferencesDialog::apply()
{
    setResult( QDialog::Accepted );
    emit applied();
}

void PreferencesDialog::reject()
{
    hide();
    help_window->hide();
    setResult( QDialog::Rejected );
}

void PreferencesDialog::addSection( PrefWidget *w )
{
    QListWidgetItem *i = new QListWidgetItem();

    QSize sz = i->sizeHint();
    sz.setHeight( 80 );
    i->setSizeHint( sz );

    sections->addItem( i );

    SectionItem *item = new SectionItem( w->sectionName(), w->sectionIcon(), this );
    sections->setItemWidget( i, item );

    pages->addWidget( w );
}

void PreferencesDialog::setData( Preferences *pref )
{
    page_general->setData(pref);
    page_subtitles->setData(pref);

#if USE_ASSOCIATIONS
    page_associations->setData(pref);
#endif
}

void PreferencesDialog::getData( Preferences *pref )
{
    page_general->getData(pref);
    page_subtitles->getData(pref);

#if USE_ASSOCIATIONS
    page_associations->getData(pref);
#endif
}

bool PreferencesDialog::requiresRestart()
{
    bool need_restart = page_general->requiresRestart();
    if (!need_restart) need_restart = page_subtitles->requiresRestart();

    return need_restart;
}

void PreferencesDialog::showHelp()
{
    PrefWidget * w = (PrefWidget*) pages->currentWidget();
    help_window->setHtml( w->help() );
    help_window->show();
    help_window->raise();
}

// Language change stuff
void PreferencesDialog::changeEvent(QEvent *e)
{
    if ( e->type() == QEvent::LanguageChange ) {
        retranslateStrings();
    }
    else {
        QDialog::changeEvent(e);
    }
}

#include "moc_preferencesdialog.cpp"
