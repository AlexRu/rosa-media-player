#include "videosearch.h"

#include <QtCore/QDebug>
#include <QtCore/QBuffer>
#include <QtGui/QDesktopServices>
#include <QtGui/QScrollBar>
#include <QtGui/QMenu>

#include "youtube/reply.h"
#include "youtube/request.h"
#include "youtube/requestmanager.h"
#include "youtube/ytvideoitem.h"
#include "youtube/ytvideoitemmodel.h"
#include "videoitemdelegate.h"
#include "core.h"
#include "remoteimageloader.h"
#include "myaction.h"
#include "playlist.h"

VideoSearch::VideoSearch(Core* core, Playlist *playlist, QWidget *parent)
    : QWidget(parent)
    , ui( new Ui::VideoSearchPanel )
    , m_core(core)
    , m_playlist(playlist)
{
    ui->setupUi(this);

    m_videoModel = new YTVideoItemModel(this);

    m_imageLoader = new RemoteImageLoader(this);
    connect(m_imageLoader, SIGNAL(imageReady(QUrl, QImage)), SLOT(gotVideoThumbnail(QUrl, QImage)));

    m_requestManager = new RequestManager(this);
    connect(m_requestManager, SIGNAL(newVideoItems(QList<YTVideoItem *>)), SLOT(gotVideoItems(QList<YTVideoItem *>)));

    VideoItemDelegate *delegate = new VideoItemDelegate();

    ui->lvVideos->setModel(m_videoModel);
    ui->lvVideos->setItemDelegate(delegate);
    ui->lvVideos->setContextMenuPolicy(Qt::CustomContextMenu);

//    connect(ui->lvVideos->verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(onSliderMoved(int)));
    connect(ui->lvVideos, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));

    createActions();
    retranslateStrings();
}

void VideoSearch::gotVideoItems(QList<YTVideoItem *> items)
{
    if (!m_videoModel)
        return;

    if (m_imageLoader) {
        QList<QUrl> images;
        foreach (YTVideoItem *item, items) {
            if (item) {
                images << item->data(YTVideoItem::SmallThumbnailUrl).toUrl();
            }
        }
        m_imageLoader->addImages(images);
        m_imageLoader->load();
    }

    m_videoModel->newVideoItems(items);
}

void VideoSearch::onReturnPressed()
{
    m_videoModel->clear();
    if (!ui->leSearchString->text().isEmpty()) {
        getVideo(ui->leSearchString->text());
    }
}

void VideoSearch::onItemClicked(QModelIndex index)
{
    if (m_core && index.isValid()) {

        QString url = index.data(YTVideoItem::VideoUrl).toString();
        if (!url.isEmpty()) {
            m_core->openStream(url);
        }
    }
}

void VideoSearch::onSliderMoved(int value)
{
    if (value == ui->lvVideos->verticalScrollBar()->maximum()) {
        // post new request...
        searchMore();
    }
}

void VideoSearch::gotVideoThumbnail(QUrl url, QImage image)
{
    for (int i = 0; i < m_videoModel->rowCount(); i++) {
        if (m_videoModel->index(i).data(YTVideoItem::SmallThumbnailUrl).toUrl() == url) {
            m_videoModel->setData(m_videoModel->index(i), image, YTVideoItem::SmallThumbnail);
            break;
        }
    }
}

void VideoSearch::showContextMenu(const QPoint &pos)
{
    if (m_videoModel->rowCount() == 0)
        return;

    QModelIndex index = ui->lvVideos->indexAt(pos);
    if (!index.isValid())
        return;

    QMenu contextMenu(tr("Context menu"), this);
    contextMenu.addAction(m_playAct);
    contextMenu.addAction(m_addPlaylistAct);
    contextMenu.addAction(m_openBrowserAct);
    contextMenu.exec(mapToGlobal(pos));
}

void VideoSearch::playVideo()
{
    QModelIndex index = ui->lvVideos->currentIndex();
    if (!index.isValid())
        return;

    onItemClicked(index);
}

void VideoSearch::addToPlaylist()
{
    QModelIndex index = ui->lvVideos->currentIndex();

    if (!index.isValid())
        return;

    if (m_playlist) {
        QString videoUrl = index.data(YTVideoItem::VideoUrl).toString();
        QString title = index.data(YTVideoItem::Title).toString();
        double duration = index.data(YTVideoItem::Duration).toDouble();

        m_playlist->addItem(videoUrl, title, duration);
        m_playlist->getMediaInfo();
    }
}

void VideoSearch::openBrowser()
{
    QModelIndex index = ui->lvVideos->currentIndex();

    if (!index.isValid())
        return;

    QString videoUrl = index.data(YTVideoItem::VideoUrl).toString();
    QDesktopServices::openUrl(videoUrl);
}

void VideoSearch::clearVideoList()
{
    ui->leSearchString->clear();
    m_videoModel->clear();
    ui->lvVideos->reset();
}

void VideoSearch::searchMore()
{
    if (!ui->leSearchString->text().isEmpty()) {
        getVideo(ui->leSearchString->text(), m_videoModel->rowCount());
    }
}

Reply *VideoSearch::videoUrl(const QString &searchVideo, int index)
{
    Request *request = m_requestManager->queryVideo(searchVideo, index);
    Reply *reply = new Reply(request, this);
    if (request != 0)
        request->start();

    return reply;
}

void VideoSearch::getVideo(const QString &searchVideo, int index)
{
    Reply *reply = videoUrl(searchVideo, index);
    /*TO-DO: process error replies*/
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void VideoSearch::createActions()
{
    m_playAct = new MyAction(this, "yt_play_video", false);
    connect(m_playAct, SIGNAL(triggered()), this, SLOT(playVideo()));

    m_addPlaylistAct = new MyAction(this, "yt_add_to_playlist", false);
    connect( m_addPlaylistAct, SIGNAL(triggered()), this, SLOT(addToPlaylist()));

    m_openBrowserAct = new MyAction(this, "yt_copy_to_clipboard", false);
    connect(m_openBrowserAct, SIGNAL(triggered()), this, SLOT(openBrowser()));
}

void VideoSearch::retranslateStrings()
{
    m_playAct->change( tr("&Play") );
    m_addPlaylistAct->change( tr("&Add to play list") );
    m_openBrowserAct->change( tr("&Open video in browser...") );
}

// Language change stuff
void VideoSearch::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        retranslateStrings();
    }
    else {
        QWidget::changeEvent(e);
    }
}
