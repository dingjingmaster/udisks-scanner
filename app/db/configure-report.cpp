//
// Created by dingjing on 23-10-10.
//

#include "configure-report.h"

#include <glib.h>

#include <QDebug>
#include <QTimer>
#include <QApplication>

#include "common/log.h"
#include "utils/tools.h"
#include "common/global.h"

ConfigureReport* ConfigureReport::gInstance = nullptr;


ConfigureReport::ConfigureReport(QObject *parent)
    : QObject (parent), mDBPath(gDBPath), mPause (0)
{
    mTimer = new QTimer;

    mTimer->setInterval (3);

    connect (mTimer, &QTimer::timeout, this, [=] () -> void {
        if (mIsRunning > 0) return;
        onDBChanged();
    });

    connect (this, &ConfigureReport::start, this, [=] () -> void {
        mPause = 0;
        onDBChanged();
    });

    connect (this, &ConfigureReport::pause, this, [=] () {
        mPause = 1;
    });

    connect (this, &ConfigureReport::stop, this, [=] () -> void {
        mPause = 2;
    });
}

bool ConfigureReport::openSqlite()
{
    closeSqlite();

    int rc = sqlite3_open (mDBPath.toUtf8().constData(), &mDB);
    if (SQLITE_OK != rc) {
        qWarning() << "connect to db: " << mDBPath << " failed!";
        return false;
    }

    return true;
}

ConfigureReport *ConfigureReport::getInstance()
{
    static gsize ii = 0;
    if (g_once_init_enter(&ii)) {
        if (!gInstance) {
            gInstance = new ConfigureReport();
            g_once_init_leave(&ii, 1);
        }
    }
    return gInstance;
}

void ConfigureReport::closeSqlite()
{
    if (mDB) {
        sqlite3_close (mDB);
        mDB = nullptr;
    }
}

void ConfigureReport::onDBChanged()
{
    if (mPause > 0) return;

    mIsRunning = 1;
    // check_result = 0 正常; check_result = 1 异常
    QString sql ("SELECT conf_name, conf_type, risk_level, check_method, fix_method, check_result FROM conf_info;");
    LOG_DEBUG("sql: %s", sql.toUtf8().constData());
    {
        if (!openSqlite()) {
            return;
        }
        {
            qInfo () << "query db start!";
            TaskDBLock lock;
            lock.lock ();
            sqlite3_stmt *stmt = nullptr;
            qDebug () << sql;
            int ret = sqlite3_prepare_v2 (mDB, sql.toUtf8 ().constData (), -1, &stmt, nullptr);
            if (SQLITE_OK == ret) {
                while (SQLITE_DONE != sqlite3_step (stmt)) {
                    QApplication::processEvents ();

                    QString name (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 0)));
                    QString type (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 1)));
                    QString level (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 2)));
                    QString checkMethod (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 3)));
                    QString fixedMethod (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 4)));
                    QString result (reinterpret_cast<const char *> (sqlite3_column_text (stmt, 5)));

                    if (nullptr == name || name.isEmpty ()) {
                        continue;
                    }

                    if (!mDataIdx.contains (name)) {
                        auto it = std::make_shared<ConfigureItem> (name, type, level, checkMethod, fixedMethod, result);
                        mData.append (it);
                        mDataIdx[name] = it;
                        Q_EMIT addItem(name);
                    }
                }
                sqlite3_finalize (stmt);
            } else {
                qWarning () << "query db error!";
            }
        }
        qInfo () << "query db OK!";
    }

    mIsRunning = 0;
}

void ConfigureReport::reset()
{
    mLocker.lock();

    mData.clear();
    mDataIdx.clear();

    mLocker.unlock();
}

std::shared_ptr<ConfigureItem> ConfigureReport::getItemByKey(QString &name)
{
    if (mDataIdx.contains (name)) {
        return mDataIdx[name];
    }

    return nullptr;
}
