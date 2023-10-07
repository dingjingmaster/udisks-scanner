//
// Created by dingjing on 23-9-27.
//

#include "software-item.h"

#include <utility>

#include <QDateTime>


SoftwareItem::SoftwareItem(QString name, QString version, QString category, qint64 installTime, QString  installPath, QObject *parent)
    : QObject (parent),
    mName(std::move(name)),
    mInstallTime(installTime),
    mVersion(std::move(version)),
    mCategory(std::move(category)),
    mInstallPath(std::move(installPath))
{

}

QString SoftwareItem::getName() const
{
    return mName;
}

QString SoftwareItem::getVersion() const
{
    return mVersion;
}

QString SoftwareItem::getCategory() const
{
    return mCategory;
}

QString SoftwareItem::getInstallPath() const
{
    return mInstallPath;
}

QString SoftwareItem::getInstallTime() const
{
    if (mInstallTime > 0) {
        return QDateTime::fromSecsSinceEpoch (mInstallTime).toString ("yyyy-MM-dd hh:mm:ss");
    }
    else {
        return "未知";
    }
}
