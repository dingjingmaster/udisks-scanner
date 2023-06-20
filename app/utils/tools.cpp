//
// Created by dingjing on 23-6-19.
//

#include "tools.h"

#include <glib.h>
#include <QDebug>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

TaskDBLock::TaskDBLock()
    : mFile(nullptr), mIsLock(false)
{
}

void TaskDBLock::lock()
{
    while (!lock1());
}

void TaskDBLock::unlock()
{
    while (!unlock1());
}

void TaskDBLock::init()
{
    if (!mFile) {
        mFile = fopen (TASK_DB_LOCKER, "a+");
        if (!mFile) {
            qWarning() << "lock file: " << TASK_DB_LOCKER;
        }
    }
}

TaskDBLock::~TaskDBLock()
{
    if (mIsLock) {
        unlock();
    }

    if (mFile) {
        fclose (mFile);
    }
}

bool TaskDBLock::lock1()
{
    init();
    g_return_val_if_fail(mFile, false);

    mLocker.lock();
    if (0 == flock(mFile->_fileno, LOCK_EX | LOCK_NB)) {
        mIsLock = true;
    }
    mLocker.unlock();

    return mIsLock;
}

bool TaskDBLock::unlock1()
{
    mLocker.lock();
    if (0 == flock (mFile->_fileno, LOCK_UN)) {
        mIsLock = false;
    }
    mLocker.unlock();

    return !mIsLock;
}
