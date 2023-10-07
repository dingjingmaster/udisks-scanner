//
// Created by dingjing on 23-10-7.
//

#include "hardware-ui.h"

#include <QLabel>
#include <QDebug>
#include <QScrollBar>
#include <QPushButton>
#include <QHeaderView>

#define SOFTWARE_TITLE              "硬件检查 ----------------------------------------------------------------------- %1项"

HardwareUI::HardwareUI(QWidget *parent)
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

    mDetailWidget = new QWidget;
    auto gridLayout = new QGridLayout;

    auto timeLabel = new QLabel("当前日期与时间:");
    mDate = new QLabel;
    gridLayout->addWidget (timeLabel,   0,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget (mDate,       0,  3, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);

    auto computerLabel = new QLabel("计算机名:");
    mComputerName = new QLabel;
    gridLayout->addWidget (computerLabel,   1,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget (mComputerName,   1,  3, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);

    auto operationSystemName = new QLabel("操作系统:");
    mOperationSystemName = new QLabel;
    gridLayout->addWidget (operationSystemName,   2,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget (mOperationSystemName,  2,  3, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);

    auto language = new QLabel("语言:");
    mLanguage = new QLabel;
    gridLayout->addWidget (language,   3,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget (mLanguage,  3,  3, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);

    auto manufacturer = new QLabel("系统制造商:");
    mManufacturer = new QLabel;
    gridLayout->addWidget (manufacturer,   4,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget (mManufacturer,  4,  3, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);

    auto systemVersion = new QLabel("系统版本:");
    mSystemVersion = new QLabel;
    gridLayout->addWidget (systemVersion,   5,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget (mSystemVersion,  5,  3, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);

    auto bios = new QLabel("BIOS:");
    mBIOS = new QLabel;
    gridLayout->addWidget (bios,   6,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget (mBIOS,  6,  3, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);

    auto cpu = new QLabel("处理器:");
    mCPU = new QLabel;
    gridLayout->addWidget (cpu,   7,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget (mCPU,  7,  3, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);

    auto mem = new QLabel("内存:");
    mMem = new QLabel;
    gridLayout->addWidget (mem,   8,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget (mMem,  8,  3, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);

    auto swap = new QLabel("页面文件:");
    mSwap = new QLabel;
    gridLayout->addWidget (swap,   9,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget (mSwap,  9,  3, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);

    auto desktop = new QLabel("桌面版本:");
    mDesktop = new QLabel;
    gridLayout->addWidget (desktop,   10,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget (mDesktop,  10,  3, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);


    mMainLayout->addLayout (titleLayout);
    mDetailWidget->setLayout (gridLayout);
    mMainLayout->addWidget(mDetailWidget);

    setLayout (mMainLayout);

    connect (this, &HardwareUI::updateItemCount, this, [=] (int count) {
        mTitle->setText (QString(SOFTWARE_TITLE).arg (count));
    });

    connect (this, &HardwareUI::load, this, [=] () {
        // FIXME://
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
    });



    Q_EMIT updateItemCount();
    Q_EMIT mShowDetail->toggled(mIsChecked);
}

void HardwareUI::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent (event);
}
