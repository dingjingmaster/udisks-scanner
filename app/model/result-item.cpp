//
// Created by dingjing on 23-6-15.
//

#include "result-item.h"

#include <QDebug>
#include <utility>
#include <QRegExp>
#include <QDateTime>
#include <QCryptographicHash>


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
    return QDateTime::fromSecsSinceEpoch (getFileTime()).toString ("yyyy-MM-dd hh:mm:ss");
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

void ResultItem::setContext(const QString& context)
{
    mContext = context;
}

QString ResultItem::getContext()
{
    QString ctx = QByteArray::fromBase64 (mContext.toUtf8());

    qDebug() << "ctx: " << ctx;

    QString keyWorld = "";
    QRegExp rx(R"(@\d{4})");
//    QRegExp rKeyWorld("\\d{4}(:|：)");
//    int keyPosS = rKeyWorld.indexIn (ctx, 0);
//    if (keyPosS > -1) {
//        keyPosS += rKeyWorld.matchedLength();
//
//        int keyPosE = rx.indexIn (ctx, 0);
//        if (keyPosE <= 0) {
//            keyPosE = 0;
//        }
//
//        if (keyPosE > keyPosS) {
//            keyWorld = ctx.mid (keyPosS, keyPosE - keyPosS);
//        }
//    }
    int pos = 0;
    if ((pos = rx.indexIn (ctx, 0)) > 0) {
        pos += rx.matchedLength();
    }

    return ((pos > 0) ? ctx.mid (pos) : ctx);
}
