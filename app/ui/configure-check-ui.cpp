//
// Created by dingjing on 23-10-10.
//

#include "configure-check-ui.h"

#include <QLabel>
#include <QDebug>
#include <QScrollBar>
#include <QScrollArea>
#include <QPushButton>
#include <QApplication>

#include "common/global.h"
#include "utils/process.h"
#include "db/configure-report.h"

#define SOFTWARE_TITLE              "配置检查 ----------------------------------------------------------------------- %1项"

ConfigureCheckUI::ConfigureCheckUI(Type type, QWidget *parent)
    : QWidget (parent), mIsChecked(false), mType(type)
{
    mProgress = new Process;
    mMainLayout = new QVBoxLayout;
    auto titleLayout = new QHBoxLayout;

    mProgress->setCommand(gConfigScannerPath);

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

    mDetailWidget = new QWidget;    // top
    auto detailLayout = new QVBoxLayout;
    mDetailWidget->setFixedHeight(360);
    mDetailWidget->setMaximumWidth (1200);
    mDetailWidget->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable (true);
    scrollArea->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto scrollWidget = new QWidget; // in
    scrollWidget->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto scrollLayout = new QVBoxLayout;

    scrollWidget->setLayout (scrollLayout);
    scrollArea->setWidget (scrollWidget);

    detailLayout->addWidget (scrollArea);
    mDetailWidget->setLayout (detailLayout);

    mMainLayout->addWidget (mDetailWidget);

    setLayout (mMainLayout);

    connect (this, &ConfigureCheckUI::stop, this, [=] () -> void {
        ConfigureReport::getInstance()->stop();
        Q_EMIT reset();
    });

    connect (this, &ConfigureCheckUI::pause, this, [=] () -> void {
        ConfigureReport::getInstance()->pause();
    });

    connect (this, &ConfigureCheckUI::start, this, [=] (bool b) -> void {
        if (b) {
            mProgress->start();
            while (!mProgress->isFinished()) {
                QApplication::processEvents ();
            };
            ConfigureReport::getInstance()->start();
        }
    });

    auto scrollBar = scrollArea->verticalScrollBar();
    scrollBar->setMinimum (0);
    connect (scrollBar, &QScrollBar::valueChanged, this, [=] (int value) {
        if (mShowedItem <= value) {
            lazyUpdate (value);
        }
    });

    connect (this, &ConfigureCheckUI::lazyUpdate, this, [=] (int c) -> void {
        auto& items = ((mType==Success) ? mSuccessItemWidget : mWaringItemWidget);
        if (0 == c) {
            if (mShowedItem > 0) {
                return;
            }
            else {
                int size = ((items.size() > 10) ? 10 : items.size());
                for (int i = 0; i < size; ++i) {
                    scrollLayout->addWidget (items[i].get());
                }
            }
        }
        else {
            int size = ((items.size() > (c + 10)) ? (c + 10) : items.size());
            for (int i = mShowedItem; i < size; ++i) {
                scrollLayout->addWidget (items[i].get());
            }
        }
    });

    connect (this, &ConfigureCheckUI::addItem, this, [=] (const std::shared_ptr<ConfigureItem>& item) -> void {
        auto itemUI = std::make_shared<ConfigureCheckItemUI>(
            item->getName(),
            item->getCategory(),
            item->getLevel(),
            item->getCheckMethod(),
            item->getRepairMethod());
        if ((mType == Success) && !item->getIsError()) {
            ++mSuccessItem;
            mSuccessItemWidget.append (itemUI);
            scrollBar->setMaximum (mSuccessItemWidget.size());
            Q_EMIT updateItemCount (mSuccessItem);
        }
        else if ((mType == Warning) && item->getIsError()) {
            ++mWarningItem;
            mWaringItemWidget.append (itemUI);
            scrollBar->setMaximum (mWaringItemWidget.size());
            Q_EMIT updateItemCount (mWarningItem);
        }
    });

    connect (this, &ConfigureCheckUI::reset, this, [=] () -> void {
        ConfigureReport::getInstance()->reset();
        mSuccessItem = 0;
        mWarningItem = 0;
        mSuccessItemWidget.clear();
        mWaringItemWidget.clear();

        Q_EMIT updateItemCount ();
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
        qDebug() << "num: " << mSuccessItemWidget.count() << " -- " << mWaringItemWidget.count();
        mShowDetail->setIcon (pix);
        if (checked) {
            if (((Success == mType) && mSuccessItemWidget.count() > 0)
                || ((Warning == mType) && mWaringItemWidget.count() > 0)) {
                mDetailWidget->show();
            }
            lazyUpdate ();
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
