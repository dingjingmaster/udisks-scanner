#include "scanner-view.h"

#include <QDebug>
#include <QScrollBar>
#include <QHeaderView>
#include <QFontMetrics>
#include <QApplication>

#include "db/scan-task-db.h"
#include "model/scanner-task-item.h"

ScannerView::ScannerView(QWidget *parent)
    : QTableView(parent)
{
    setMouseTracking(true);

    setContentsMargins (0, 0, 0, 0);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::NoSelection);

    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void ScannerView::paintEvent(QPaintEvent *ev)
{
    QTableView::paintEvent(ev);
}

void ScannerView::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex idx = indexAt(event->pos());

    if (6 == idx.column()) {
        auto db = static_cast<ScanTaskDB*>(idx.internalPointer());
        if (!db) return;
        auto item = db->getItemByIndex (idx.row());
        if (!item) return;
        QRect rect = QTableView::visualRect (idx);

        static const int mlr = 5;
        static const int h = 30;
        auto f = qApp->font();
        f.setPointSizeF (f.pointSizeF() - 2);
        QFontMetrics fm(f);
        static const int fSize1 = fm.width ("扫扫扫  |");
        static const int fSize2 = fm.width ("启动");

        int w = rect.width();

        int startX1 = rect.left() + (w - 4 * mlr - fSize1 - 4 * fSize2) / 2;
        int startY = rect.top() + (rect.height() - h) / 2;

        int startX2 = startX1 + fSize1 + mlr;
        QRect qd(startX2, startY, fSize2, h);

        int startX3 = startX2 + fSize2 + mlr;
        QRect zt(startX3, startY, fSize2, h);

        int startX4 = startX3 + fSize2 + mlr;
        QRect tz(startX4, startY, fSize2, h);

        int startX5 = startX4 + fSize2 + mlr;
        QRect sc(startX5, startY, fSize2, h);

        switch (item->getStatus()) {
            case ScannerTaskItem::NoBegin: {
                if (qd.contains (event->pos()) || sc.contains (event->pos())) {
                    setCursor (Qt::PointingHandCursor);
                }
                else {
                    setCursor (Qt::ArrowCursor);
                }
                break;
            }
            case ScannerTaskItem::Scanning: {
                if (zt.contains (event->pos()) || tz.contains (event->pos())) {
                    setCursor (Qt::PointingHandCursor);
                }
                else {
                    setCursor (Qt::ArrowCursor);
                }
                break;
            }
            case ScannerTaskItem::Stop: {
                if (qd.contains (event->pos()) || sc.contains (event->pos())) {
                    setCursor (Qt::PointingHandCursor);
                }
                else {
                    setCursor (Qt::ArrowCursor);
                }
                break;
            }
            case ScannerTaskItem::Finish: {
                if (qd.contains (event->pos()) || sc.contains (event->pos())) {
                    setCursor (Qt::PointingHandCursor);
                }
                else {
                    setCursor (Qt::ArrowCursor);
                }
                break;
            }
            case ScannerTaskItem::Suspended: {
                if (zt.contains (event->pos()) || tz.contains (event->pos()) || sc.contains (event->pos())) {
                    setCursor (Qt::PointingHandCursor);
                }
                else {
                    setCursor (Qt::ArrowCursor);
                }
                break;
            }
            default: {
//                setCursor (Qt::ArrowCursor);
                break;
            }
        }
    }
    else if (8 == idx.column()) {
        setCursor (Qt::PointingHandCursor);
    }
    else {
        setCursor (Qt::ArrowCursor);
    }

    QAbstractItemView::mouseMoveEvent (event);
}

void ScannerView::mouseReleaseEvent(QMouseEvent *event)
{
    QModelIndex idx = indexAt(event->pos());
    if (!idx.isValid()) {
        return;
    }

    auto db = static_cast<ScanTaskDB*>(idx.internalPointer());
    if (!db) return;
    auto item = db->getItemByIndex (idx.row());
    if (!item) return;
    if (6 == idx.column()) {
        QRect rect = QTableView::visualRect (idx);

        static const int mlr = 5;
        static const int h = 30;
        auto f = qApp->font();
        f.setPointSizeF (f.pointSizeF() - 2);
        QFontMetrics fm(f);
        static const int fSize1 = fm.width ("扫扫扫  |");
        static const int fSize2 = fm.width ("启动");

        int w = rect.width();

        int startX1 = rect.left() + (w - 4 * mlr - fSize1 - 4 * fSize2) / 2;
        int startY = rect.top() + (rect.height() - h) / 2;

        int startX2 = startX1 + fSize1 + mlr;
        QRect qd(startX2, startY, fSize2, h);

        int startX3 = startX2 + fSize2 + mlr;
        QRect zt(startX3, startY, fSize2, h);

        int startX4 = startX3 + fSize2 + mlr;
        QRect tz(startX4, startY, fSize2, h);

        int startX5 = startX4 + fSize2 + mlr;
        QRect sc(startX5, startY, fSize2, h);

        switch (item->getStatus()) {
            case ScannerTaskItem::NoBegin: {
                if (qd.contains (event->pos())) {
                    Q_EMIT taskStart(item->getID());
                } else if (sc.contains (event->pos())) {
                    Q_EMIT taskDelete(item->getID());
                }
                break;
            }
            case ScannerTaskItem::Scanning: {
                if (zt.contains (event->pos())) {
                    Q_EMIT taskPause(item->getID());
                } else if (tz.contains (event->pos())) {
                    Q_EMIT taskStop(item->getID());
                }
                break;
            }
            case ScannerTaskItem::Stop:
            case ScannerTaskItem::Finish: {
                if (qd.contains (event->pos())) {
                    Q_EMIT taskStart(item->getID());
                } else if (sc.contains (event->pos())) {
                    Q_EMIT taskDelete(item->getID());
                }
                break;
            }
            case ScannerTaskItem::Suspended: {
                if (zt.contains (event->pos())) {
                    Q_EMIT taskResume(item->getID());
                } else if (tz.contains (event->pos())) {
                    Q_EMIT taskStop(item->getID());
                } else if (sc.contains (event->pos())) {
                    Q_EMIT taskDelete(item->getID());
                }
                break;
            }
            default: {
                break;
            }
        }
    }
    else if (8 == idx.column()) {
        Q_EMIT taskDetail(item->getID());
    }

    QAbstractItemView::mouseReleaseEvent (event);
}

