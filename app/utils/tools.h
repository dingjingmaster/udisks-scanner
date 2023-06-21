//
// Created by dingjing on 23-6-19.
//

#ifndef UDISKS_SCANNER_TOOLS_H
#define UDISKS_SCANNER_TOOLS_H
#include <cstdio>
#include <QMutex>
#include <QString>

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

QString getLocalIP ();
QString getLocalMAC ();
QString getLocalPCName ();

#endif //UDISKS_SCANNER_TOOLS_H
