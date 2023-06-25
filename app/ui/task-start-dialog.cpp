//
// Created by dingjing on 23-6-25.
//

#include "task-start-dialog.h"

#include <QVBoxLayout>


TaskStartDialog::TaskStartDialog(QWidget *parent)
    : QDialog (parent)
{
    setMinimumSize(380, 200);

    setWindowTitle ("任务启动");

    setWindowFlag (Qt::WindowCloseButtonHint, true);
    setWindowFlag (Qt::WindowMinimizeButtonHint, false);
    setWindowFlag (Qt::WindowMaximizeButtonHint, false);

    mMainLayout = new QVBoxLayout;

    auto l1 = new QVBoxLayout;

    setLayout (mMainLayout);
}

QString TaskStartDialog::getScanDir()
{
    return mScanDir;
}

QString TaskStartDialog::getExceptDir()
{
    return mExceptDir;
}
