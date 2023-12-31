//
// Created by dingjing on 23-9-27.
//

#include "main-widget-21.h"

#include <QLabel>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QScrollArea>
#include <QPushButton>
#include <QResizeEvent>
#include <QProgressBar>
#include <QApplication>

#include <glib.h>
#include <QFileDialog>
#include <QStandardPaths>

#include "push-button.h"
#include "software-ui.h"
#include "hardware-ui.h"
#include "utils/tools.h"
#include "db/software-db.h"
#include "utils/export-xlsx.h"
#include "configure-check-ui.h"
#include "db/configure-report.h"
#include "vulnerability-check-ui.h"
#include "db/vulnerability-report.h"


#define SUCCESS_TIP         "以下 %1 项没有问题"
#define WARNING_TIP         "以下 %1 项有问题"

#define RESULT_TIP(val, icon, layout) \
    auto label ## val = new QWidget;                                            \
    auto ml ## val = new QHBoxLayout;                                           \
    auto icon ## val = new QLabel;                                              \
    auto tip ## val = new QLabel;                                               \
    icon ## val->setPixmap(QPixmap(":/data/" #icon ".png").scaled (32, 32));    \
    ml ## val->addWidget (icon ## val);                                         \
    ml ## val->addSpacing(8);                                                   \
    ml ## val->addWidget (tip ## val);                                          \
    ml ## val->addStretch ();                                                   \
    label ## val->setLayout (ml ## val);                                        \
    layout->addWidget (label ## val);                                           \


#define RESULT_SET_TIP(val, tipStr, numValue) \
    (tip ## val)->setText (QString(tipStr).arg (numValue));                     \


MainWidget21::MainWidget21(QWidget *parent)
    : QWidget (parent)
{
    setContentsMargins (0, 0, 0, 0);

    mProgressTimer = new QTimer;
    mProgressTimer->setInterval (1000);

    auto f = qApp->font();
    f.setPointSizeF (f.pointSizeF() - 2);
    QFontMetrics fm(f);
    static const int pSize = fm.width (" 100.0 % ") + 10;
    static const int sSize = fm.width (" 啊啊啊啊啊啊啊啊啊 ");

    auto mainLayout = new QVBoxLayout;
    auto btnLayout = new QHBoxLayout;
    auto scanUILayout = new QVBoxLayout;
    auto progressLayout = new QHBoxLayout;

    auto progressLabel = new QLabel;
    mProgress = new QProgressBar;
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

    mProgress->setRange (0, 1000);
    mProgress->setFixedHeight (10);
    mProgress->setTextVisible (false);
    progressLabel->setFixedWidth (pSize);
    progressStatusLabel->setFixedWidth (sSize);

    progressLayout->setSpacing (9);
    progressLayout->addWidget (mProgress);
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

    mScrollWidget1 = new QWidget;           // top
    mScrollWidget1->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);

    mScrollArea = new QScrollArea;
    mScrollArea->setWidgetResizable (true);
    mScrollArea->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Preferred);

    mScrollWidget2 = new QWidget;           // in
    mScrollWidget2->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto scrollWidget1L = new QVBoxLayout;

    // 以下xxx项有问题
    RESULT_TIP(Warning, warning, scanUILayout)

    mVulnerabilityUIWARN = new VulnerabilityCheckUI(VulnerabilityCheckUI::Warning);
    scanUILayout->addWidget (mVulnerabilityUIWARN);

    mConfigureUIWARN = new ConfigureCheckUI(ConfigureCheckUI::Warning);
    scanUILayout->addWidget (mConfigureUIWARN);

    // 以下xxx项没有问题
    RESULT_TIP(Success, success, scanUILayout)

    mVulnerabilityUIOK = new VulnerabilityCheckUI(VulnerabilityCheckUI::Success);
    scanUILayout->addWidget (mVulnerabilityUIOK);

    mConfigureUIOK = new ConfigureCheckUI(ConfigureCheckUI::Success);
    scanUILayout->addWidget (mConfigureUIOK);

    mHardwareUI = new HardwareUI;
    scanUILayout->addWidget (mHardwareUI);

    mSoftwareUI = new SoftwareUI;
    scanUILayout->addWidget (mSoftwareUI);
    scanUILayout->addStretch ();

    mScrollWidget2->setLayout (scanUILayout);
    mScrollArea->setWidget (mScrollWidget2);
    scrollWidget1L->addWidget (mScrollArea);
    mScrollWidget1->setLayout (scrollWidget1L);

    mainLayout->addWidget(mScrollWidget1);

    setLayout (mainLayout);

    auto configDB = ConfigureReport::getInstance();
    connect (configDB, &ConfigureReport::addItem, this, [=] (QString& name) {
        auto item = configDB->getItemByKey(name);
        g_return_if_fail(nullptr != item);
        Q_EMIT mConfigureUIWARN->addItem (item);
        Q_EMIT mConfigureUIOK->addItem (item);
        Q_EMIT updateCheckResult ();
    });

    auto vulnerabilityDB = VulnerabilityReport::getInstance();
    connect (vulnerabilityDB, &VulnerabilityReport::addItem, this, [=] (QString& name) {
        auto item = vulnerabilityDB->getItemByKey(name);
        g_return_if_fail(nullptr != item);
        Q_EMIT mVulnerabilityUIOK->addItem (item);
        Q_EMIT mVulnerabilityUIWARN->addItem (item);
        Q_EMIT updateCheckResult ();
    });

    connect (this, &MainWidget21::updateCheckResult, this, [=] (int success, int warning) ->void {
        mSuccessItem = (mHardwareUI->getSuccessItem()
            + mSoftwareUI->getSuccessItem()
            + (mConfigureUIWARN->getSuccessItem() + mConfigureUIOK->getSuccessItem())
            + (mVulnerabilityUIWARN->getSuccessItem() + mVulnerabilityUIOK->getSuccessItem()));
        mWarningItem = (mHardwareUI->getWarningItem()
            + mSoftwareUI->getWarningItem()
            + (mConfigureUIWARN->getWarningItem() + mConfigureUIOK->getWarningItem())
            + (mVulnerabilityUIWARN->getWarningItem() + mVulnerabilityUIOK->getWarningItem()));

        RESULT_SET_TIP(Success, SUCCESS_TIP, mSuccessItem)
        RESULT_SET_TIP(Warning, WARNING_TIP, mWarningItem)
    });

    connect (mProgressTimer, &QTimer::timeout, this, [=] () -> void {
        if (Pause == mStatus) return;

        auto cur = mProgress->value();

        if (cur < mMinProgress) {
            cur = mMinProgress;
        }
        else if (cur >= mMaxProgress) {
            cur = mMaxProgress;
        }
        else {
            cur += 1;
        }

        if (Finished == mStatus) {
            mProgress->setValue (mProgress->maximum());
            mProgressTimer->stop();
        }
        else if (Stop == mStatus) {
            progressStatusLabel->setText ("");
            mProgressTimer->stop();
        }
        else {
            mProgress->setValue(cur);
        }
    });

    connect (mProgress, &QProgressBar::valueChanged, this, [=] (int val) ->void {
        auto pec = float (val) / (float) (mProgress->maximum() - mProgress->minimum());
        pec = ((pec < 0) ? 0 : ((pec > 1) ? 1 : pec));
        progressLabel->setText((pec == 0) ? "" : QString("%1%").arg (pec * 100, 0, 'f', 1));
        if (pec > 0 && pec < 1) {
            progressStatusLabel->setText ("检查中");
        }
        else if (pec == 1) {
            progressStatusLabel->setText ("已完成");
        }
        else {
            progressStatusLabel->setText ("");
        }
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

    connect (mDelBtn, &PushButton::clicked, this, [&] () {
        changeStatus (Stop);
    });

    connect (mExpBtn, &PushButton::clicked, this, [=] () {
        // 所有按钮失效

        // 开始导出
        QFileDialog dlg;
        dlg.setDefaultSuffix (".xlsx");
        dlg.setAcceptMode (QFileDialog::AcceptSave);
        dlg.setDirectory (QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        dlg.setFileMode (QFileDialog::AnyFile);
        if (QDialog::Accepted == dlg.exec()) {
            auto path = dlg.selectedFiles();
            qDebug() << "open path: " << path;
            if (path.isEmpty()) return;
            auto l = path.first();
            ExportXlsx exp;

            // 基础信息
            exp.addBaseInfo ("", "脆弱性检查", g_get_host_name(), getLocalIP(), getLocalMAC(),
                             (mStartTimeS > 0) ? QDateTime::fromSecsSinceEpoch (mStartTimeS).toString ("yyyy-MM-dd hh:mm:ss") : "",
                             (mStopTimeS > 0) ? QDateTime::fromSecsSinceEpoch (mStopTimeS).toString ("yyyy-MM-dd hh:mm:ss") : "",
                             QString("%1").arg (mSuccessItem + mWarningItem), QString("%1").arg (mSuccessItem), QString("%1").arg(mWarningItem),
                             mHardwareUI->getSystemName(), mHardwareUI->getLanguage(), mHardwareUI->getSystemManufacture(),
                             mHardwareUI->getSystemModel(), mHardwareUI->getBIOS(), mHardwareUI->getCPU(), mHardwareUI->getMem(),
                             mHardwareUI->getSwap(), mHardwareUI->getDesktop()
                             );

            // 漏洞检查
            int i = 0;
            auto vulnerabilityDB = VulnerabilityReport::getInstance();
            while (auto it = vulnerabilityDB->getItemByID (i)) {
                exp.addVnuInfo (QString("%1").arg (++i), it->getName(), it->getNameNumber(), it->getDesc(), it->getDate(), it->getUrl());
            }

            i = 0;
            auto configDB = ConfigureReport::getInstance();
            while (auto it = configDB->getItemByID (i)) {
                exp.configInfo(QString("%1").arg (++i), it->getCategory(), it->getName(), it->getLevel(), it->getCheckMethod(), it->getRepairMethod(), (it->getIsError() ? "不合格" : "合格"));
            }

            // 软件
            i = 0;
            auto softwareDB = SoftwareDB::getInstance();
            while (auto it = softwareDB->getItemByIndex(i)) {
                exp.softwareInfo(QString("%1").arg (++i), it->getCategory(), it->getName(), it->getVersion(), it->getInstallTime(), it->getInstallPath());
            }
            exp.save (l);
        }
    });

    connect (this, &MainWidget21::allFinished, this, [&] () {
        changeStatus (Finished);
    });

    connect (this, &MainWidget21::cancel, this, [=] () {
        mHardwareUI->stop();
        mSoftwareUI->stop();
        mConfigureUIOK->stop();
        mConfigureUIWARN->stop();
        mVulnerabilityUIOK->stop();
        mVulnerabilityUIWARN->stop();
        Q_EMIT mStpBtn->clicked();
    });

    // software ui -- 开始
    connect (mSoftwareUI, &SoftwareUI::stop,  SoftwareDB::getInstance(), &SoftwareDB::stop,  Qt::DirectConnection);
    connect (mSoftwareUI, &SoftwareUI::start, SoftwareDB::getInstance(), &SoftwareDB::start, Qt::DirectConnection);
    connect (mSoftwareUI, &SoftwareUI::pause, SoftwareDB::getInstance(), &SoftwareDB::pause, Qt::DirectConnection);
    //

    // init
    Q_EMIT updateCheckResult ();
    Q_EMIT mProgress->setValue(0);
    updateBaseInfo ();
    changeStatus (Stop);
}

void MainWidget21::updateBaseInfo(qint64 startTime, qint64 stopTime)
{
    if (startTime > 0) {
        mStartTimeS = startTime;
    }

    if (stopTime > 0) {
        mStopTimeS = stopTime;
    }

    if (startTime == 0 && stopTime == 0) {
        mStartTimeS = 0;
        mStopTimeS = 0;
    }

    mBaseInfoLabel->setText (QString("<h3>基本信息</h3>"
                                     "主机名: %1 &nbsp;&nbsp;&nbsp;&nbsp;任务名称: %2 &nbsp;&nbsp;&nbsp;&nbsp;IP地址: %3 &nbsp;&nbsp;&nbsp;&nbsp;开始时间: %4 &nbsp;&nbsp;&nbsp;&nbsp;结束时间: %5")
                                     .arg (g_get_host_name(), "脆弱性检查任务", getLocalIP(),
                                           (mStartTimeS > 0) ? QDateTime::fromSecsSinceEpoch (mStartTimeS).toString ("yyyy-MM-dd hh:mm:ss") : "",
                                           (mStopTimeS > 0) ? QDateTime::fromSecsSinceEpoch (mStopTimeS).toString ("yyyy-MM-dd hh:mm:ss") : "")
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
            mVulnerabilityUIWARN->stop();
            mVulnerabilityUIOK->stop();
            mConfigureUIWARN->stop();
            mConfigureUIOK->stop();
            mHardwareUI->stop();
            mSoftwareUI->stop();
            mHardwareUI->reset();
            mSoftwareUI->reset();
            mConfigureUIOK->reset();
            mConfigureUIWARN->reset();
            mVulnerabilityUIOK->reset();
            mVulnerabilityUIWARN->reset();
            mProgress->setValue(0);
            break;
        }
        case Running: {
            mDelBtn->setEnable (false);
            mExpBtn->setEnable (false);
            mPauBtn->setEnable (true);
            mStpBtn->setEnable (true);
            mChkBtn->setEnable (false);
            if (!mProgressTimer->isActive()) {
                mProgressTimer->start();
                mProgress->setValue(0);

                mSoftwareUI->reset();
                mConfigureUIOK->reset();
                mConfigureUIWARN->reset();
                mVulnerabilityUIOK->reset();
                mVulnerabilityUIWARN->reset();

                updateBaseInfo (0, 0);
                updateBaseInfo (QDateTime::currentSecsSinceEpoch());
            }
            mMinProgress = 0;
            mMaxProgress = 50;
            Q_EMIT mHardwareUI->start();
            mMinProgress = 50;
            mMaxProgress = 200;
            Q_EMIT mSoftwareUI->start();
            mMinProgress = 200;
            mMaxProgress = 400;
            mConfigureUIOK->start();
            mMinProgress = 400;
            mMaxProgress = 600;
            mConfigureUIWARN->start(false);
            mMinProgress = 600;
            mMaxProgress = 800;
            mVulnerabilityUIOK->start();
            qInfo() << "v ok finished!";
            mMinProgress = 800;
            mMaxProgress = 998;
            mVulnerabilityUIWARN->start(false);
            qInfo() << "v warn finished!";
            Q_EMIT allFinished();
            qInfo() << "all finished!";
            break;
        }
        case Pause: {
            mDelBtn->setEnable (false);
            mExpBtn->setEnable (false);
            mPauBtn->setEnable (true);
            mStpBtn->setEnable (false);
            mChkBtn->setEnable (false);
            mHardwareUI->pause();
            mSoftwareUI->pause();
            mConfigureUIOK->pause();
            mConfigureUIWARN->pause();
            mVulnerabilityUIOK->pause();
            mVulnerabilityUIWARN->pause();
            break;
        }
        case Finished: {
            mDelBtn->setEnable (true);
            mExpBtn->setEnable (true);
            mPauBtn->setEnable (false);
            mStpBtn->setEnable (false);
            mChkBtn->setEnable (true);
            updateBaseInfo (0, QDateTime::currentSecsSinceEpoch());
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
    return;
//    auto diffW = 20 + contentsMargins().left() + contentsMargins().right()
//                 + mScrollWidget->contentsMargins().left() + mScrollWidget->contentsMargins().right();
//    auto w = size().width() - diffW;
//    mScrollWidget->setFixedWidth(w - 20);
//    mScrollArea->setFixedWidth(w);
//
//    auto diffH = 240;
//    auto h = size().height() - diffH;
//    mScrollArea->setMinimumHeight(h + 10);
//
//    auto hContent = mSoftwareUI->getHeight() + mHardwareUI->getHeight();
//    hContent = ((hContent < h) ? h : hContent);
//    mScrollWidget->setMinimumHeight(hContent);
}

