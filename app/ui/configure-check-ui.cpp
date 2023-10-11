//
// Created by dingjing on 23-10-10.
//

#include "configure-check-ui.h"

#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include "../db/configure-report.h"

#define SOFTWARE_TITLE              "配置检查 ----------------------------------------------------------------------- %1项"

ConfigureCheckUI::ConfigureCheckUI(Type type, QWidget *parent)
    : QWidget (parent), mIsChecked(false), mType(type)
{
    mMainLayout = new QVBoxLayout;
    auto titleLayout = new QHBoxLayout;

    mTitle = new QLabel;
    titleLayout->addWidget (mTitle);

    auto f = qApp->font();
    f.setPointSizeF (f.pointSizeF() - 2);
    QFontMetrics fm(f);
    static const int pSize = fm.width (SOFTWARE_TITLE);
    mTitle->setMinimumWidth (pSize + 100);

    mShowDetail = new QPushButton;
    mShowDetail->setFlat (true);
    titleLayout->addWidget (mShowDetail);
    titleLayout->addStretch ();

    mMainLayout->addLayout (titleLayout);

    {
        mDetailWidget = new QWidget;
        mMainLayout->addWidget (mDetailWidget);
    }

    setLayout (mMainLayout);

    connect (this, &ConfigureCheckUI::stop, this, [=] () -> void {
        ConfigureReport::getInstance()->stop();
        Q_EMIT reset();
    });

    connect (this, &ConfigureCheckUI::pause, this, [=] () -> void {
        ConfigureReport::getInstance()->pause();
    });

    connect (this, &ConfigureCheckUI::start, this, [=] () -> void {
        ConfigureReport::getInstance()->start();
    });

    connect (this, &ConfigureCheckUI::reset, this, [=] () -> void {
        ConfigureReport::getInstance()->reset();
        for (auto& i : mItemWidget) {
            mMainLayout->removeWidget (i.get());
        }
        mItemWidget.clear();
    });

    connect (this, &ConfigureCheckUI::updateItemCount, this, [=] (int count) {
        mTitle->setText (QString(SOFTWARE_TITLE).arg (count));
    });

    connect (mShowDetail, &QPushButton::clicked, this, [=] (bool) {
        mIsChecked = !mIsChecked;
        mShowDetail->toggled (mIsChecked);
    });
    connect (mShowDetail, &QPushButton::toggled, this, [=] (bool checked) {
        QPixmap pix((!checked ? ":/data/down.png" : ":/data/up.png"));
        mShowDetail->setIcon (pix);
        if (checked) {
            mDetailWidget->show();
        }
        else {
            mDetailWidget->hide();
        }
        Q_EMIT resizeUI();
    });

    Q_EMIT updateItemCount ();
    Q_EMIT mShowDetail->toggled (mIsChecked);
}

int ConfigureCheckUI::getSuccessItem() const
{
    return ((mSuccessItem > 0) ? mSuccessItem : 0);
}

int ConfigureCheckUI::getWarningItem() const
{
    return ((mWarningItem > 0) ? mWarningItem : 0);
}
