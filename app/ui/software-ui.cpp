//
// Created by dingjing on 23-9-27.
//

#include "software-ui.h"

#include <QLabel>
#include <QDebug>
#include <QScrollBar>
#include <QPushButton>
#include <QHeaderView>

#include "../view/software-view.h"
#include "../model/software-item.h"
#include "../model/software-model.h"

#define SOFTWARE_TITLE              "软件检查 ----------------------------------------------------------------------- %1项"

SoftwareUI::SoftwareUI(QWidget *parent)
    : QWidget (parent), mIsChecked(false)
{
    mMainLayout = new QVBoxLayout;
    auto titleLayout = new QHBoxLayout;

    mTitle = new QLabel;
    titleLayout->addWidget (mTitle);

    mShowDetail = new QPushButton;
    mShowDetail->setFlat (true);
    titleLayout->addWidget (mShowDetail);
    titleLayout->addStretch ();

    mView = new SoftwareView;
    mModel = new SoftwareModel;

    mView->setModel (mModel);

    mMainLayout->addLayout (titleLayout);
    mMainLayout->addWidget (mView);

    setLayout (mMainLayout);

    connect (this, &SoftwareUI::updateItemCount, this, [=] () {
        mTitle->setText (QString(SOFTWARE_TITLE).arg (mModel->rowCount ()));
    });

    connect (mShowDetail, &QPushButton::clicked, this, [=] (bool b) {
        mIsChecked = !mIsChecked;
        mShowDetail->toggled (mIsChecked);
    });
    connect (mShowDetail, &QPushButton::toggled, this, [=] (bool checked) {
        QPixmap pix((!checked ? ":/data/down.png" : ":/data/up.png"));
        mShowDetail->setIcon (pix);
        if (checked) mView->show();
        else mView->hide();
    });

    connect (mModel, &SoftwareModel::updateView, this, [=] () -> void {
        if (!mModel || !mView)return;
        auto s = mView->verticalScrollBar();
        auto scrollBarRatio = float(s->value()) / (s->maximum() - s->minimum());
        auto curItemIndex = scrollBarRatio * mModel->rowCount();
        auto startIndex = ((curItemIndex - 30) >= 0) ? (curItemIndex - 30) : 0;
        auto stopIndex = ((curItemIndex + 30) < mModel->rowCount()) ? (curItemIndex + 30) : mModel->rowCount();

        // 开始更新
        for (auto i = startIndex; i <= stopIndex; ++i) {
            mView->update(mModel->index(i, 0));
            mView->update(mModel->index(i, 1));
            mView->update(mModel->index(i, 2));
            mView->update(mModel->index(i, 3));
            mView->update(mModel->index(i, 4));
            mView->update(mModel->index(i, 5));
            mView->update(mModel->index(i, 6));
            mView->update(mModel->index(i, 7));
            mView->update(mModel->index(i, 8));
        }
        Q_EMIT updateItemCount();
    });

    connect (this, &SoftwareUI::load, mModel, &SoftwareModel::loadApps);

    mView->horizontalHeader()->setSectionsClickable (false);
    mView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mView->setSelectionMode (QAbstractItemView::SingleSelection);
    Q_EMIT updateItemCount();
    Q_EMIT mShowDetail->toggled(mIsChecked);
}

void SoftwareUI::resizeEvent(QResizeEvent *event)
{
    if (!mView || !mView->horizontalHeader()) return;

    mView->horizontalHeader()->resizeSection (0, mItemIdxWidth);
    mView->horizontalHeader()->resizeSection (1, mItemCategoryWidth);
    mView->horizontalHeader()->resizeSection (4, mItemInstallTimeWidth);
    mView->horizontalHeader()->resizeSection (5, mItemInstallPathWidth);

    int w = mView->width()      \
        - mItemIdxWidth         \
        - mItemCategoryWidth    \
        - mItemInstallTimeWidth \
        - mItemInstallPathWidth \
        - contentsMargins().left()  \
        - contentsMargins().right() \
        - 20;

    mView->horizontalHeader()->resizeSection (2, (int) (w * 0.6));
    mView->horizontalHeader()->resizeSection (3, (int) (w * 0.4));
}
