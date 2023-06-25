#include "push-button.h"

#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>

PushButton::PushButton(QWidget *parent, Type type)
    : QWidget{parent}, mType(type)
{
    setMouseTracking(true);
    setFixedWidth(mMaxWidth);
    setCursor(Qt::PointingHandCursor);
    setContentsMargins(0, 0, 0, 0);

    if (mType == Type1) setCursor(Qt::ArrowCursor);

    mLayout = new QVBoxLayout;
    mLayout->setSpacing(0);
    mLayout->setContentsMargins(0, 0, 0, 0);

    mLabel = new QLabel;
    mLabel->setMargin(mLabelMargin);
    mLabel->setFixedWidth(mMaxWidth);
    mLabel->setStyleSheet("font: blod;");
    mLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mLayout->addWidget(mLabel);

    switch (mType) {
        case Type1: {
            auto* widget = new QWidget;
            widget->setFixedHeight(3);
            mLabel->setStyleSheet("color: rgb(0, 0, 0);");
            widget->setStyleSheet("background-color:rgb(204, 48, 51);");
            mLayout->addWidget(widget);
            break;
        }
        case Type2: {
            mLabel->setStyleSheet("font: blod; color: rgb(255, 255, 255); border-radius: 6px;");
            setEnable();
            connect (this, &PushButton::enable, this, QOverload<bool>::of(&PushButton::setEnable));
            break;
        }
        case Type3: {
            mLabel->setStyleSheet("font: blod; color: rgb(204, 48, 51); border-radius: 6px;");
            setEnable();
            connect (this, &PushButton::enable, this, QOverload<bool>::of(&PushButton::setEnable));
            break;
        }
    }

    setLayout(mLayout);
}

void PushButton::setText(QString text)
{
    if (nullptr == text)        return;

    mLabel->setText(text);

    if (Type1 == mType) {
        int w = QFontMetrics(mLabel->font()).width(text) + 2 * mLabelMargin + 12;
        if (w > mMaxWidth) {
            setFixedWidth(w);
            mLabel->setFixedWidth(w);
            mLabel->update();
        } else {
            setFixedWidth(mMaxWidth);
            mLabel->setFixedWidth(mMaxWidth);
        }
    }
}

bool PushButton::isEnable()
{
    return mIsEnable;
}

void PushButton::setEnable()
{
    if (mType == Type1) return;
    if (mIsEnable) {
        setCursor(Qt::PointingHandCursor);
        mLabel->setStyleSheet("background-color:#cc3033; font: blod; color: rgb(255, 255, 255); border-radius: 6px;");
    } else {
        setCursor(Qt::ArrowCursor);
        mLabel->setStyleSheet("background-color:#696969; font: blod; color: rgb(255, 255, 255); border-radius: 6px;");
    } 
}

void PushButton::setEnable(bool e)
{
    mIsEnable = e;
    setEnable();    
}

void PushButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    if (mIsEnable) {
        qDebug() << "mouseReleaseEvent";
        Q_EMIT clicked ();
    }
}

void PushButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    
    if (mIsEnable) {
        doubleClicked();
    }
}
