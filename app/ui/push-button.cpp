#include "push-button.h"

#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>

PushButton::PushButton(QWidget *parent, Type type)
    : QWidget{parent}, mType(type)
{
    setFixedHeight (40);
    setMouseTracking(true);
    setFixedWidth(mMaxWidth);
    setContentsMargins(0, 0, 0, 0);
    setCursor(Qt::PointingHandCursor);

    if (mType == Type1) setCursor(Qt::ArrowCursor);

    mLayout = new QVBoxLayout;
    mLayout->setSpacing(0);
    mLayout->setContentsMargins(0, 0, 0, 0);

    mLabel = new QLabel;
    mLabel->setMargin(mLabelMargin);
    mLabel->setFixedWidth(mMaxWidth);
    mLabel->setAutoFillBackground (true);
    mLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mLayout->addWidget(mLabel);

    // private
    mPLine = new QWidget;
    mPLine->setFixedHeight(3);
    mPLine->setStyleSheet("background-color:rgb(204, 48, 51);");
    mLayout->addWidget(mPLine);

    setLayout(mLayout);

    setType (mType);
    connect (this, &PushButton::enable, this, QOverload<bool>::of(&PushButton::setEnable));
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
    else if ((mType = Type2) || (mType == Type3)) {
        if (mIsEnable) {
            setCursor(Qt::PointingHandCursor);
            mLabel->setStyleSheet("background-color:#cc3033; font: blod; color: rgb(255, 255, 255); border-radius: 6px;");
        } else {
            setCursor(Qt::ArrowCursor);
            mLabel->setStyleSheet("background-color:#696969; font: blod; color: rgb(255, 255, 255); border-radius: 6px;");
        }
    }
    else if (mType == Type4) {
        setCursor ((mIsEnable ? Qt::PointingHandCursor : Qt::ArrowCursor));
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

void PushButton::setType(PushButton::Type type)
{
    mType = type;
    switch (mType) {
        case Type1: {
            mPLine->show();
            mLabel->setStyleSheet("color: rgb(0, 0, 0);");
            break;
        }
        case Type2: {
            mPLine->hide();
            mLabel->setStyleSheet("font: blod; color: rgb(255, 255, 255); border-radius: 6px;");
            setEnable();
            break;
        }
        case Type3: {
            mPLine->hide();
            mLabel->setStyleSheet("font: blod; color: rgb(204, 48, 51); border-radius: 6px;");
            setEnable();
            break;
        }
        case Type4: {
            mPLine->hide();
            setEnable();
            mLabel->setStyleSheet("border: 1px solid; border-color: #a8a8a8a8; border-radius: 6px;");
            break;
        }
    }
}
