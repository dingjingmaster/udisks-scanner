//
// Created by dingjing on 23-9-27.
//

#include "software-model.h"

#include <QDir>
#include <QFont>
#include <QDebug>
#include <QProcess>
#include <QApplication>

#include <glib.h>
#include <stdio.h>
#include <gio/gio.h>
#include <sys/wait.h>
#include <gio/gdesktopappinfo.h>

#include "software-item.h"
#include "../utils/tools.h"
#include "db/software-db.h"

SoftwareModel::SoftwareModel(QAbstractTableModel *parent)
    : QAbstractTableModel (parent), mData(SoftwareDB::getInstance())
{
    connect (SoftwareDB::getInstance(), &SoftwareDB::addItem, this, [=] {
        insertRow(mData->rowCount() - 1);
        updateView();
    });
    connect (this, &SoftwareModel::addItem, this, [=] () {
        insertRow(mData->rowCount() - 1);
        updateView();
    });
    connect (this, &SoftwareModel::reset, this, [=] () {
        beginResetModel();
        SoftwareDB::getInstance()->reset();
        endResetModel();
    });
}

int SoftwareModel::rowCount(const QModelIndex &parent) const
{
    return mData->rowCount();
}

int SoftwareModel::columnCount(const QModelIndex &parent) const
{
    return EnumSize;
}

QVariant SoftwareModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())       return {};

    auto item = mData->getItemByIndex (index.row()); //static_cast<SoftwareItem*>(index.internalPointer());
    if (!item) return {};

    if (Qt::DisplayRole == role) {
        switch (index.column()) {
            case SIndex:            return index.row() + 1;
            case SCategory:         return item->getCategory();
            case SName:             return item->getName();
            case SVersion:          return item->getVersion();
            case SInstallTime:      return item->getInstallTime();
            case SInstallPath:      return item->getInstallPath();
            default:                return {};
        }
    } else if (Qt::BackgroundRole == role) {
        if (0 == index.column()) {
//            return QColor::fromRgb(mBackgroundR, mBackgroundG, mBackgroundB);
        }
    } else if (Qt::TextAlignmentRole == role) {
        return Qt::AlignCenter;
    } else if (Qt::ForegroundRole == role) {
        switch (index.column()) {
//            case TaskDetail: {
//                return QColor(0, 0, 255);
//            }
            default:
                break;
        }
    }

    return {};
}

QVariant SoftwareModel::headerData(int section, Qt::Orientation orentation, int role) const
{
    if (Qt::DisplayRole == role && Qt::Horizontal == orentation) {
        switch (section) {
            case SIndex:
                return QString("序号");
            case SCategory:
                return QString("软件分类");
            case SName:
                return QString("软件名称");
            case SVersion:
                return QString("版本号");
            case SInstallTime:
                return QString("安装时间");
            case SInstallPath:
                return QString("安装路径");
            default:
                break;
        }

    } else if (Qt::TextAlignmentRole == role) {
        return Qt::AlignCenter;
    }
    else if (Qt::FontRole == role) {
        QFont f = qApp->font();
        f.setBold (false);
        return f;
    }

    return {};
}

QModelIndex SoftwareModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (row < 0 || row > mData->rowCount() - 1) {
            return {};
        }
        return createIndex(row, column, mData);
    }

    return {};
}

Qt::ItemFlags SoftwareModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool SoftwareModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    endInsertRows();

    return true;
}

bool SoftwareModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return QAbstractItemModel::removeRows (row, count, parent);
}

void SoftwareModel::loadApps()
{
#if 0
    GList* apps = g_app_info_get_all();
    for (auto& app = apps; app; app = app->next) {
        if (G_IS_APP_INFO(app->data)) {
            auto appID = g_app_info_get_id (G_APP_INFO(app->data));
            auto name = g_app_info_get_name (G_APP_INFO(app->data));
            auto cmd = g_app_info_get_commandline (G_APP_INFO(app->data));
            g_autoptr (GDesktopAppInfo) desktopApp = g_desktop_app_info_new (appID);
            auto category = g_desktop_app_info_get_categories (desktopApp);
            auto version = getAppVersionByCommand (cmd);
            auto item = new SoftwareItem(name, version, category, getAppInstallTimeByCommand (cmd));
            QApplication::processEvents();
            if (!version.isEmpty() && (name && strlen (name) > 0)) {
                Q_EMIT addItem (item);
            }
            else {
                delete item;
            }
        }
        g_object_unref (app->data);
    }
    g_list_free(apps);
#else
    Q_EMIT SoftwareDB::getInstance()->start();
#endif
}






