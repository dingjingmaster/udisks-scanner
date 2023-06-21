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
    mScanPath = scanPath.split ("|");
}

void ScannerTaskItem::setStartTime(qint64 startTime)
{
    mStartTime = startTime;
}

void ScannerTaskItem::setStatus(int status)
{
    //enum Status { Unknow = -1, NoBegin = 0, Scanning, Stop, Finish, Suspended = 4, Continue = 5, Error = 13 }; Q_ENUM(Status)
    switch (status) {
        case 0: {
            mStatus = NoBegin;
            break;
        }
        case 1: {
            mStatus = Scanning;
            break;
        }
        case 2: {
            mStatus = Stop;
            break;
        }
        case 3: {
            mStatus = Finish;
            break;
        }
        case 4: {
            mStatus = Suspended;
            break;
        }
        case 5: {
            mStatus = Continue;
            break;
        }
        default: {
            mStatus = NoBegin;
        }
    }
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
    return mScanPath.join (";");
}

qint64 ScannerTaskItem::getStartTime() const
{
    return mStartTime;
}

ScannerTaskItem::Status ScannerTaskItem::getStatus()
{
    return mStatus;
}

float ScannerTaskItem::getScanProgress()
{
    mScanProgress = (float) mFinishedFile / (float) mAllFile;
    if (mScanProgress > 1) mScanProgress = 1;

    return mScanProgress;
}

bool ScannerTaskItem::getIsChecked() const
{
    return mIsChecked;
}

QString ScannerTaskItem::getStartTimeStr() const
{
    return QDateTime::fromSecsSinceEpoch (mStartTime).toString ("yyyy-MM-dd hh:mm:ss");
}

void ScannerTaskItem::setPolicy(QString policy)
{
    mPolicy = policy.split ("|");
}

void ScannerTaskItem::setScanOutPath(QString scanOutPath)
{
    mScanOutPath = scanOutPath.split ("|");
}

void ScannerTaskItem::setAllFile(qint64 f)
{
    mAllFile = f;
}

void ScannerTaskItem::setFinishedFile(qint64 f)
{
    mFinishedFile = f;
}

int ScannerTaskItem::getFinishedFile()
{
    if (mFinishedFile < 0) mFinishedFile = 0;

    return (int) mFinishedFile;
}

int ScannerTaskItem::getAllFile()
{
    if (mAllFile < 0) mAllFile = 0;

    return (int) mAllFile;
}

void ScannerTaskItem::setIsChecked(bool c)
{
    mIsChecked = c;
}

