//
// Created by dingjing on 23-6-14.
//

#include "scanner-task-delegate.h"

#include <QFont>
#include <QDebug>
#include <QObject>
#include <QApplication>

#include "model/scanner-task-item.h"
#include "model/scanner-task-model.h"


ScannerTaskDelegate::ScannerTaskDelegate(QObject* parent)
    : QStyledItemDelegate (parent)
{
    QFont f = qApp->font();

    mFontSize = f.pointSizeF() - 2;
}

void ScannerTaskDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    // 绘制背景
    auto item = static_cast<ScannerTaskItem*>(index.internalPointer());
    if (!item) { return;}

    p->save();

    QRect rect = option.rect;
    QStyleOptionViewItem itemOption;
    itemOption = option;
    itemOption.font.setPointSizeF (mFontSize);

    p->setFont (itemOption.font);

    if (item->getIsChecked()) {
        p->save();
        p->setPen (Qt::NoPen);
        p->setBrush (QColor(255, 138, 140));
        p->drawRect (rect);
        p->restore();
    }

    switch (index.column()) {
        case 0: {
            QRect rectCB(rect.left() + (rect.width() - 20) / 2, rect.top() + (rect.height() - 20) / 2, 20, 20);
            QStyleOptionButton cbOp;
            cbOp.initFrom(dynamic_cast<QWidget*>(parent()));
            cbOp.state |= QStyle::State_Enabled;
            cbOp.rect = rectCB;

            QBrush bth;
            bth.setColor(Qt::darkRed);
            if (index.model()->data(index).toBool()) {
                cbOp.state |= QStyle::State_On;
            } else {
                cbOp.state |= QStyle::State_Off;
            }

            p->setBrush(bth);
            QApplication::style()->drawControl(QStyle::CE_CheckBox, &cbOp, p);
            break;
        }
        case ScannerTaskModel::TaskStatus: {
            static const int fSize1 = p->fontMetrics().width ("扫扫扫  |");
            static const int fSize2 = p->fontMetrics().width ("启动");

            int w = rect.width();

            int startX1 = rect.left() + (w - 4 * mStatusMargin - fSize1 - 4 * fSize2) / 2;
            int startY = rect.top() + (rect.height() - mStatusHigh) / 2;

            QRect st(startX1, startY, fSize1, mStatusHigh);
            p->drawText (st, "扫描中  |");

            int startX2 = startX1 + fSize1 + mStatusMargin;
            QRect qd(startX2, startY, fSize2, mStatusHigh);
            p->drawText (qd, "启动");

            int startX3 = startX2 + fSize2 + mStatusMargin;
            QRect zt(startX3, startY, fSize2, mStatusHigh);
            p->drawText (zt, "暂停");

            int startX4 = startX3 + fSize2 + mStatusMargin;
            QRect tz(startX4, startY, fSize2, mStatusHigh);
            p->drawText (tz, "停止");

            int startX5 = startX4 + fSize2 + mStatusMargin;
            QRect sc(startX5, startY, fSize2, mStatusHigh);
            p->drawText (sc, "删除");

            break;
        }
        case ScannerTaskModel::TaskProgress: {
            static const int mlr = 5;
            static const int h = 16;

            QRect rectCB(rect.left() + mlr, rect.top() + (rect.height() - h) / 2, rect.width() - 2 * mlr, h);
            QStyleOptionProgressBar cbOp;
            cbOp.maximum |= 100;
            cbOp.minimum = 0;
            cbOp.progress = 80;
            cbOp.rect = rectCB;
            cbOp.textVisible = true;
            cbOp.text = "60/100";

            p->save();
            auto f = p->font();
            f.setPointSizeF (f.pointSizeF() - 2);
            p->setFont (f);
            QApplication::style()->drawControl(QStyle::CE_ProgressBar, &cbOp, p);
            p->restore();

            break;
        }
#if 0
        case 2: {
            auto m = index.model();
            if (m) {
                auto d = m->data (index);
                if (d.isValid()) {
                    QString text = d.toString();
                    if (nullptr == text || text.isNull() || text.isEmpty()) text = "";
                    p->setBrush(pal.windowText());
                    p->drawText(rect, Qt::AlignCenter, text);
                }
            }
            break;
        }
        case 3:
        case 4:
        case 5: {
            auto m = index.model();
            if (m) {
                auto d = m->data (index);
                if (d.isValid()) {
                    QString text = d.toString();
                    if (nullptr == text || text.isNull() || text.isEmpty()) text = "";
                    align |= Qt::AlignHCenter;
                    p->setBrush(pal.windowText());
                    p->drawText(rect, (int) align, text);
                }
            }
            break;
        }
        case 1: {
            auto m = index.model();
            if (m) {
                auto d = m->data (index);
                if (d.isValid()) {
                    QString text = d.toString();
                    if (nullptr == text || text.isNull() || text.isEmpty()) text = "";
                    align |= Qt::AlignLeft;
                    p->setBrush(pal.windowText());
                    QFont f = option.font;
                    QFontMetrics fm(f);
                    QString text1 = fm.elidedText (text, Qt::ElideRight, rect.width() * 2 - 10);
                    p->setFont (f);
                    p->drawText(rect, (int) (align | Qt::TextWordWrap | Qt::ElideRight), text1);
                }
            }
            break;
        }
#endif
        default: {
            QStyledItemDelegate::paint (p, itemOption, index);
            break;
        }
    }

    p->restore();
}
