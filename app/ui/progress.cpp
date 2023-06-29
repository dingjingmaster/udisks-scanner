//
// Created by dingjing on 6/28/23.
//

#include <QDebug>
#include <QResizeEvent>
#include <QtCore/QEvent>
#include <QtGui/QPainter>

#include "../view/scanner-view.h"

#include "progress.h"

Progress::Progress(QWidget *parent)
    : QWidget(parent)
{
    //setAttribute(Qt::WA_NoSystemBackground, false);
//    setAttribute(Qt::WA_TranslucentBackground, false);

    if (parent) setFixedSize(parent->size());

    //mMainLayout = new QVBoxLayout{nullptr};

    mInfo = new QLabel(this);
    mInfo->setAutoFillBackground(true);
    mInfo->setAttribute(Qt::WA_TranslucentBackground, false);
    mInfo->setAttribute(Qt::WA_NoSystemBackground, false);

    if (parent) {
        if (parent->parent()) {
//                mInfo->setParent(static_cast<QWidget*>(parent->parent()->parent()));
            if (parent->parent()->parent()) {
//                mInfo->setParent(static_cast<QWidget*>(parent->parent()->parent()));
            }
        }
    }

    mInfo->setAlignment(Qt::AlignCenter);
    //mMainWidget = new QWidget;
    mProcess = new QProgressIndicator;
    updateLabelPosition();
    //mProcess->setColor(Qt::blue);

    connect(this, &Progress::updateProcess, this, [=] (int cur, int total) {

        if (cur < total && !mProcess->isAnimated()) {
            mProcess->startAnimation();
        } else if (cur >= total) {
            mProcess->stopAnimation();
        }

        mInfo->setText(QString("当前进度： %1/%2\t完成：%3%").arg(cur, 5).arg(total, 5)
                    .arg(QString::number(float(cur)/total * 100, 'f', 2)));
    });

}

void Progress::newParent()
{
    if (parent()) {
        parent()->installEventFilter(this);
        raise();
    }
}

bool Progress::eventFilter(QObject *obj, QEvent *ev)
{
    if (parent() == obj) {
        if (QEvent::Resize == ev->type()) {
            resize(static_cast<QResizeEvent*>(ev)->size());
        } else if (QEvent::ChildAdded == ev->type()) {
            raise();
        }
    }

    return QWidget::eventFilter(obj, ev);
}

bool Progress::event(QEvent *ev)
{
    if (QEvent::ParentAboutToChange == ev->type()) {
        if (parent()) {
            parent()->removeEventFilter(this);
        } else if (QEvent::ParentChange == ev->type()) {
            newParent();
        }
    } else if (QEvent::Show == ev->type()) {
        if (parent()) {
            auto w = static_cast<ScannerView*>(parent());
            if (w)  w->setGraphicsEffect(new QGraphicsBlurEffect);
            updateLabelPosition();
        }
    } else if (QEvent::Hide == ev->type()) {
        if (parent()) {
            auto w = static_cast<QWidget*>(parent());
            if (w)  w->setGraphicsEffect({});
        }
    } else if (QEvent::Resize == ev->type()) {
        if (parent()) {
            auto w = static_cast<QWidget*>(parent());
            if (w)  setFixedSize(w->size());
            updateLabelPosition();
        }
    }

    return QWidget::event(ev);
}

void Progress::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void Progress::updateLabelPosition()
{
    QRect rec = rect();

    QRect pos;

    pos.setX( (rec.width() - mLabelWidth) / 2);
    pos.setY( (rec.height() - mLabelHeight) / 2);

    pos.setWidth(mLabelWidth);
    pos.setHeight(mLabelHeight);

    mInfo->setGeometry(pos);

}

QProgressIndicator::QProgressIndicator(QWidget* parent)
        : QWidget(parent),
          mAngle(0),
          mTimerId(-1),
          mDelay(40),
          mDisplayedWhenStopped(false),
          mColor(Qt::black)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
}

bool QProgressIndicator::isAnimated () const
{
    return (mTimerId != -1);
}

void QProgressIndicator::setDisplayedWhenStopped(bool state)
{
    mDisplayedWhenStopped = state;

    update();
}

bool QProgressIndicator::isDisplayedWhenStopped() const
{
    return mDisplayedWhenStopped;
}

void QProgressIndicator::startAnimation()
{
    mAngle = 0;

    if (mTimerId == -1)
        mTimerId = startTimer(mDelay);
}

void QProgressIndicator::stopAnimation()
{
    if (mTimerId != -1)
        killTimer(mTimerId);

    mTimerId = -1;

    update();
}

void QProgressIndicator::setAnimationDelay(int delay)
{
    if (mTimerId != -1)
        killTimer(mTimerId);

    mDelay = delay;

    if (mTimerId != -1)
        mTimerId = startTimer(mDelay);
}

void QProgressIndicator::setColor(const QColor & color)
{
    mColor = color;

    update();
}

QSize QProgressIndicator::sizeHint() const
{
    return QSize(20,20);
}

int QProgressIndicator::heightForWidth(int w) const
{
    return w;
}

void QProgressIndicator::timerEvent(QTimerEvent * /*event*/)
{
    mAngle = (mAngle + 30) % 360;

    update();
}

void QProgressIndicator::paintEvent(QPaintEvent * /*event*/)
{
    if (!mDisplayedWhenStopped && !isAnimated())
        return;

    int width = qMin(this->width(), this->height());

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int outerRadius = (width-1)*0.5;
    int innerRadius = (width-1)*0.5*0.38;

    int capsuleHeight = outerRadius - innerRadius;
    int capsuleWidth  = (width > 32 ) ? capsuleHeight *.23 : capsuleHeight *.35;
    int capsuleRadius = capsuleWidth/2;

    for (int i = 0; i < 12; i++) {
        QColor color = mColor;
        color.setAlphaF(1.0f - (i/12.0f));
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.save();
        p.translate(rect().center());
        p.rotate(mAngle - i*30.0f);
        p.drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight), capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
        p.restore();
    }
}

