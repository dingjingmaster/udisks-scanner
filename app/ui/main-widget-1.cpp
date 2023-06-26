//
// Created by dingjing on 23-6-13.
//

#include "main-widget-1.h"

#include <QLabel>
#include <QComboBox>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QPushButton>
#include <QStandardPaths>
#include <QAbstractItemView>

#include "ipc/to-pf.h"
#include "push-button.h"
#include "message-box.h"
#include "view/header-view.h"
#include "db/scan-result-db.h"
#include "view/scanner-view.h"
#include "task-start-dialog.h"
#include "scanner-task-delegate.h"
#include "model/scanner-task-model.h"


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

    auto btn11 = new PushButton(nullptr, PushButton::Type3);
    btn11->setEnable(true);
    btn11->setText ("清除");

    l1->addWidget (lb1);
    l1->addWidget (mComboBox);
    l1->addWidget (btn1);
    l1->addSpacing (10);
    l1->addWidget (btn11);
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
//    mComboBox->setModel (mModel);
//    mComboBox->setModelColumn (2);
    auto header = new HeaderView(Qt::Horizontal);
    mView->setHorizontalHeader (header);

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

    // 导入
    connect (btn1, &PushButton::clicked, this, [=] () -> void {
        QFileDialog dlg;
        dlg.setAcceptMode (QFileDialog::AcceptOpen);
        dlg.setDirectory (QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        dlg.setFileMode (QFileDialog::ExistingFile);
        if (QDialog::Accepted == dlg.exec()) {
            auto path = dlg.selectedFiles();
            qDebug() << "open path: " << path;
            if (path.isEmpty()) return;
            auto l = path.first();
            if (!mCombList.contains (l)) {
                mCombList.append (l);
                ToPF::getInstance()->loadTask (l);
                QFileInfo fi(l);
                mComboBox->addItem (fi.fileName());
            }
        }
        else {
            qDebug() << "load task cancel!";
        }
    });

    // 清除
    connect (btn11, &PushButton::clicked, this, [=] () -> void {
        if (mComboBox)  mComboBox->clear();
        mCombList.clear();
    });

    // 删除
    connect (btn21, &PushButton::clicked, this, [=] () -> void {
        auto toDels = mModel->getSelectedItems();

        // 运行中的任务不可删除
        QStringList ids;
        for (auto& l : toDels) {
            if (ScannerTaskItem::Scanning != l->getStatus()) {
                ids << l->getID();
            }
        }

        if (ids.isEmpty()) {
            return;
        }

        ToPF::getInstance()->taskDelete(ids);
    });

    // 导出
    connect (btn22, &PushButton::clicked, this, [=] () -> void {
        auto toE = mModel->getSelectedItems();
        QStringList ids;
        for (auto& l : toE) {
            ids << l->getID();
        }

        if (ids.isEmpty()) {
            return;
        }

        QFileDialog dlg;
        dlg.setDefaultSuffix (".csv");
        dlg.setAcceptMode (QFileDialog::AcceptSave);
        dlg.setDirectory (QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        dlg.setFileMode (QFileDialog::ExistingFile);
        if (QDialog::Accepted == dlg.exec()) {
            auto path = dlg.selectedFiles();
            qDebug() << "open path: " << path;
            if (path.isEmpty()) return;
            auto l = path.first();
            ScanResultDB::getInstance()->exportResultByTaskID (l, ids);
        }
    });

    // 单任务操作 - start
    connect (mView, &ScannerView::taskStart, this, [=] (const QString& id) -> void {
        qDebug() << "start: " << id;
        TaskStartDialog dlg(this);
        if (QDialog::Accepted == dlg.exec()) {
            qDebug() << "\nscan: " << dlg.getScanDir() << "\nexpect scan: " << dlg.getExceptDir();
            ToPF::getInstance()->taskStart(id, dlg.getScanDir(), dlg.getExceptDir());
        }
    });

    connect (mView, &ScannerView::taskPause, this, [=] (const QString& id) -> void {
        qDebug() << "pause: " << id;
        MessageBox mb ("提示", "您确定是否暂停任务操作？", mView);
        auto b = mb.addButton ("确定");
        connect (b, &QPushButton::clicked, this, [&](bool) { ToPF::getInstance ()->taskPause (id); mb.accept(); });
        b = mb.addButton ("取消");
        connect (b, &QPushButton::clicked, this, [&](bool) { mb.reject(); });
        mb.exec();
    });

    connect (mView, &ScannerView::taskResume, this, [=] (const QString& id) -> void {
        qDebug() << "resume: " << id;
        MessageBox mb ("提示", "您确定是否再启动任务操作？", mView);
        auto b = mb.addButton ("确定");
        connect (b, &QPushButton::clicked, this, [&](bool) { ToPF::getInstance ()->taskResume (id); mb.accept(); });
        b = mb.addButton ("取消");
        connect (b, &QPushButton::clicked, this, [&](bool) { mb.reject(); });
        mb.exec();
    });
    connect (mView, &ScannerView::taskStop, this, [=] (const QString& id) -> void {
        qDebug() << "stop: " << id;
        MessageBox mb ("提示", "您确定是否停止任务操作？", mView);
        auto b = mb.addButton ("确定");
        connect (b, &QPushButton::clicked, this, [&](bool) { ToPF::getInstance ()->taskStop(id); mb.accept(); });
        b = mb.addButton ("取消");
        connect (b, &QPushButton::clicked, this, [&](bool) { mb.reject(); });
        mb.exec();
    });
    connect (mView, &ScannerView::taskDelete, this, [=] (const QString& id) -> void {
        qDebug() << "delete: " << id;
        MessageBox mb ("提示", "您确定是否删除任务操作？", mView);
        auto b = mb.addButton ("确定");
        connect (b, &QPushButton::clicked, this, [&](bool) { ToPF::getInstance ()->taskDelete(id); mb.accept(); });
        b = mb.addButton ("取消");
        connect (b, &QPushButton::clicked, this, [&](bool) { mb.reject(); });
        mb.exec();
    });
    // 单任务操作 - end

#if 0
    connect (mModel, &QAbstractTableModel::rowsInserted, this, [=] (const QModelIndex& p, int first, int last) -> void {
        if (!mModel)return;
        if (mModel->rowCount (QModelIndex()) > 0) {
            mComboBox->setCurrentIndex (0);
        }
    });

    connect (mComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [=] (int index) -> void {
        if (!mModel)return;
        auto idx = mModel->getIndexByRow (index);
        mView->scrollTo (idx);
        mView->setCurrentIndex (idx);
    });
#endif

    connect (mView, qOverload<const QString&>(&ScannerView::taskDetail), this, qOverload<const QString&>(&MainWidget1::taskDetail));

    connect (header, &HeaderView::checkBoxClicked, this, [=] (bool c) -> void {
        if (!mModel)return;
        mModel->setSelectedAll (c);
        Q_EMIT checkedItem (mModel->hasChecked());
    });

    connect (mModel, &ScannerTaskModel::updateView, this, [=] () -> void {
        if (!mModel || !mView)return;
        auto s = mView->verticalScrollBar();
        auto scrollBarRatio = float(s->value()) / (s->maximum() - s->minimum());
        auto curItemIndex = scrollBarRatio * mModel->rowCount(QModelIndex());
        auto startIndex = ((curItemIndex - 30) >= 0) ? (curItemIndex - 30) : 0;
        auto stopIndex = ((curItemIndex + 30) < mModel->rowCount(QModelIndex())) ? (curItemIndex + 30) : mModel->rowCount(QModelIndex());

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
    });

    connect (mView, &QAbstractItemView::clicked, this, [=] (const QModelIndex &index) {
        if (!index.isValid())   return;
        if (index.column() == 0) {
            auto cc = static_cast<ScanTaskDB*>(index.internalPointer());
            if (!cc) {
                return;
            }
            auto item = cc->getItemByIndex (index.row());
            item->setIsChecked(!item->getIsChecked());
            bool checkAll = cc->isCheckAllItems();
            header->setChecked(checkAll);
            Q_EMIT checkedItem(checkAll || cc->hasChecked ());
            Q_EMIT mModel->updateView();
        }
    });

    connect (this, &MainWidget1::checkedItem, this, [=] (bool b) -> void {
        if (b) {
            btn21->setEnable (true);
            btn22->setEnable (true);
        }
        else {
            btn21->setEnable (false);
            btn22->setEnable (false);
        }
    });


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

