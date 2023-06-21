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
#include <QProcess>
#include <sqlite3.h>
#include <QFileInfo>
#include <QApplication>
#include <QFileSystemWatcher>

#include "utils/tools.h"
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

    QString sql = QString("SELECT Sn, File_name, Scan_time "
                      " FROM udisk_scan_result WHERE task_id=%1;").arg(mTaskID);
    {
        open();
        setRunning (true);

        QSet<QString> addedResult;
        auto last = getCurrentResultIDs().toSet();

        {
            TaskDBLock lock;
            lock.lock();
            sqlite3_stmt* stmt = nullptr;
            qDebug() << sql;
            int ret = sqlite3_prepare_v2 (mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
            if (SQLITE_OK == ret) {
                while (SQLITE_DONE != sqlite3_step (stmt)) {
                    if (g_cancellable_is_cancelled (mCancel)) {
                        qDebug() << "cancelled";
                        break;
                    }

                    QApplication::processEvents();

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
            gInstance = new ScanResultDB(DB_PATH);
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

}


#include "scan-result-db.moc"