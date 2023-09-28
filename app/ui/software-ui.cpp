//
// Created by dingjing on 23-9-27.
//

#include "software-ui.h"

#include <QLabel>
#include <QDebug>
#include <QScrollBar>
#include <QHeaderView>

#include "../view/software-view.h"
#include "../model/software-item.h"
#include "../model/software-model.h"

#define SOFTWARE_TITLE              "软件检查--------------------------------------------%1项"

SoftwareUI::SoftwareUI(QWidget *parent)
    : QWidget (parent)
{
    mMainLayout = new QVBoxLayout;
    auto titleLayout = new QHBoxLayout;

    mTitle = new QLabel;
    titleLayout->addWidget (mTitle);

    mView = new SoftwareView;
    mModel = new SoftwareModel;

    mView->setModel (mModel);

    mMainLayout->addLayout (titleLayout);
    mMainLayout->addWidget (mView);

    setLayout (mMainLayout);

    connect (this, &SoftwareUI::updateItemCount, this, [=] () {
        mTitle->setText (QString(SOFTWARE_TITLE).arg (mModel->rowCount ()));
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
}
