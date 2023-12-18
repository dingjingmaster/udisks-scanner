//
// Created by dingjing on 23-6-21.
//

#include "scan-result-db.h"

#include <glib.h>
#include <gio/gio.h>

#include <QDir>
#include <memory>
#include <QDebug>
#include <QTimer>
#include <utility>
#include <fstream>
#include <iostream>
#include <QProcess>
#include <sqlite3.h>
#include <QFileInfo>
#include <QApplication>
#include <QFileSystemWatcher>
#include <QDateTime>

#include "common/log.h"
#include "utils/tools.h"
#include "common/global.h"
#include "ui/message-box.h"
#include "model/result-item.h"
#include "model/result-model.h"


ScanResultDB* ScanResultDB::gInstance = nullptr;

class ScanResultDBPrivate : public QObject
{
    Q_OBJECT
public:
    explicit ScanResultDBPrivate(QString dbPath, ScanResultDB* p, QObject* parent=nullptr);
    ~ScanResultDBPrivate()override;

    void open ();
    void close ();
    void setRunning(bool r);
    bool isRunning();
    QStringList getCurrentResultIDs ();

    void setDBFileIsChanged (bool b);
    bool getDBFileIsChanged ();

    void onDBChanged();


private:
    QString                                             mTaskID;

    sqlite3*                                            mDB;
    QString                                             mDBPath;
    GCancellable*                                       mCancel;

    QMutex                                              mLocker;
    QList<std::shared_ptr<ResultItem>>                  mData;
    QMap<QString, std::shared_ptr<ResultItem>>          mDataIdx;

    bool                                                mIsRunning;

    QTimer*                                             mTimer;
    QFileSystemWatcher*                                 mDBWatcher;

    bool                                                mIsDBChanged;
    qint8                                               mTimes;
    QMutex                                              mDBChangedLocker;

    ScanResultDB*                                       q_ptr;
    Q_DECLARE_PUBLIC(ScanResultDB);
};

ScanResultDBPrivate::~ScanResultDBPrivate()
{
    if (mCancel) {
        g_cancellable_is_cancelled (mCancel);
        g_object_unref (mCancel);
    }
    close();

    delete mDBWatcher;
    delete mTimer;

    mLocker.lock();
    mData.clear();
    mLocker.unlock();
}

ScanResultDBPrivate::ScanResultDBPrivate(QString dbPath, ScanResultDB *p, QObject *parent)
    : QObject (parent),
    mDBPath(std::move(dbPath)),
    q_ptr(p),
    mDB(nullptr),
    mCancel(g_cancellable_new()),
    mIsRunning(false),
    mTimer (new QTimer(this)),
    mDBWatcher(new QFileSystemWatcher(this)),
    mIsDBChanged(false),
    mTimes(0)
{
    connect (mTimer, &QTimer::timeout, this, [=] () -> void {
        if (mTaskID.isNull() || mTaskID.isEmpty() || g_cancellable_is_cancelled (mCancel)) return;
        if (QFileInfo::exists(mDBPath) && mDBWatcher->files().isEmpty()) {
            mDBWatcher->addPath (mDBPath);
            onDBChanged();
            return;
        }

        if (!getDBFileIsChanged()) {
            return;
        }
        setDBFileIsChanged (false);

        onDBChanged();
    });

    connect (mDBWatcher, &QFileSystemWatcher::fileChanged, this, [=] (const QString& path) {
        setDBFileIsChanged (true);
    });

    mTimer->setInterval (5 * 1000);
    mTimer->start();
}

void ScanResultDBPrivate::open()
{
    close();

    int rc = sqlite3_open (mDBPath.toUtf8().constData(), &mDB);
    if (SQLITE_OK != rc) {
        qCritical() << "connect to db: " << mDBPath << " failed!";
        qApp->exit (-1);
    }
}

void ScanResultDBPrivate::close()
{
    if (mDB) {
        sqlite3_close (mDB);
        mDB = nullptr;
    }
}

void ScanResultDBPrivate::setRunning(bool r)
{
    mLocker.lock();
    mIsRunning = r;
    mLocker.unlock();
}

bool ScanResultDBPrivate::isRunning()
{
    mLocker.lock();
    bool r = mIsRunning;
    mLocker.unlock();

    return r;
}

QStringList ScanResultDBPrivate::getCurrentResultIDs()
{
    mLocker.lock();
    auto ls = mDataIdx.keys();
    mLocker.unlock();

    return ls;
}

void ScanResultDBPrivate::setDBFileIsChanged(bool b)
{
    mDBChangedLocker.lock();
    mIsDBChanged = b;
    mDBChangedLocker.unlock();
}

bool ScanResultDBPrivate::getDBFileIsChanged()
{
    mDBChangedLocker.lock();
    bool c = mIsDBChanged;
    if (!c) ++mTimes; else mTimes = 0;
    if (mTimes >= 20 * 5) {
        // 数据库长时间未更新(5分钟)，还是要做一次更新的
        c = true;
        mTimes = 0;
    }
    mDBChangedLocker.unlock();

    return c;
}

void ScanResultDBPrivate::onDBChanged()
{
    if (isRunning() || g_cancellable_is_cancelled (mCancel)) return;

    Q_Q(ScanResultDB);

    QString sql = QString("SELECT sn, file_name, scan_time "
                      " FROM udisk_scan_result WHERE task_id='%1';").arg(mTaskID);
    LOG_DEBUG("sql: %s", sql.toUtf8().constData());
    {
        open();
        setRunning (true);

        QSet<QString> addedResult;
        auto last = getCurrentResultIDs().toSet();

        {
            TaskDBLock::getInstance()->lock();
            sqlite3_stmt* stmt = nullptr;
            qDebug() << sql;
            int ret = sqlite3_prepare_v2 (mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
            if (SQLITE_OK == ret) {
                while (SQLITE_DONE != sqlite3_step (stmt)) {
                    QApplication::processEvents();
                    if (g_cancellable_is_cancelled (mCancel)) {
                        qDebug() << "cancelled";
                        break;
                    }

                    QString id = QString("%1").arg (sqlite3_column_int64(stmt, 0));
                    QString name(reinterpret_cast<const char*> (sqlite3_column_text (stmt, 1)));
                    qint64 scanTime = sqlite3_column_int64 (stmt, 2);

                    QFileInfo fi(name);

                    qDebug() << "id: " << id;

                    if (nullptr == id || id.isEmpty()) {
                        continue;
                    }

                    std::shared_ptr<ResultItem> it;

                    mLocker.lock();
                    if (mDataIdx.contains (id)) {
                        it = mDataIdx[id];
                    }
                    else {
                        it = std::make_shared<ResultItem>(id, fi.fileName(), fi.dir().absolutePath(), scanTime);
                        mDataIdx[id] = it;
                        mData.append (it);
                        addedResult += id;
                    }
                    last -= id;

                    it->setID (id);
                    it->setFileName (fi.fileName());
                    it->setFilePath (fi.dir().absolutePath());
                    it->setFileTime (scanTime);
                    mLocker.unlock();
                }
            }
            else {
                qWarning() << "query db error!";
            }
            TaskDBLock::getInstance()->unlock();
        }

        if (!last.isEmpty()) {
            auto delLs = last.toList();
            Q_EMIT q->delTaskResult(delLs);
        }

        if (!addedResult.isEmpty()) {
            auto addLs = addedResult.toList();
            Q_EMIT q->addTasksResult(addLs);
        }
        setRunning (false);
    }
    qDebug() << "db query ok!";

    Q_EMIT q->loadTaskResultFinished();

}

ScanResultDB *ScanResultDB::getInstance()
{
    static gsize ii = 0;
    if (g_once_init_enter(&ii)) {
        if (!gInstance) {
            gInstance = new ScanResultDB(gDBPath);
            g_once_init_leave(&ii, 1);
        }
    }
    return gInstance;
}

void ScanResultDB::setTaskID(const QString &id)
{
    Q_D(ScanResultDB);

    d->mTaskID = id;

    if (d->mTaskID.isNull() || d->mTaskID.isEmpty()) {
        g_cancellable_cancel (d->mCancel);
    }
    else {
        g_cancellable_reset (d->mCancel);
    }
}

ScanResultDB::ScanResultDB(const QString &dbPath, QObject *parent)
    : QObject (parent), d_ptr(new ScanResultDBPrivate(dbPath, this))
{

}

int ScanResultDB::rowCount()
{
    Q_D(ScanResultDB);

    d->mLocker.lock();
    int c = d->mData.count();
    d->mLocker.unlock();

    return c;
}

std::shared_ptr<ResultItem> ScanResultDB::getItemByIndex(int row)
{
    Q_D(ScanResultDB);

    d->mLocker.lock();
    auto c = d->mData.count();
    std::shared_ptr<ResultItem> pp;
    if (row >= 0 && row < c) {
        pp = d->mData.at (row);
    }
    else {
        pp = nullptr;
    }

    d->mLocker.unlock();

    return pp;
}

int ScanResultDB::getRowByItem(std::shared_ptr<ResultItem> item)
{
    return -1;
}

int ScanResultDB::getRowByItemID(const QString &id)
{
    Q_D(ScanResultDB);

    int row = -1;
    d->mLocker.lock();
    if (d->mDataIdx.contains (id)) {
        auto ii = d->mDataIdx[id];
        row = d->mData.indexOf (ii);
    }
    d->mLocker.unlock();

    return row;
}
void ScanResultDB::testInsertItem()
{
    Q_D(ScanResultDB);

    sqlite3* db = nullptr;
    int rc = sqlite3_open(d->mDBPath.toUtf8().constData(), &db);
    if (SQLITE_OK != rc) {
        printf ("open db error!\n");
        qApp->exit (-1);
    }

    for (int i = 0; i < 1000000; ++i) {
        TaskDBLock::getInstance()->lock();
        QString sql = QString("INSERT INTO `udisk_scan_result` "
                              "(`task_id`, `file_name`, `file_path`)"
                              "VALUES"
                              "('%1', '%2', '%3');"
                              "").arg (QString("ID-%1").arg (375),
                                       QString("name-%1").arg (i),
                                       "/path/to/scan/dir/filterout/|/p/out/");
        qDebug() << sql;
        sqlite3_exec (db, sql.toUtf8().constData(), nullptr, nullptr, nullptr);
        TaskDBLock::getInstance()->unlock();
    }
    sqlite3_close (db);
}

void ScanResultDB::exportResultByTaskID(const QString& file, const QStringList &ids)
{
    Q_D(ScanResultDB);

    // 事件ID(event_ID)， text
    // 任务名称(task_name)， text
    // 任务ID(task_id)， text
    // 检查路径(scan_path)， text
    // 策略名称(policy_name)， text
    // 策略ID(policy_ID)， text
    // 严重性(severity)， text
    // 文件名(file_name)， text
    // 文件路径(file_path)， text
    // 时间(scan_time)， int
    // IP(ip)， text
    // 主机名(pc_name)， text
    // MAC(mac)， text
    // 操作系统类型(os)， text
    // 文件类型(file_exte), text

    g_return_if_fail(!ids.isEmpty());

    std::ofstream outStream;
    outStream.open (file.toUtf8().toStdString(), std::ios::out | std::ios::trunc);
    outStream << "事件ID," << "任务名称," << "任务ID," << "检查路径," << "策略名称,"
              << "策略ID," << "严重性," << "文件名," << "文件路径," << "时间,"
              << "IP," << "主机名," << "MAC," << "操作系统类型," << "文件类型"
              << std::endl;

    QString sql = QString("SELECT sn, event_ID, task_name, task_id, scan_path,"
                          " policy_name, policy_ID, severity, file_name, file_path,"
                          " scan_time, ip, pc_name, mac, os, file_exte "
                          " FROM udisk_scan_result WHERE task_id IN (%1);").arg(ids.join (","));
    {
        d->open();
        d->setRunning (true);
        {
            TaskDBLock::getInstance()->lock();
            sqlite3_stmt* stmt = nullptr;
            qDebug() << sql;
            int ret = sqlite3_prepare_v2 (d->mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
            if (SQLITE_OK == ret) {
                while (SQLITE_DONE != sqlite3_step (stmt)) {
                    QApplication::processEvents ();

                    QString id = QString ("%1").arg (sqlite3_column_int64 (stmt, 0));
                    QString eventID = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 1)));
                    QString taskName = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 2)));
                    QString taskID = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 3)));
                    QString scanPath = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 4)));
                    QString policyName = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 5)));
                    QString policyID = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 6)));
                    QString serverity = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 7)));
                    QString fileName = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 8)));
                    QString filePath = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 9)));
                    QDateTime dt = QDateTime::fromTime_t(sqlite3_column_int64(stmt, 10));
                    QString scanTime = dt.toString("yyyy-MM-dd hh:mm:ss");
                    QString ip = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 11)));
                    QString pcName = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 12)));
                    QString mac = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 13)));
                    QString os = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 14)));
                    QString fileExt = (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 15)));

                    qDebug () << "id: " << id;

                    if (nullptr == id || id.isEmpty ()) {
                        continue;
                    }

                    outStream << eventID.toUtf8().toStdString() << ","
                              << "\"" << taskName.toUtf8().toStdString() << "\"" << ","
                              << taskID.toUtf8().toStdString() << ","
                              << "\"" << scanPath.toUtf8().toStdString() << "\"" << ","
                              << "\"" << policyName.toUtf8().toStdString() << "\"" << ","
                              << policyID.toUtf8().toStdString() << ","
                              << serverity.toUtf8().toStdString() << ","
                              << "\"" << fileName.toUtf8().toStdString() << "\"" << ","
                              << "\"" << filePath.toUtf8().toStdString() << "\"" << ","
                              << scanTime.toUtf8().toStdString() << ","
                              << ip.toUtf8().toStdString() << ","
                              << pcName.toUtf8().toStdString() << ","
                              << mac.toUtf8().toStdString() << ","
                              << os.toUtf8().toStdString() << ","
                              << "\"" << fileExt.toUtf8().toStdString() << "\"" << ","
                              << std::endl;
                }
            }
            else {
                qWarning() << "query db error!";
            }
            TaskDBLock::getInstance()->unlock();
        }
        d->setRunning (false);
    }
}

void ScanResultDB::onLoad()
{
    Q_D(ScanResultDB);

    d->onDBChanged();
}

void ScanResultDB::clear()
{
    Q_D(ScanResultDB);
    d->mLocker.lock();
    d->mData.clear();
    d->mDataIdx.clear();
    d->mLocker.unlock();
}


#include "scan-result-db.moc"
