//
// Created by dingjing on 23-6-13.
//

#include "main-widget-1.h"

#include <QLabel>
#include <QComboBox>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAbstractItemView>

#include "push-button.h"
#include "view/header-view.h"
#include "view/scanner-view.h"
#include "model/scanner-task-model.h"
#include "scanner-task-delegate.h"


MainWidget1::MainWidget1(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking (true);

    mMainLayout = new QVBoxLayout;
    auto l1 = new QHBoxLayout;
    auto l2 = new QHBoxLayout;
    auto l3 = new QHBoxLayout;

    auto lb1 = new QLabel("任务：");
    mComboBox = new QComboBox;
    mComboBox->setStyleSheet ("QComboBox{combobox-popup:0}");
    mComboBox->setMaxVisibleItems (9);
    mComboBox->view()->setAutoScroll (true);
    mComboBox->setMinimumWidth (300);
    mComboBox->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto btn1 = new PushButton(nullptr, PushButton::Type3);
    btn1->setEnable(true);
    btn1->setText ("导入");

    l1->addWidget (lb1);
    l1->addWidget (mComboBox);
    l1->addWidget (btn1);
    l1->addStretch ();

    auto btn21 = new PushButton(nullptr, PushButton::Type3);
    btn21->setEnable(false);
    btn21->setText ("删除");

    auto btn22 = new PushButton(nullptr, PushButton::Type3);
    btn22->setEnable(false);
    btn22->setText ("导出");

    mView = new ScannerView;
    mModel = new ScannerTaskModel;

    mView->setItemDelegate (new ScannerTaskDelegate(this));

    mView->setModel (mModel);
    mView->setHorizontalHeader (new HeaderView(Qt::Horizontal));

    auto lb = new QLabel("版权所有、版本号、咨询电话以及官网信息");
    l3->addStretch ();
    l3->addWidget (lb);
    l3->addStretch ();

    l2->addStretch ();
    l2->addWidget (btn21);
    l2->addSpacing (10);
    l2->addWidget (btn22);

    mMainLayout->addLayout (l1);
    mMainLayout->addSpacing (20);
    mMainLayout->addLayout (l2);
    mMainLayout->addWidget (mView);
    mMainLayout->addLayout (l3);

    setLayout (mMainLayout);

    mView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mView->setSelectionMode (QAbstractItemView::SingleSelection);
    mView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    mView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mView->verticalHeader()->setDefaultSectionSize (50);
    mView->horizontalHeader()->setMinimumSectionSize(30);

    mView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    mView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    mView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    mView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    mView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    mView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    mView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    mView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    mView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Fixed);

    mView->horizontalHeader()->setSectionsClickable (false);

#if 1
    // test
    mModel->test();
#endif
}

void MainWidget1::resizeEvent(QResizeEvent *event)
{
    if (!mView || !mView->horizontalHeader()) return;

    mView->horizontalHeader()->resizeSection (0, mItemCheckSize);
    mView->horizontalHeader()->resizeSection (1, mItemIdxSize);
    mView->horizontalHeader()->resizeSection (5, mItemStartTimeSize);
    mView->horizontalHeader()->resizeSection (6, mItemStatusSize);
    mView->horizontalHeader()->resizeSection (7, mItemProgressSize);
    mView->horizontalHeader()->resizeSection (8, mItemDetailSize);

    int w = mView->width()      \
        - mItemCheckSize        \
        - mItemIdxSize          \
        - mItemStartTimeSize    \
        - mItemStatusSize       \
        - mItemProgressSize     \
        - mItemDetailSize       \
        - contentsMargins().left()  \
        - contentsMargins().right() \
        - 20;

    mView->horizontalHeader()->resizeSection (2, (int) (w * 0.3));
    mView->horizontalHeader()->resizeSection (3, (int) (w * 0.3));
    mView->horizontalHeader()->resizeSection (4, (int) (w * 0.4));
}

