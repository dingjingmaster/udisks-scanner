#include "header-view.h"

#include <QDebug>
#include <QPainter>
#include <QCheckBox>
#include <QMouseEvent>
#include <QApplication>

HeaderView::HeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    setHighlightSections(false);
    setMouseTracking(true);

    QFont f = font();
    f.setBold (false);
    setFont (f);

//    setSectionsClickable(true);

    setSelectionMode (SingleSelection);
    setSectionResizeMode(QHeaderView::Stretch);
}

bool HeaderView::isChecked() const
{
    return mChecked;
}

void HeaderView::setChecked(bool c)
{
    if (c != mChecked) {
        mChecked = c;
        redrawCheckBox();
    }
}

void HeaderView::paintSection(QPainter *p, const QRect &rect, int logicalIndex) const
{
    p->save();
    QHeaderView::paintSection(p, rect, logicalIndex);
    p->restore();

    p->save();

    QPalette pal;
    p->setBrush(pal.windowText());

    if (logicalIndex == 0) {
        QRect rectCB(rect.left() + (rect.width() - 20) / 2, rect.top() + (rect.height() - 20) / 2, 20, 20);
        QStyleOptionButton cbOp;
        cbOp.initFrom(this);
        cbOp.palette = pal;
        cbOp.state |= QStyle::State_Enabled;
        cbOp.state |= QStyle::State_Sunken;
        cbOp.state |= mChecked ? QStyle::State_On : QStyle::State_Off;

        if (mChecked) {
            auto palette = cbOp.palette;
            palette.setColor (QPalette::All, QPalette::Highlight, QColor(204, 48, 51));
            cbOp.palette = palette;
        }

        cbOp.rect = rectCB;
        QApplication::style()->drawControl(QStyle::CE_CheckBox, &cbOp, p);
    }
    else {

    }

    p->restore();
}

void HeaderView::mousePressEvent(QMouseEvent *e)
{
    if (!logicalIndexAt(e->pos())) {
        setChecked(!isChecked());
        Q_EMIT checkBoxClicked(isChecked());
    }
}

void HeaderView::redrawCheckBox()
{
    viewport()->update();
}
