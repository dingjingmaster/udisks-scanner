//
// Created by dingjing on 23-6-19.
//

#ifndef UDISKS_SCANNER_TOOLS_H
#define UDISKS_SCANNER_TOOLS_H
#include <cstdio>
#include <QMutex>

#define TASK_DB_LOCKER          "/tmp/.est.db.lock"

class TaskDBLock
{
public:
    TaskDBLock();
    ~TaskDBLock();

    void lock ();
    void unlock ();


private:
    void init ();
    bool lock1();
    bool unlock1();

private:
    FILE*       mFile;
    bool        mIsLock;
    QMutex      mLocker;
};


#endif //UDISKS_SCANNER_TOOLS_H
