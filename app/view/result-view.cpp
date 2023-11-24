//
// Created by dingjing on 23-6-15.
//

#include "result-view.h"

#include <QDebug>
#include <QToolTip>

#include "model/result-item.h"
#include "db/scan-result-db.h"


ResultView::ResultView(QWidget *parent)
    : QTableView(parent)
{
    setMouseTracking (true);
    setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::NoSelection);
}

void ResultView::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex idx = indexAt(event->pos());
    auto db = static_cast<ScanResultDB*>(idx.internalPointer());

    if (db) {
        auto item = db->getItemByIndex (idx.row());
        if (item) {
            if (4 == idx.column()) {
                QPoint p = mapToGlobal (event->pos());
                QToolTip::showText (p + QPoint(16, 16), item->getContext(), this, QRect(), 0);
            }
        }
    }

    QAbstractItemView::mouseMoveEvent (event);
}
