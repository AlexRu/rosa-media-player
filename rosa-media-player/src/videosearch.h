#ifndef VIDEOSEARCH_H
#define VIDEOSEARCH_H

#include <QtGui/QWidget>
#include <QtCore/QUrl>

#include "ui_videosearch.h"


namespace Ui
{
class VideoSearchPanel;
}

class RequestManager;
class Reply;
class YTVideoItem;
class YTVideoItemModel;
class RemoteImageLoader;
class Core;
class Playlist;
class MyAction;

class VideoSearch : public QWidget
{
    Q_OBJECT
public:
    explicit VideoSearch(Core *core, Playlist *playlist, QWidget *parent = 0);
    
signals:
    
protected slots:
    void gotVideoItems(QList<YTVideoItem *> items);
    void onReturnPressed();
    void onItemClicked(QModelIndex index);
    void onSliderMoved(int value);
    void gotVideoThumbnail(QUrl url, QImage image);
    void showContextMenu(const QPoint &pos);

    void playVideo();
    void addToPlaylist();
    void openBrowser();
    void clearVideoList();
    void searchMore();

protected:
    Reply *videoUrl(const QString &searchVideo, int index);
    void getVideo(const QString &searchVideo, int index = 0);
    void retranslateStrings();

    void createActions();

    virtual void changeEvent(QEvent *event);

private:
    Ui::VideoSearchPanel* ui;

    RequestManager *m_requestManager;
    YTVideoItemModel *m_videoModel;
    RemoteImageLoader *m_imageLoader;
    Core* m_core;
    Playlist* m_playlist;

    MyAction *m_playAct;
    MyAction *m_addPlaylistAct;
    MyAction *m_openBrowserAct;

};

#endif // VIDEOSEARCH_H
