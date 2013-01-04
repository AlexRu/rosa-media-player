#ifndef VIDEOITEMDELEGATE_H
#define VIDEOITEMDELEGATE_H

#include <QStyledItemDelegate>

class VideoItemDelegate : public QStyledItemDelegate
{
public:
    VideoItemDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const;

};

#endif // VIDEOITEMDELEGATE_H
