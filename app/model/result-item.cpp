//
// Created by dingjing on 23-6-15.
//

#include "result-item.h"

#include <utility>
#include <QDateTime>

ResultItem::ResultItem(QString id, QString fileName, QString filePath, qint64 fileTime, QObject*parent)
    : QObject (parent), mID(std::move(id)), mFileName(std::move(fileName)), mFilePath(std::move(filePath)), mFileTime(fileTime)
{

}

void ResultItem::setID(QString id)
{
    mID = std::move(id);
}

void ResultItem::setFileName(QString fileName)
{
    mFileName = std::move(fileName);
}

void ResultItem::setFilePath(QString filePath)
{
    mFilePath = std::move(filePath);
}

void ResultItem::setFileTime(qint64 fileTime)
{
    mFileTime = fileTime;
}

QString ResultItem::getFileTimeStr()
{
    return QDateTime::fromMSecsSinceEpoch (getFileTime()).toString ("yyyy-MM-dd hh:mm:ss");
}

qint64 ResultItem::getFileTime()
{
    mFileTime = (mFileTime < 0) ? 0 : mFileTime;

    return mFileTime;
}

QString ResultItem::getID()
{
    return mID;
}

QString ResultItem::getFileName()
{
    return mFileName;
}

QString ResultItem::getFilePath()
{
    return mFilePath;
}
