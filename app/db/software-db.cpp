//
// Created by dingjing on 23-10-9.
//

#include "software-db.h"

#include <QDebug>
#include <QThread>
#include <QApplication>

#include <glib.h>
#include <gio/gio.h>
#include <sys/wait.h>
#include <gio/gdesktopappinfo.h>

#include "../utils/tools.h"


SoftwareDB* SoftwareDB::gInstance = nullptr;

SoftwareDB *SoftwareDB::getInstance()
{
    static gsize ii = 0;
    if (g_once_init_enter(&ii)) {
        if (!gInstance) {
            gInstance = new SoftwareDB();
            g_once_init_leave(&ii, 1);
        }
    }
    return gInstance;
}

int SoftwareDB::rowCount()
{
    mLocker.lock();
    int c = mData.count();
    mLocker.unlock();

    return c;
}

std::shared_ptr<SoftwareItem> SoftwareDB::getItemByIndex(int row)
{
    mLocker.lock();
    auto c = mData.count();
    std::shared_ptr<SoftwareItem> pp;
    if (row >= 0 && row < c) {
        pp = mData.at (row);
    }
    else {
        pp = nullptr;
    }

    mLocker.unlock();

    return pp;
}

void SoftwareDB::delItemById(const QString &id)
{
    mLocker.lock();
    if (mDataIdx.contains (id)) {
        auto ii = mDataIdx[id];
        mData.removeOne (ii);
        mDataIdx.remove (id);
    }
    mLocker.unlock();
}

int SoftwareDB::getRowByItemID(const QString &id)
{
    int row = -1;
    mLocker.lock();
    if (mDataIdx.contains (id)) {
        auto ii = mDataIdx[id];
        row = mData.indexOf (ii);
    }
    mLocker.unlock();

    return row;
}

SoftwareDB::SoftwareDB(QObject *parent)
    : QObject (parent), mPause (0)
{
    connect (this, &SoftwareDB::start, this, [&] () -> void {
        mPause = 0;
        onDBChanged();
    });

    connect (this, &SoftwareDB::pause, this, [&] () {
        mPause = 1;
    });

    connect (this, &SoftwareDB::stop, this, [&] () -> void {
        mPause = 2;
    });
}

void SoftwareDB::onDBChanged()
{
    if (mPause > 0) return;

    GList* apps = g_app_info_get_all();
    for (auto& app = apps; app; app = app->next) {
        if (G_IS_APP_INFO(app->data)) {
            auto appID = g_app_info_get_id (G_APP_INFO(app->data));
            auto name = g_app_info_get_name (G_APP_INFO(app->data));
            auto cmd = g_app_info_get_commandline (G_APP_INFO(app->data));
            g_autoptr (GDesktopAppInfo) desktopApp = g_desktop_app_info_new (appID);
            auto category = getSoftwareCategory (g_desktop_app_info_get_categories (desktopApp));
            auto version = getAppVersionByCommand (cmd);
            auto item = std::make_shared<SoftwareItem>(name, version, category, getAppInstallTimeByCommand (cmd));
            QApplication::processEvents();
            if (!version.isEmpty()
                && (name && strlen (name) > 0)
                && !mDataIdx.contains (name)) {
                mData.append (item);
                mDataIdx[name] = item;
                Q_EMIT addItem ();
            }
        }
        g_object_unref (app->data);

        while (mPause == 1) {g_usleep(500); QApplication::processEvents (); };
        if (mPause == 2) break;
    }
    g_list_free(apps);

    Q_EMIT loadItemFinished();
}

void SoftwareDB::reset()
{
    mLocker.lock();

    mData.clear();
    mDataIdx.clear();

    mLocker.unlock();
}
