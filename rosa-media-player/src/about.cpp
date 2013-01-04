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

#include "about.h"
#include "images.h"
#include "version.h"
#include "global.h"
#include "preferences.h"
#include "paths.h"
#include "mplayerversion.h"
#include <QAbstractButton>

#include <QFile>

//#define TRANS_ORIG
#define TRANS_LIST
//#define TRANS_TABLE

using namespace Global;


bool Filter::eventFilter( QObject* obj, QEvent* e )
{
    if ( !obj->isWidgetType() ) return true;
    QWidget* w = (QWidget*) obj;
    if ( e->type() == QEvent::Enter )
    {
        /*
        QPalette palette;
        palette.setColor( w->foregroundRole(), palette.color( QPalette::Link ) );
        w->setPalette( palette );
*/
        w->setCursor( QCursor( Qt::PointingHandCursor ) );
    }
    else if ( e->type() == QEvent::Leave )
    {
        /*
        QPalette palette;
        palette.setColor( w->foregroundRole(), palette.color( QPalette::WindowText ) );
        w->setPalette(palette);
        */
        w->setCursor( QCursor( Qt::ArrowCursor ) );
    }
    else if ( e->type() == QEvent::MouseButtonPress )
    {
        emit clicked();
    }

    return false;
}

About::About( QWidget* parent, Qt::WindowFlags f )
    : QDialog( parent, f )
{
    setupUi(this);

    setWindowFlags( Qt::Tool | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint );
    setFixedSize( 400, 250 );

    m_filter = new Filter();
    connect( m_filter, SIGNAL( clicked() ), this, SLOT( onTermsClicked() ) );

    QPalette pal = labelTerms->palette();
    pal.setColor( labelTerms->foregroundRole(), pal.color( QPalette::Link ) );
    labelTerms->setPalette( pal );
    labelTerms->setAutoFillBackground( true );
    labelTerms->installEventFilter( m_filter );

    setWindowIcon( Images::icon( "logo", 64 ) );

    QString mplayer_version;
    if (pref->mplayer_detected_version > 0)
    {
        mplayer_version = tr("Using MPlayer %1").arg(MplayerVersion::toString(pref->mplayer_detected_version)) + "<br><br>";
    }

    info->setText( tr( "<html><head></head><body><table width=\"100%\" cellpadding=\"0\"><tr>"
                       "<td><p><span style=\"font-size:14pt; font-weight:600;\">ROSA Media Player</span><br/><br/>Version %1</p></td>"
                       "<td width=\"128\" valign=\"top\">"
                       "<img src=\":/icons-png/logo.png\" width=\"128\" height=\"128\"></td>"
                       "</tr></table></body></html>" ).arg( rompVersion() ) );

    labelCopyright->setText( tr( "(c) ROSA 2011-2012\n\nROSA Media Player is a free software." ) );
    labelTerms->setText( tr( "terms of use" ) );

    QString license_file = Paths::doc("gpl.html", pref->language);
    if ( QFile::exists(license_file) )
    {
        qDebug("license_file is exist...");
        QFont fixed_font;
        fixed_font.setStyleHint(QFont::TypeWriter);
        fixed_font.setFamily("Courier");
        license->setFont(fixed_font);

        QFile f(license_file);
        if (f.open(QIODevice::ReadOnly))
        {
            license->setText(QString::fromUtf8(f.readAll().constData()));
        }
        f.close();
    }
    else
    {
        license->setText(
                    "<i>" +
                    tr("This program is free software; you can redistribute it and/or modify "
                       "it under the terms of the <a href=\"http://www.gnu.org/licenses/gpl.html\">GNU General Public License</a> as published by "
                       "the <a href=\"http://www.fsf.org\">Free Software Foundation</a>; either version 3 of the License, or "
                       "(at your option) any later version.") + "</i>");
    }

    // Copy the background color ("window") of the tab widget to the "base" color of the qtextbrowsers
    // Problem, it doesn't work with some styles, so first we change the "window" color of the tab widgets.
    info_tab->setAutoFillBackground( true );
    license_tab->setAutoFillBackground( true );

    pal = info_tab->palette();
    pal.setColor( QPalette::Window, palette().color( QPalette::Window ) );

    info_tab->setPalette(pal);
    license_tab->setPalette(pal);

    pal = info->palette();
    pal.setColor( QPalette::Base, info_tab->palette().color( QPalette::Window ) );

    adjustSize();
    //foreach (QAbstractButton* button, buttonBox->buttons())
    //button->setIcon(QIcon());

    stackedWidget->setCurrentWidget( info_tab );
    setFocus();
}

void About::focusOutEvent ( QFocusEvent * event )
{
    Q_UNUSED( event )
    close();
}

About::~About()
{
}

void About::onTermsClicked()
{
    stackedWidget->setCurrentWidget( license_tab );
}

QString About::getTranslators()
{
    return QString(
                tr("The following people have contributed with translations:") +
            #ifndef TRANS_TABLE
                "<ul>" +
            #else
                "<table>" +
            #endif
                trad(tr("German"), "Henrikx <henrikx@users.sourceforge.net>") +
                trad(tr("Slovak"), "Sweto <peter.mendel@gmail.com>") +
                trad(tr("Italian"), QStringList()
                     << "greengreat <gmeildeno@gmail.com>"
                     << "Giancarlo Scola <scola.giancarlo@libero.it>") +
                trad(tr("French"), QStringList()
                     << "Olivier g <1got@caramail.com>"
                     << "Temet <goondy@free.fr>"
                     << "Erwann MEST <kud.gray@gmail.com>") +
                trad(tr("Simplified-Chinese"), "Tim Green <iamtimgreen@gmail.com>") +
                trad(tr("Russian"), QString::fromUtf8("Белый Владимир <wiselord1983@gmail.com>"))+
                trad(tr("Hungarian"), QStringList()
                     << "Charles Barcza <kbarcza@blackpanther.hu>"
                     << "CyberDragon <cyberdragon777@gmail.com>") +
                trad(tr("Polish"), QStringList()
                     << "qla <qla0@vp.pl>"
                     << "Jarek <ajep9691@wp.pl>" ) +
                trad(tr("Japanese"), "Nardog <nardog@e2umail.com>") +
                trad(tr("Dutch"), QStringList()
                     << "profoX <wesley@ubuntu-nl.org>"
                     << "BalaamsMiracle"
                     << "Kristof Bal <kristof.bal@gmail.com>") +
                trad(tr("Ukrainian"), QStringList()
                     << "Motsyo Gennadi <drool@altlinux.ru>"
                     << "Oleksandr Kovalenko <alx.kovalenko@gmail.com>" ) +
                trad(tr("Portuguese - Brazil"), "Ventura <ventura.barbeiro@terra.com.br>") +
                trad(tr("Georgian"), "George Machitidze <giomac@gmail.com>") +
                trad(tr("Czech"), QStringList()
                     << QString::fromUtf8("Martin Dvořák <martin.dvorak@centrum.cz>")
                     << QString::fromUtf8("Jaromír Smrček <jaromir.smrcek@zoner.com>") ) +
                trad(tr("Bulgarian"), "<marzeliv@mail.bg>") +
                trad(tr("Turkish"), "alper er <alperer@gmail.com>") +
                trad(tr("Swedish"), "Leif Larsson <leif.larsson@gmail.com>") +
                trad(tr("Serbian"), "Kunalagon Umuhanik <kunalagon@gmail.com>") +
                trad(tr("Traditional Chinese"), "Hoopoe <dai715.tw@yahoo.com.tw>") +
                trad(tr("Romanian"), "DoruH <DoruHushHush@gmail.com>") +
                trad(tr("Portuguese - Portugal"), QStringList()
                     << "Waxman <waxman.pt@gmail.com>"
                     << QString::fromUtf8("Sérgio Marques <smarquespt@gmail.com>") ) +
                trad(tr("Greek"), "my80s <wamy80s@gmail.com>") +
                trad(tr("Finnish"), "peeaivo <peeaivo@gmail.com>") +
                trad(tr("Korean"), "Heesu Yoon <imsu30@gmail.com>") +
                trad(tr("Macedonian"), "Marko Doda <mark0d0da@gmail.com>") +
                trad(tr("Basque"), "Piarres Beobide <pi@beobide.net>") +
                trad(tr("Catalan"), QString::fromUtf8("Roger Calvó <rcalvoi@yahoo.com>")) +
                trad(tr("Slovenian"), "Janez Troha <janez.troha@gmail.com>") +
                trad(tr("Arabic"), "Muhammad Nour Hajj Omar <arabianheart@live.com>") +
                trad(tr("Kurdish"), "Si_murg56 <simurg56@gmail.com>") +
                trad(tr("Galician"), "Miguel Branco <mgl.branco@gmail.com>") +
                trad(tr("Vietnamese"), QString::fromUtf8("Lê Xuân Thảo <thaolx@gmail.com>")) +
                trad(tr("Estonian"), QString::fromUtf8("Olav Mägi <olav.magi@hotmail.com>")) +
                trad(tr("Lithuanian"), "Freemail <ricka_g@freemail.lt>") +
            #ifndef TRANS_TABLE
                "</ul>");
#else
                "</table>");
#endif
}

QString About::trad(const QString & lang, const QString & author)
{
    return trad(lang, QStringList() << author);
}

QString About::trad(const QString & lang, const QStringList & authors)
{
#ifdef TRANS_ORIG
    QString s;

    switch (authors.count())
    {
    case 2:
        s = tr("%1 and %2");
        break;
    case 3:
        s = tr("%1, %2 and %3");
        break;
    case 4:
        s = tr("%1, %2, %3 and %4");
        break;
    case 5:
        s = tr("%1, %2, %3, %4 and %5");
        break;
    default:
        s = "%1";
    }

    for (int n = 0; n < authors.count(); n++)
    {
        QString author = authors[n];
        s = s.arg(author.replace("<", "&lt;").replace(">", "&gt;"));
    }

    return "<li>"+ tr("<b>%1</b>: %2").arg(lang).arg(s) + "</li>";
#endif

#ifdef TRANS_LIST
    QString s = "<ul>";;
    for (int n = 0; n < authors.count(); n++)
    {
        QString author = authors[n];
        s += "<li>"+ author.replace("<", "&lt;").replace(">", "&gt;") + "</li>";
    }
    s+= "</ul>";

    return "<li>"+ tr("<b>%1</b>: %2").arg(lang).arg(s) + "</li>";
#endif

#ifdef TRANS_TABLE
    QString s;
    for (int n = 0; n < authors.count(); n++)
    {
        QString author = authors[n];
        s += author.replace("<", "&lt;").replace(">", "&gt;");
        if (n < (authors.count()-1)) s += "<br>";
    }

    return QString("<tr><td align=right><b>%1</b></td><td>%2</td></tr>").arg(lang).arg(s);
#endif
}

QString About::link(const QString & url, QString name)
{
    if (name.isEmpty()) name = url;
    return QString("<a href=\"" + url + "\">" + name +"</a>");
}

QString About::contr(const QString & author, const QString & thing)
{
    return "<li>"+ tr("<b>%1</b> (%2)").arg(author).arg(thing) +"</li>";
}

QSize About::sizeHint () const
{
    return QSize(518, 326);
}

#include "moc_about.cpp"
