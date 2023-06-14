#include "scanner-task-item.h"

#include <QDebug>
#include <QDateTime>

ScannerTaskItem::ScannerTaskItem(QObject *parent)
    : QObject(parent),
    mID(""),
    mName(""),
    mPolicy({}),
    mScanPath(""),
    mStartTime(0),
    mStatus(Unknow),
    mScanProgress(0)
{

}

ScannerTaskItem::ScannerTaskItem(
    QString id,
    QString name,
    QStringList policy,
    QString scanPath,
    qint64 startTime,
    ScannerTaskItem::Status status,
    float scanProgress, QObject *p)
    : QObject (p),
    mID(std::move(id)),
    mName(std::move (name)),
    mPolicy(std::move(policy)),
    mScanPath(std::move(scanPath)),
    mStartTime(startTime),
    mStatus(status),
    mScanProgress(scanProgress)
{

}

void ScannerTaskItem::setID(QString id)
{
    mID = std::move (id);
}

void ScannerTaskItem::setName(QString name)
{
    mName = std::move (name);
}

void ScannerTaskItem::setPolicy(QStringList policy)
{
    mPolicy = std::move (policy);
}

void ScannerTaskItem::setScanProgress(float scanProgress)
{
    mScanProgress = scanProgress;
}

void ScannerTaskItem::setScanPath(QString scanPath)
{
    mScanPath = std::move(scanPath);
}

void ScannerTaskItem::setStartTime(qint64 startTime)
{
    mStartTime = startTime;
}

void ScannerTaskItem::setStatus(int status)
{
    mStatus = (ScannerTaskItem::Status) status;
}

void ScannerTaskItem::setStatus(ScannerTaskItem::Status status)
{
    mStatus = status;
}

QString ScannerTaskItem::getID()
{
    return mID;
}

QString ScannerTaskItem::getName()
{
    return mName;
}

QStringList ScannerTaskItem::getPolicy()
{
    return mPolicy;
}

QString ScannerTaskItem::getScanPath()
{
    return mScanPath;
}

qint64 ScannerTaskItem::getStartTime() const
{
    return mStartTime;
}

ScannerTaskItem::Status ScannerTaskItem::getStatus()
{
    return mStatus;
}

float ScannerTaskItem::getScanProgress() const
{
    return mScanProgress;
}

bool ScannerTaskItem::getIsChecked() const
{
    return mIsChecked;
}

QString ScannerTaskItem::getStartTimeStr() const
{
    return "2023-12-12_06:08:00";
}

