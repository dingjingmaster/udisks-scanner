//
// Created by dingjing on 23-6-13.
//

#include "main-widget-2.h"

#include <QLabel>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QApplication>

#include "task-trace.h"
#include "push-button.h"
#include "view/result-view.h"
#include "model/result-model.h"


MainWidget2::MainWidget2(QWidget *parent)
    : QWidget (parent)
{
    setMouseTracking (true);

    auto font1 = qApp->font();
    font1.setPointSizeF (font1.pointSizeF() + 3);

    auto font2 = qApp->font();
    font2.setPointSizeF (font2.pointSizeF() - 2);

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

    auto lb21 = new QLabel("单位名称: 中国信息安全测评中心AAA");
    lb21->setFont (font2);
    auto lb22 = new QLabel("任务名称：电信行业扫描任务");
    lb22->setFont (font2);
    auto lb23 = new QLabel("应用策略：策略名称1;策略名称2;策略名称3");
    lb23->setFont (font2);
    l211->addWidget (lb21);
    l211->addWidget (lb22);
    l211->addWidget (lb23);

    auto lbb1 = new QLabel("检查结果");
    lbb1->setFont (font1);

    auto lbb21 = new QLabel("主机名: ZRD");
    lbb21->setFont (font2);
    auto lbb22 = new QLabel("IP地址：192.168.30.23");
    lbb22->setFont (font2);
    auto lbb23 = new QLabel("MAC地址：08-5F-A3-C5-F5-F1");
    lbb23->setFont (font2);
    l212->addWidget (lbb21);
    l212->addWidget (lbb22);
    l212->addWidget (lbb23);

    auto lbb24 = new QLabel("检查路径: /data/code");
    lbb24->setFont (font2);
    auto lbb25 = new QLabel("例外路径：/home/test");
    lbb25->setFont (font2);
    auto lbb26 = new QLabel("开始时间：2023-06-06 11:11:11");
    lbb26->setFont (font2);
    l213->addWidget (lbb24);
    l213->addWidget (lbb25);
    l213->addWidget (lbb26);

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
