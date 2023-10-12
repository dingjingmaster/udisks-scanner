//
// Created by dingjing on 23-10-10.
//

#include "configure-check-ui.h"

#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QApplication>

#include "common/global.h"
#include "db/configure-report.h"

#define SOFTWARE_TITLE              "配置检查 ----------------------------------------------------------------------- %1项"

ConfigureCheckUI::ConfigureCheckUI(Type type, QWidget *parent)
    : QWidget (parent), mIsChecked(false), mType(type)
{
    mProgress = new QProcess;
    mMainLayout = new QVBoxLayout;
    auto titleLayout = new QHBoxLayout;

    mProgress->setProgram (gConfigScannerPath);

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

    mDetailWidget = new QWidget;
    auto detailLayout = new QVBoxLayout;
    mDetailWidget->setLayout (detailLayout);
    mMainLayout->addWidget (mDetailWidget);

    setLayout (mMainLayout);

//    connect (mProgress, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT([=] (int extCode, QProcess::ExitStatus status) -> void {
//        if (status == QProcess::NormalExit) {
//            qDebug() << "正常结束...";
//            ConfigureReport::getInstance()->start();
//        }
//        else {
//            qDebug() << "不正常结束...";
//        }
//    }));

    connect (this, &ConfigureCheckUI::stop, this, [=] () -> void {
        ConfigureReport::getInstance()->stop();
        Q_EMIT reset();
    });

    connect (this, &ConfigureCheckUI::pause, this, [=] () -> void {
        ConfigureReport::getInstance()->pause();
    });

    connect (this, &ConfigureCheckUI::start, this, [=] () -> void {
        mProgress->startDetached();
        while (!mProgress->waitForFinished (2000)) {QApplication::processEvents ();};
        ConfigureReport::getInstance()->start();
    });

    connect (this, &ConfigureCheckUI::addItem, this, [=] (const std::shared_ptr<ConfigureItem>& item) -> void {
        if (mType == Success) {
            ++mSuccessItem;
            Q_EMIT updateItemCount (mSuccessItem);
        }
        else if (mType == Warning) {
            ++mWarningItem;
            Q_EMIT updateItemCount (mWarningItem);
        }

        auto itemUI = std::make_shared<ConfigureCheckItemUI>(
            item->getName(),
            item->getCategory(),
            item->getLevel(),
            item->getCheckMethod(),
            item->getRepairMethod());
        mItemWidget.append (itemUI);
        detailLayout->addWidget (itemUI.get());
    });

    connect (this, &ConfigureCheckUI::reset, this, [=] () -> void {
        ConfigureReport::getInstance()->reset();
        for (auto& i : mItemWidget) {
            detailLayout->removeWidget (i.get());
        }

        mSuccessItem = 0;
        mWarningItem = 0;
        mItemWidget.clear();

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

int ConfigureCheckUI::getSuccessItem() const
{
    return ((mSuccessItem > 0) ? mSuccessItem : 0);
}

int ConfigureCheckUI::getWarningItem() const
{
    return ((mWarningItem > 0) ? mWarningItem : 0);
}
