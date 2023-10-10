//
// Created by dingjing on 23-10-10.
//

#include "configure-check-ui.h"

#include <QLabel>
#include <QPushButton>
#include <QApplication>

#define SOFTWARE_TITLE              "配置检查 ----------------------------------------------------------------------- %1项"

ConfigureCheckUI::ConfigureCheckUI(QWidget *parent)
    : QWidget (parent), mIsChecked(false)
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
