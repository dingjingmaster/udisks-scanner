//
// Created by dingjing on 23-6-19.
//

#include <glib.h>
#include <gio/gio.h>

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
#include "scan-task-db.h"
#include "ui/message-box.h"
#include "model/scanner-task-item.h"
#include "model/scanner-task-model.h"

ScanTaskDB* ScanTaskDB::gInstance = nullptr;

class ScanTaskDBPrivate : public QObject
{
    Q_OBJECT
public:
    explicit ScanTaskDBPrivate(QString dbPath, ScanTaskDB* p, QObject* parent=nullptr);
    ~ScanTaskDBPrivate() override;

    void open ();
    void close ();
    void setRunning(bool r);
    bool isRunning();
    QStringList getCurrentTaskIds ();

    void setDBFileIsChanged (bool b);
    bool getDBFileIsChanged ();

public:
    void onDBChanged();

private:
    sqlite3*                                            mDB;
    QString                                             mDBPath;
    GCancellable*                                       mCancel;

    QMutex                                              mLocker;
    QList<std::shared_ptr<ScannerTaskItem>>             mData;
    QMap<QString, std::shared_ptr<ScannerTaskItem>>     mDataIdx;

    bool                                                mIsRunning;

    QTimer*                                             mTimer;
    QFileSystemWatcher*                                 mDBWatcher;

    bool                                                mIsDBChanged;
    qint8                                               mTimes;
    QMutex                                              mDBChangedLocker;

    ScanTaskDB*                                         q_ptr;
    Q_DECLARE_PUBLIC(ScanTaskDB);
};

ScanTaskDBPrivate::ScanTaskDBPrivate(QString dbPath, ScanTaskDB *p, QObject* parent)
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
        if (QFileInfo::exists(mDBPath) && mDBWatcher->files().isEmpty()) {
            mDBWatcher->addPath (mDBPath);
            mTimer->setInterval (3 * 1000);
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

    mTimer->setInterval (1 * 1000);
    mTimer->start();
}

ScanTaskDBPrivate::~ScanTaskDBPrivate()
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

void ScanTaskDBPrivate::onDBChanged()
{
    if (isRunning()) return;

    Q_Q(ScanTaskDB);

    qDebug() << "db changed!";

    const char* sql = "SELECT task_id, task_name, scan_task_filter_name, "
                      "scan_task_dir, scan_task_dir_filterout, "
                      "task_start_time, "
                      "task_scan_file_count, task_scan_finished_file_count,"
                      "task_status "
                      "FROM scan_task;";
    {
        open();
        setRunning (true);

        QSet<QString> addedTasks;
        auto last = getCurrentTaskIds().toSet();

        {
            TaskDBLock lock;
            lock.lock();
            sqlite3_stmt* stmt = nullptr;
            qDebug() << sql;
            int ret = sqlite3_prepare_v2 (mDB, sql, -1, &stmt, nullptr);
            if (SQLITE_OK == ret) {
                while (SQLITE_DONE != sqlite3_step (stmt)) {
                    if (g_cancellable_is_cancelled (mCancel)) {
                        qDebug() << "cancelled";
                        break;
                    }

                    QApplication::processEvents();

                    QString id(reinterpret_cast<const char*> (sqlite3_column_text (stmt, 0)));
                    QString name(reinterpret_cast<const char*> (sqlite3_column_text (stmt, 1)));
                    QString filterName(reinterpret_cast<const char*> (sqlite3_column_text (stmt, 2)));
                    QString scanDir(reinterpret_cast<const char*> (sqlite3_column_text (stmt, 3)));
                    QString scanDirOut(reinterpret_cast<const char*> (sqlite3_column_text (stmt, 4)));
                    qint64 startTime = sqlite3_column_int64 (stmt, 5);
                    qint32 fileCount = sqlite3_column_int (stmt, 6);
                    qint32 fileCountFinished = sqlite3_column_int (stmt, 7);
                    qint32 status = sqlite3_column_int (stmt, 8);

//                    qDebug() << "id: " << id;

                    if (nullptr == scanDir || scanDir.isEmpty()) {
                        scanDir = "/";
                    }

                    if (nullptr == id || id.isEmpty()) {
                        continue;
                    }

                    std::shared_ptr<ScannerTaskItem> it;

                    mLocker.lock();
                    if (mDataIdx.contains (id)) {
                        it = mDataIdx[id];
                    }
                    else {
                        it = std::make_shared<ScannerTaskItem>();
                        mDataIdx[id] = it;
                        mData.append (it);
                        addedTasks += id;
                    }
                    last -= id;

                    it->setID (id);
                    it->setName (name);
                    it->setPolicy (filterName);
                    it->setScanPath (scanDir);
                    it->setScanOutPath (scanDirOut);
                    it->setStartTime (startTime);
                    it->setAllFile (fileCount);
                    it->setFinishedFile (fileCountFinished);
                    it->setStatus (status);
                    mLocker.unlock();
                }
            }
            else {
                qWarning() << "query db error!";
            }
        }

        if (!last.isEmpty()) {
            auto delLs = last.toList();
            Q_EMIT q->delTasks (delLs);
        }

        if (!addedTasks.isEmpty()) {
            auto addLs = addedTasks.toList();
            Q_EMIT q->addTasks (addLs);
        }
        setRunning (false);
    }

    qDebug() << "db query ok!";

    Q_EMIT q->loadTaskFinished();
}

void ScanTaskDBPrivate::open()
{
    close();

    int rc = sqlite3_open (mDBPath.toUtf8().constData(), &mDB);
    if (SQLITE_OK != rc) {
        qCritical() << "connect to db: " << mDBPath << " failed!";
        qApp->exit (-1);
    }
}

void ScanTaskDBPrivate::close()
{
    if (mDB) {
        sqlite3_close (mDB);
        mDB = nullptr;
    }
}

void ScanTaskDBPrivate::setRunning(bool r)
{
    mLocker.lock();
    mIsRunning = r;
    mLocker.unlock();
}

bool ScanTaskDBPrivate::isRunning()
{
    mLocker.lock();
    bool r = mIsRunning;
    mLocker.unlock();

    return r;
}

QStringList ScanTaskDBPrivate::getCurrentTaskIds()
{
    mLocker.lock();
    auto ls = mDataIdx.keys();
    mLocker.unlock();

    return ls;
}

void ScanTaskDBPrivate::setDBFileIsChanged(bool b)
{
    mDBChangedLocker.lock();
    mIsDBChanged = b;
    mDBChangedLocker.unlock();
}

bool ScanTaskDBPrivate::getDBFileIsChanged()
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

ScanTaskDB::ScanTaskDB(const QString &dbPath, QObject *parent)
    : QObject (parent), d_ptr(new ScanTaskDBPrivate(dbPath, this))
{
}

ScanTaskDB *ScanTaskDB::getInstance()
{
    static gsize ii = 0;
    if (g_once_init_enter(&ii)) {
        if (!gInstance) {
            gInstance = new ScanTaskDB(DB_PATH);
            g_once_init_leave(&ii, 1);
        }
    }
    return gInstance;
}

int ScanTaskDB::rowCount()
{
    Q_D(ScanTaskDB);

    d->mLocker.lock();
    int c = d->mData.count();
    d->mLocker.unlock();

    return c;
}

std::shared_ptr<ScannerTaskItem> ScanTaskDB::getItemByIndex(int row)
{
    Q_D(ScanTaskDB);

    d->mLocker.lock();
    auto c = d->mData.count();
    std::shared_ptr<ScannerTaskItem> pp;
    if (row >= 0 && row < c) {
        pp = d->mData.at (row);
    }
    else {
        pp = nullptr;
    }

    d->mLocker.unlock();

    return pp;
}

void ScanTaskDB::delItemById(const QString &id)
{
    Q_D(ScanTaskDB);

    d->mLocker.lock();
    if (d->mDataIdx.contains (id)) {
        auto ii = d->mDataIdx[id];
        d->mData.removeOne (ii);
        d->mDataIdx.remove (id);
    }
    d->mLocker.unlock();
}

void ScanTaskDB::testInsertItem()
{
    Q_D(ScanTaskDB);

#define DB_TABLE        ""                                                  \
    "CREATE TABLE scan_task ("                                              \
    "   task_id                         VARCHAR(32) DEFAULT ''  NOT NULL,"  \
    "   task_name                       TEXT        DEFAULT ''  NOT NULL,"  \
    "   scan_task_filter_name           TEXT        DEFAULT ''  NOT NULL,"  \
    "   scan_task_dir                   TEXT        DEFAULT ''  NOT NULL,"  \
    "   scan_task_dir_filterout         TEXT        DEFAULT ''  NOT NULL,"  \
    "   task_start_time                 INTEGER     DEFAULT 0   NOT NULL,"  \
    "   task_scan_file_count            INT         DEFAULT 0   NOT NULL,"  \
    "   task_scan_finished_file_count   INT         DEFAULT 0   NOT NULL,"  \
    "   task_status                     INT         DEFAULT 0   NOT NULL,"  \
    "   PRIMARY KEY(task_id)"                                               \
    ");"                                                                    \
    ""                                                                      \
    "CREATE TABLE udisk_scan_result ("                                      \
    "   Sn                              INTEGER PRIMARY KEY AUTOINCREMENT," \
    "   task_id                         VARCHAR(32)             NOT NULL,"  \
    "   Match_id                        INTEGER                         ,"  \
    "   Org_name                        TEXT                            ,"  \
    "   Task                            TEXT                    NOT NULL,"  \
    "   Policy_name                     TEXT                    NOT NULL,"  \
    "   Pc_name                         TEXT                    NOT NULL,"  \
    "   Ip                              VARCHAR(16)             NOT NULL,"  \
    "   Mac                             VARCHAR(17)             NOT NULL,"  \
    "   Scan_path                       TEXT                            ,"  \
    "   Not_scan_path                   TEXT                            ,"  \
    "   Scan_time                       INTEGER                 NOT NULL,"  \
    "   File_name                       TEXT                    NOT NULL,"  \
    "   Context                         TEXT                             "  \
    ");"                                                                    \
    ""

    qInfo() << DB_TABLE;

    QProcess::execute("rm", QStringList() << "-f" << d->mDBPath);
    if (!QFileInfo::exists(DB_PATH)) {
        QProcess::startDetached("/usr/bin/sqlite3", QStringList() << d->mDBPath << DB_TABLE);
    }

    sqlite3* db = nullptr;
    int rc = sqlite3_open(d->mDBPath.toUtf8().constData(), &db);
    if (SQLITE_OK != rc) {
        printf ("open db error!\n");
        qApp->exit (-1);
    }

    for (int i = 0; i < 1000000; ++i) {
        TaskDBLock l;
        l.lock();
        QString sql = QString("INSERT INTO `scan_task` "
                              "(`task_id`, `task_name`, `scan_task_filter_name`, `scan_task_dir`,"
                              " `scan_task_dir_filterout`, `task_start_time`, `task_scan_file_count`,"
                              " `task_scan_finished_file_count`, `task_status`)"
                              "VALUES"
                              "('%1', '%2', '%3', '%4', '%5', %6, %7, %8, %9)"
                              "").arg (QString("ID-%1").arg (i),
                                       QString("name-%1").arg (i),
                                       "filter1|filter2|filter3|filter4",
                                       "/path/to/scan/dir1|/path/to/scan/dir2",
                                       "/path/to/scan/dir/filterout/|/p/out/")
                                  .arg (i)
                                  .arg (100)
                                  .arg (random() % 100)
                                  .arg (random() % 5);
        qDebug() << sql;
        sqlite3_exec (db, sql.toUtf8().constData(), nullptr, nullptr, nullptr);
    }
    sqlite3_close (db);
}

int ScanTaskDB::getRowByItem(std::shared_ptr<ScannerTaskItem> item)
{
    return -1;
}

int ScanTaskDB::getRowByItemID(const QString &id)
{
    Q_D(ScanTaskDB);

    int row = -1;
    d->mLocker.lock();
    if (d->mDataIdx.contains (id)) {
        auto ii = d->mDataIdx[id];
        row = d->mData.indexOf (ii);
    }
    d->mLocker.unlock();

    return row;
}

#include "scan-task-db.moc"