//
// Created by dingjing on 23-6-25.
//

#include "task-start-dialog.h"

#include <QLabel>
#include <QDebug>
#include <QTextEdit>
#include <QLineEdit>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPushButton>
#include <QStandardPaths>

#include "push-button.h"
#include "message-box.h"


TaskStartDialog::TaskStartDialog(QWidget *parent)
    : QDialog (parent)
{
    setMinimumSize(680, 300);

    setWindowTitle ("任务启动");

    setWindowFlag (Qt::WindowCloseButtonHint, true);
    setWindowFlag (Qt::WindowMinimizeButtonHint, false);
    setWindowFlag (Qt::WindowMaximizeButtonHint, false);

    mMainLayout = new QVBoxLayout;
    mMainLayout->setContentsMargins (16, 16, 16, 16);

    auto l1 = new QGridLayout;
    auto l2 = new QHBoxLayout;

    auto lb11 = new QLabel;
    lb11->setText ("<red style='font-weight:bold;color:red;'>*</red>检查路径：");
    auto lb12 = new QLineEdit;
    auto lb13 = new PushButton(nullptr, PushButton::Type3);
    lb11->setTextFormat (Qt::RichText);
    lb13->setEnable(true);
    lb13->setText ("选择");
    lb12->setFocus();
    lb12->setPlaceholderText ("请输入要扫描的路径");

    auto lb21 = new QLabel;
    lb21->setText ("检查例外路径：");
    auto lb22 = new QLineEdit;
    auto lb23 = new PushButton(nullptr, PushButton::Type3);
    lb23->setEnable(true);
    lb23->setText ("选择");
    lb22->setPlaceholderText ("请输入或选择检查文件路径");

    auto lb31 = new QLabel;
    lb31->setText ("备注：");
    auto lb32 = new QTextEdit;
    lb32->setPlaceholderText ("请输入备注，最多不超过50 字");

    l1->addWidget (lb11, 1,  0, 1, 2, Qt::AlignRight);
    l1->addWidget (lb12, 1,  3, 1, 8);
    l1->addWidget (lb13, 1, 11, 1, 2);

    l1->addWidget (lb21, 2,  0, 1, 2, Qt::AlignRight);
    l1->addWidget (lb22, 2,  3, 1, 8);
    l1->addWidget (lb23, 2, 11, 1, 2);

    l1->addWidget (lb31, 3,  0, 1, 2, Qt::AlignRight);
    l1->addWidget (lb32, 3,  3, 1, 8);

    auto btn1 = new PushButton(nullptr, PushButton::Type3);
    btn1->setEnable(true);
    btn1->setText ("确定");

    auto btn2 = new PushButton(nullptr, PushButton::Type3);
    btn2->setEnable(true);
    btn2->setText ("取消");

    l2->addWidget (btn1);
    l2->addWidget (btn2);

    mMainLayout->addLayout (l1);
    mMainLayout->addLayout (l2);

    setLayout (mMainLayout);

    connect (lb13, &PushButton::clicked, this, [=] () {
        QFileDialog dlg;
        dlg.setAcceptMode (QFileDialog::AcceptOpen);
        dlg.setDirectory (QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        dlg.setFileMode (QFileDialog::Directory);
        QString scanDir;
        if (QDialog::Accepted == dlg.exec()) {
            auto path = dlg.selectedFiles();
            qDebug() << "open path: " << path;
            if (path.isEmpty()) return;
            scanDir = path.first();
            lb12->setText (scanDir);
        }
    });

    connect (lb23, &PushButton::clicked, this, [=] () {
        QFileDialog dlg;
        dlg.setAcceptMode (QFileDialog::AcceptOpen);
        dlg.setDirectory (QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        dlg.setFileMode (QFileDialog::Directory);
        if (QDialog::Accepted == dlg.exec()) {
            auto path = dlg.selectedFiles();
            qDebug() << "open path: " << path;
            if (path.isEmpty()) return;
            auto l = path.first();
            lb22->setText (l);
        }
    });

    connect (lb12, &QLineEdit::textChanged, this, [=] (const QString& str) {
        if (mScanDir.isEmpty()) {
            mScanDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
        mScanDir = str;
    });

    connect (lb22, &QLineEdit::textChanged, this, [=] (const QString& str) {
        mExceptDir = str;
    });

    connect (btn1, &PushButton::clicked, this, [=] {
        if (!QFile::exists (mScanDir)) {
            MessageBox msg("提示", "请输入合法文件夹路径");
            auto b = msg.addButton ("确定");
            connect (b, &QPushButton::clicked, this, [&] (bool) -> void { msg.reject(); });
            msg.exec();
            lb12->clear();
        }
        else {
            accept();
        }
    });

    connect (btn2, &PushButton::clicked, this, [=] { reject(); });
}

QString TaskStartDialog::getScanDir()
{
    return mScanDir;
}

QString TaskStartDialog::getExceptDir()
{
    return mExceptDir;
}
