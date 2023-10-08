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

inline QString getCommandFullPath(const QString& cmd);

SoftwareModel::SoftwareModel(QAbstractTableModel *parent)
    : QAbstractTableModel (parent)
{
    connect (this, &SoftwareModel::addItem, this, [=] (SoftwareItem* item) {
        if (!item || mDataIdx.contains (item->getName())) { delete item; return; }
        mData.append (item);
        mDataIdx[item->getName()] = item;
        insertRow(mData.count() - 1);

        qDebug() << item->getName();
        updateView();
    });
}

int SoftwareModel::rowCount(const QModelIndex &parent) const
{
    return mData.count();
}

int SoftwareModel::columnCount(const QModelIndex &parent) const
{
    return EnumSize;
}

QVariant SoftwareModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())       return {};

    auto item = static_cast<SoftwareItem*>(index.internalPointer());
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
        if (row < 0 || row > mData.count() - 1) {
            return {};
        }
        return createIndex(row, column, mData[row]);
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

static QString getAppVersionByCommand(const QString& cmd);
static qint64 getAppInstallTimeByCommand(const QString& cmd);

void SoftwareModel::loadApps()
{
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
}

inline QString getPkgBin()
{
    const char* pkg[] = {
        "pacman",
        "dpkg",
        nullptr
    };

    for (int i = 0; pkg[i]; ++i) {
        auto cmd = getCommandFullPath (pkg[i]);
        if (cmd.startsWith ("/")) {
            return cmd;
        }
    }
    return nullptr;
}

inline QString getCommandFullPath(const QString& cmd)
{
    if (cmd.startsWith ("/") || cmd.startsWith ("\\\"/"))  {
        return cmd.split (" ").takeFirst().split ("%").takeFirst().trimmed();
    }

    auto cmdT = cmd.trimmed();
    if (cmd.split (" ").count() > 1) {
        cmdT = cmd.split (" ").first();
    }

    QString cmdFull = nullptr;
    auto path = QString(qgetenv ("PATH")).split (":");
    for (auto& p : path) {
        auto cmdFullT = p + "/" + cmdT;
        if (QFile::exists(cmdFullT)) {
            cmdFull = cmdFullT;
            break;
        }
    }

    return ((cmdFull != nullptr) ? cmdFull : cmd);
}

inline QString executeCommandGetResult(const QString& cmd)
{
    g_return_val_if_fail((cmd != nullptr) && !cmd.isEmpty(), "");
    qDebug() << "cmd: " << cmd;

#if 0
    QProcess pro;

    pro.start (cmd);
    pro.waitForFinished (-1);
    auto outStr = pro.readAllStandardOutput();
    qDebug() << "cmd res: " << outStr;
    return outStr;
#else
//    g_autofree char* outStr = nullptr;
//    g_autofree char* errStr = nullptr;
//    g_autoptr (GError) error = nullptr;
//    g_spawn_command_line_sync (cmd.toUtf8().constData(), &outStr, &errStr, nullptr, &error);
//    qDebug() << "cmd res: " << (outStr ? outStr : "<null>") << " -- " << (errStr ? errStr : "<null>") << (error ? error->message : "<null>");
//    return (outStr ? outStr : "");
    auto fr = popen (cmd.toUtf8().constData(), "r");
    g_return_val_if_fail(fr, "");
    QString outBuf;
    do {
        char buf[32] = {0};
        fgets (buf, sizeof(buf)/sizeof(buf[0]), fr);
        outBuf.append (buf);
    } while (!feof (fr) && !ferror (fr));
    if (fr) pclose (fr);

    return outBuf;
#endif
}

static QString getAppVersionByCommand(const QString& cmd)
{
    static QString tool = getPkgBin();
    qDebug() << "toos: " << tool;
    if (nullptr == tool) return "";

    auto cmdT = getCommandFullPath (cmd);
    if (nullptr == cmdT) return "";

    if (tool.endsWith ("dpkg")) {
        return executeCommandGetResult (QString("dpkg -s $(dpkg -S %1 | awk -F':' '{print $1}' | grep 'Version' | awk -F':' '{print $2}')").arg(cmdT)).trimmed();
    }
    else if (tool.endsWith ("pacman")) {
        return executeCommandGetResult (QString("pacman -Qo %1 | awk '{print $6}'").arg(cmdT)).trimmed();
    }

    return "";
}

static qint64 getAppInstallTimeByCommand(const QString& cmd)
{
    return 0;
    static QString tool = getPkgBin();
    qDebug() << "toos: " << tool;
    if (nullptr == tool) return 0;

    auto cmdT = getCommandFullPath (cmd);
    if (nullptr == cmdT) return 0;

    if (tool.endsWith ("dpkg")) {
        return 0; //executeCommandGetResult (QString("dpkg -s $(dpkg -S %1 | awk -F':' '{print $1}' | grep 'Version' | awk -F':' '{print $2}')").arg(cmdT)).trimmed();
    }
    else if (tool.endsWith ("pacman")) {
        //auto installTimeCMD = QString("pacman -Qo %1 | awk '{print $5}' | pacman -Qi | grep 'Install Date' | head -1 | awk -F':' '{print $2}' | date '+%%s'").arg(cmdT);
        auto installTimeCMD = QString(R"(bash -c 'pacman -Qi $(pacman -Qo %1 | awk "\'"{print $5}"\'") | grep "\'"Install Date"\'" | awk -F"\'":"\'" "\'"{print $2}"\'" | date "\'"+%%s"\'"')").arg(cmdT);
        auto ret = executeCommandGetResult (installTimeCMD).trimmed();
        if ((nullptr != ret) && !ret.isEmpty()) {
            return ret.toInt ();
        }
    }

    return 0;
}
