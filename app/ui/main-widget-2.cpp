//
// Created by dingjing on 23-6-13.
//

#include "main-widget-2.h"

#include <QLabel>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QApplication>

#include "push-button.h"
#include "utils/tools.h"
#include "db/scan-task-db.h"
#include "view/result-view.h"
#include "model/result-model.h"
#include "model/scanner-task-item.h"


MainWidget2::MainWidget2(QWidget *parent)
    : QWidget (parent)
{
    setMouseTracking (true);

    auto font1 = qApp->font();
    font1.setPointSizeF (font1.pointSizeF() + 3);

    auto font2 = qApp->font();
    font2.setPointSizeF (font2.pointSizeF() - 1);

    mMainLayout = new QVBoxLayout;
    auto l1 = new QHBoxLayout;
    auto l2 = new QHBoxLayout;
    auto l21 = new QVBoxLayout; // 左侧主要布局
    auto l210 = new QHBoxLayout;
    auto l211 = new QHBoxLayout;
    auto l212 = new QHBoxLayout;
    auto l213 = new QHBoxLayout;
    auto l22 = new QVBoxLayout;
    auto l3 = new QHBoxLayout;

    l2->setContentsMargins (3, 3, 3, 3);
    l211->setContentsMargins (8, 0, 3, 0);
    l212->setContentsMargins (8, 0, 3, 0);
    l213->setContentsMargins (8, 0, 3, 0);

    auto btn1 = new PushButton(nullptr, PushButton::Type3);
    btn1->setEnable(true);
    btn1->setText ("返回");

//    auto ts = new TaskTrace;
//    l22->addWidget (ts);

    auto lb1 = new QLabel("基本信息");
    lb1->setFont (font1);
//    auto lb12 = new QLabel("已完成");
//    lb12->setFont (font2);
    l210->addWidget (lb1);
    l210->addStretch ();
//    l210->addWidget (lb12);

    mDW = new QLabel("单位名称: 中国信息安全测评中心");
    mDW->setFont (font2);
    mTN = new QLabel("任务名称：电信行业扫描任务");
    mTN->setFont (font2);
    mCL = new QLabel("应用策略：策略名称1;策略名称2;策略名称3");
    mCL->setFont (font2);
    l211->addWidget (mDW);
    l211->addWidget (mTN);
    l211->addWidget (mCL);

    auto lbb1 = new QLabel("检查结果");
    lbb1->setFont (font1);

    auto lbb21 = new QLabel(QString("主机名: %1").arg (getLocalPCName()));
    lbb21->setFont (font2);
    auto lbb22 = new QLabel(QString("IP地址：%1").arg (getLocalIP()));
    lbb22->setFont (font2);
    auto lbb23 = new QLabel(QString("MAC地址：%1").arg (getLocalMAC()));
    lbb23->setFont (font2);
    l212->addWidget (lbb21);
    l212->addWidget (lbb22);
    l212->addWidget (lbb23);

    mSP = new QLabel("检查路径: /data/code");
    mSP->setFont (font2);
    mSNP = new QLabel("例外路径：/home/test");
    mSNP->setFont (font2);
    mST = new QLabel("开始时间：2023-06-06 11:11:11");
    mST->setFont (font2);
    l213->addWidget (mSP);
    l213->addWidget (mSNP);
    l213->addWidget (mST);

    mView = new ResultView;
    mModel = new ResultModel;
    mView->setModel (mModel);

    auto lb = new QLabel("版权所有、版本号、咨询电话以及官网信息");

    l1->addWidget (btn1);
    l1->addStretch ();

    l21->addLayout (l210);
    l21->addLayout (l211);
    l21->addWidget (lbb1);
    l21->addLayout (l212);
    l21->addLayout (l213);
    l21->addWidget (mView);
    l2->addLayout (l21);
    l2->addLayout (l22);

    l3->addStretch ();
    l3->addWidget (lb);
    l3->addStretch ();

    mMainLayout->addLayout (l1);
    mMainLayout->addLayout (l2);
    mMainLayout->addLayout (l3);

    setLayout (mMainLayout);

    mView->horizontalHeader()->setSectionsClickable (false);
    mView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mView->setSelectionMode (QAbstractItemView::SingleSelection);
//    mView->horizontalHeader()->setSelectionMode (QHeaderView::SingleSelection);

    connect (btn1, qOverload<>(&PushButton::clicked), this, qOverload<>(&MainWidget2::showTaskList));

#if 1
    mModel->test();
#endif
}

void MainWidget2::resizeEvent(QResizeEvent *event)
{
    if (!mView || !mView->horizontalHeader()) return;

    mView->horizontalHeader()->resizeSection (0, mItemIdxSize);
    mView->horizontalHeader()->resizeSection (3, mItemTimeSize);

    int w = mView->width()          \
        - mItemIdxSize              \
        - mItemTimeSize             \
        - contentsMargins().left()  \
        - contentsMargins().right() \
        - 23;

    mView->horizontalHeader()->resizeSection (1, (int) (w * 0.4));
    mView->horizontalHeader()->resizeSection (2, (int) (w * 0.6));

    QWidget::resizeEvent (event);
}

void MainWidget2::showResult(const QString &taskID)
{
    mTaskID = taskID;
#define DW  "单位名称："
#define TN  "任务名称："
#define CL  "应用策略："
#define SP  "检查路径："
#define SNP "例外路径："
#define ST  "开始时间："
    ScanTaskDB* sc = ScanTaskDB::getInstance();
    auto item = sc->getItemByIndex (sc->getRowByItemID (taskID));
    if (mTaskID.isNull() || mTaskID.isEmpty() || !item) {
        mDW->setText (QString("%1%2").arg (DW, "中国信息安全测评中心"));
        mTN->setText (TN);
        mCL->setText (CL);
        mSP->setText (SP);
        mSNP->setText (SNP);
        mST->setText (ST);
        show();
        return;
    }

    mDW->setText (QString("%1%2").arg (DW, "中国信息安全测评中心"));
    mTN->setText (QString("%1%2").arg (TN, item->getName()));
    mCL->setText (QString("%1%2").arg (CL, item->getPolicy().join (";")));
    mSP->setText (QString("%1%2").arg (SP, item->getScanPath()));
    mSNP->setText (QString("%1%2").arg (SNP, item->getScanOutPath()));
    mST->setText (QString("%1%2").arg (ST, item->getStartTimeStr()));

    show ();
}

void MainWidget2::hideResult()
{
    mTaskID = "";

    hide();
}
