//
// Created by dingjing on 23-9-27.
//

#include "main-widget-21.h"

#include <QLabel>
#include <QDebug>
#include <QDateTime>
#include <QScrollArea>
#include <QPushButton>
#include <QResizeEvent>
#include <QProgressBar>
#include <QApplication>

#include <glib.h>

#include "push-button.h"
#include "software-ui.h"
#include "hardware-ui.h"
#include "../utils/tools.h"



MainWidget21::MainWidget21(QWidget *parent)
    : QWidget (parent)
{
    setContentsMargins (0, 0, 0, 0);

    auto f = qApp->font();
    f.setPointSizeF (f.pointSizeF() - 2);
    QFontMetrics fm(f);
    static const int pSize = fm.width (" 100.0 % ");
    static const int sSize = fm.width (" 啊啊啊啊啊啊啊啊啊 ");

    auto mainLayout = new QVBoxLayout;
    auto btnLayout = new QHBoxLayout;
    auto scanUILayout = new QVBoxLayout;
    auto progressLayout = new QHBoxLayout;

    auto progressLabel = new QLabel;
    auto progress = new QProgressBar;
    auto progressStatusLabel = new QLabel;

    mDelBtn = new PushButton;
    mExpBtn = new PushButton;
    mPauBtn = new PushButton;
    mStpBtn = new PushButton;
    mChkBtn = new PushButton;

    // 修改状态
    mDelBtn->setType(PushButton::Type2);
    mExpBtn->setType(PushButton::Type2);
    mPauBtn->setType(PushButton::Type2);
    mStpBtn->setType(PushButton::Type2);
    mChkBtn->setType(PushButton::Type2);

    mDelBtn->setText ("删除");
    mExpBtn->setText ("导出");
    mPauBtn->setText ("暂停");
    mStpBtn->setText ("停止");
    mChkBtn->setText ("一键检查");

    mDelBtn->setMinimumWidth (160);
    mExpBtn->setMinimumWidth (160);
    mPauBtn->setMinimumWidth (160);
    mStpBtn->setMinimumWidth (160);
    mChkBtn->setMinimumWidth (160);

    btnLayout->addWidget (mDelBtn);
    btnLayout->addWidget (mExpBtn);
    btnLayout->addWidget (mPauBtn);
    btnLayout->addWidget (mStpBtn);
    btnLayout->addWidget (mChkBtn);
    btnLayout->addStretch ();
    mainLayout->addLayout (btnLayout);

    progress->setRange (0, 100);
    progress->setFixedHeight (10);
    progress->setTextVisible (false);
    progressLabel->setFixedWidth (pSize);
    progressStatusLabel->setFixedWidth (sSize);

    progressLayout->setSpacing (9);
    progressLayout->addWidget (progress);
    progressLayout->addWidget (progressLabel);
    progressLayout->addWidget (progressStatusLabel);

    mainLayout->addLayout (progressLayout);

    // base info
    mBaseInfoLabel = new QLabel;
    mBaseInfoLabel->setTextFormat (Qt::RichText);
    mainLayout->addWidget (mBaseInfoLabel);

    // 检查结果
    auto result = new QLabel;
    result->setText ("<h3>检查结果</h3>");
    result->setTextFormat (Qt::RichText);
    mainLayout->addSpacing (12);
    mainLayout->addWidget (result);

    mScrollWidget = new QWidget;
    mScrollArea = new QScrollArea;
//    mScrollWidget->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Preferred);

    mHardwareUI = new HardwareUI;
    scanUILayout->addWidget (mHardwareUI);

    mSoftwareUI = new SoftwareUI;
    scanUILayout->addWidget (mSoftwareUI);
    scanUILayout->addStretch ();

    mScrollWidget->setLayout (scanUILayout);
    mScrollArea->setWidget (mScrollWidget);

    mainLayout->addWidget(mScrollArea);

    mainLayout->addStretch ();
    setLayout (mainLayout);

    connect (progress, &QProgressBar::valueChanged, this, [=] (int val) {
        auto pec = float (val) / (float) (progress->maximum() - progress->minimum());
        pec = ((pec < 0) ? 0 : ((pec > 1) ? 1 : pec));
        progressLabel->setText((pec == 0) ? "" : QString("%1%").arg (pec * 100, 0, 'f', 1));
        if (pec > 0 && pec < 1) {
            progressStatusLabel->setText ("检查中");
        }
        else if (pec == 1) {
            progressStatusLabel->setText ("<red>已完成</red>");
            // NOTE://
        }
        else {
            progressStatusLabel->setText ("");
        }
        progress->setValue (qRound(pec * 100));
    });

    connect (mHardwareUI, &HardwareUI::resizeUI, this, &MainWidget21::resizeResultUI);
    connect (mSoftwareUI, &SoftwareUI::resizeUI, this, &MainWidget21::resizeResultUI);

    connect (mChkBtn, &PushButton::clicked, this, [&] () {
        changeStatus (Running);
    });
    connect (mStpBtn, &PushButton::clicked, this, [&] () {
        changeStatus (Stop);
    });

    connect (mPauBtn, &PushButton::clicked, this, [&] () {
        changeStatus (Pause);
    });
    connect (this, &MainWidget21::allFinished, this, [&] () {
        changeStatus (Finished);
    });

    // init
    Q_EMIT progress->valueChanged (90);
    updateBaseInfo ();
    changeStatus (Stop);
}

void MainWidget21::updateBaseInfo(qint64 startTime, qint64 stopTime)
{
    static qint64 startTimeS = 0;
    static qint64 stopTimeS = 0;

    if (startTime > 0) {
        startTimeS = startTime;
    }

    if (stopTime > 0) {
        stopTimeS = stopTime;
    }

    mBaseInfoLabel->setText (QString("<h3>基本信息</h3>"
                                     "主机名: %1 &nbsp;&nbsp;&nbsp;&nbsp;任务名称: %2 &nbsp;&nbsp;&nbsp;&nbsp;IP地址: %3 &nbsp;&nbsp;&nbsp;&nbsp;开始时间: %4 &nbsp;&nbsp;&nbsp;&nbsp;结束时间: %5")
                                     .arg (g_get_host_name(), "脆弱性检查任务", getLocalIP(),
                                           (startTimeS > 0) ? QDateTime::fromSecsSinceEpoch (startTimeS).toString ("yyyy-MM-dd hh:mm:ss") : "",
                                           (stopTimeS > 0) ? QDateTime::fromSecsSinceEpoch (startTimeS).toString ("yyyy-MM-dd hh:mm:ss") : "")
                                     );
}

void MainWidget21::changeStatus(MainWidget21::Status status)
{
    if ((mStatus == status) && (mStatus != Pause)) return;

    if ((mStatus == Pause) && (status == Pause)) {
        mStatus = Running;
        mPauBtn->setText ("暂停");
    }
    else {
        mStatus = status;
        if (mStatus == Pause) { mPauBtn->setText ("开始"); }
    }

    switch (mStatus) {
        case Stop: {
            mDelBtn->setEnable (false);
            mExpBtn->setEnable (false);
            mPauBtn->setEnable (false);
            mStpBtn->setEnable (false);
            mChkBtn->setEnable (true);
            mSoftwareUI->stop();
            mHardwareUI->stop();
            break;
        }
        case Running: {
            mDelBtn->setEnable (false);
            mExpBtn->setEnable (false);
            mPauBtn->setEnable (true);
            mStpBtn->setEnable (true);
            mChkBtn->setEnable (false);
//            mSoftwareUI->start();
            mHardwareUI->start();
            break;
        }
        case Pause: {
            mDelBtn->setEnable (false);
            mExpBtn->setEnable (false);
            mPauBtn->setEnable (true);
            mStpBtn->setEnable (false);
            mChkBtn->setEnable (false);
            mSoftwareUI->pause();
            mHardwareUI->pause();
            break;
        }
        case Finished: {
            mDelBtn->setEnable (true);
            mExpBtn->setEnable (true);
            mPauBtn->setEnable (false);
            mStpBtn->setEnable (false);
            mChkBtn->setEnable (true);
            break;
        }
        default: {
            mStatus = Stop;
        }
    }
}

void MainWidget21::run()
{

}

void MainWidget21::resizeEvent(QResizeEvent* ev)
{
    resizeResultUI();
    QWidget::resizeEvent (ev);
}

void MainWidget21::resizeResultUI()
{
    auto diffW = 20 + contentsMargins().left() + contentsMargins().right()
                 + mScrollWidget->contentsMargins().left() + mScrollWidget->contentsMargins().right();
    auto w = size().width() - diffW;
    mScrollWidget->setFixedWidth(w - 20);
    mScrollArea->setFixedWidth(w);
//    qDebug() << "w: " << w << " -- " << diffW;

    auto diffH = 240;
    auto h = size().height() - diffH;
    mScrollArea->setMinimumHeight(h + 10);
//    qDebug() << "h: " << h << " -- " << diffH;

    auto hContent = mSoftwareUI->getHeight() + mHardwareUI->getHeight();
    qDebug() << "c h 1: " << hContent;
    hContent = ((hContent < h) ? h : hContent);
    mScrollWidget->setMinimumHeight(hContent);
    qDebug() << "c h 2: " << hContent;
}
