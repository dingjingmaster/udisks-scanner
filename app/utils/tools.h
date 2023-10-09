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

QString getBIOS ();
QString getOSName ();
QString getOSType ();
QString getVendor ();
QString getLocalIP ();
QString getCpuInfo ();
QString getMemInfo ();
QString getSwapInfo ();
QString getLocalMAC ();
QString getDesktopInfo ();
QString getLocalPCName ();
QString getCommandFullPath(const QString& cmd);
QString getSoftwareCategory(const QString& cate);
QString getAppVersionByCommand(const QString& cmd);
QString executeCommandGetResult(const QString& cmd);
qint64 getAppInstallTimeByCommand(const QString& cmd);
QString getHostPropertyByDbus (const QString& propertyName);

#endif //UDISKS_SCANNER_TOOLS_H
