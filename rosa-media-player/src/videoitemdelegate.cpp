#include "videoitemdelegate.h"

#include <QApplication>
#include <QtGui/QPainter>
#include <QtCore/QDebug>

#include "youtube/ytvideoitem.h"

#define ITEM_WIDTH      120
#define ITEM_HEIGHT     90

VideoItemDelegate::VideoItemDelegate()
{
}


//alocate each item size in listview.
QSize VideoItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    QFont font = QApplication::font();
    QFontMetrics fm(font);
    int margin = fm.height()/2;

    // calc width and height
    int w = ITEM_WIDTH + margin*2;
    int h = ITEM_HEIGHT + margin*2;

    return QSize(w, h);
}

void VideoItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
 {
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QFont font = QApplication::font();
    font.setBold(true);
    QFontMetrics fm(font);

    int margin = fm.height()/2;

    QImage pix = qvariant_cast<QImage>(index.data(YTVideoItem::SmallThumbnail));

    if (pix.size().width() > ITEM_WIDTH || pix.size().height() > ITEM_HEIGHT)
        pix = pix.scaled(QSize(ITEM_WIDTH, ITEM_HEIGHT), Qt::KeepAspectRatio);

    QString title = index.data(YTVideoItem::Title).toString();

    QRect titleRect = option.rect;
    QRect iconRect = option.rect;

    iconRect.setLeft(margin);
    iconRect.setWidth(ITEM_WIDTH);
    iconRect.setTop(iconRect.top() + margin);
    iconRect.setBottom(iconRect.top() + ITEM_HEIGHT);

    titleRect.setLeft(iconRect.right() + margin);
    titleRect.setTop(titleRect.top() + margin);

    painter->drawPixmap(iconRect.x(), iconRect.y(), pix.size().width(), pix.size().height(), QPixmap::fromImage(pix));
    painter->drawText(titleRect, Qt::AlignLeft|Qt::TextWordWrap, title);
    painter->restore();
}
