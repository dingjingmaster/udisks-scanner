//
// Created by dingjing on 23-6-15.
//

#include "result-model.h"

#include <glib.h>

#include <QFont>
#include <QDebug>
#include <QColor>
#include <QObject>
#include <QApplication>

#include "model/result-item.h"
#include "db/scan-result-db.h"


ResultModel::ResultModel(QObject *parent)
    : QAbstractTableModel(parent),
    mScanResultDB(ScanResultDB::getInstance())
{
    connect (mScanResultDB, &ScanResultDB::addTasksResult, this, [=] (QList<QString>& ls) {
        beginInsertRows (QModelIndex(), mScanResultDB->rowCount(), mScanResultDB->rowCount() + ls.count() - 1);
        endInsertRows();
    });

    connect (mScanResultDB, &ScanResultDB::delTaskResult, this, [=] (QList<QString>& ls) {
        for (const auto& it : ls) {
            auto row = mScanResultDB->getRowByItemID(it);
            beginRemoveRows (QModelIndex(), row, 1);
            endRemoveRows();
        }
    });
}

void ResultModel::clearData()
{

}

QModelIndex ResultModel::getIndexByItem(const ResultItem *item, int column)
{
    if (!item) {
        return {};
    }

    return {};
}

void ResultModel::test()
{
    beginResetModel();

    endResetModel();
}

void ResultModel::resetModel()
{
    beginResetModel ();

    endResetModel ();
}

void ResultModel::onScrollbarMoved(float ratio)
{

}

int ResultModel::rowCount(const QModelIndex &parent) const
{
    g_return_val_if_fail(mScanResultDB, 0);

    return mScanResultDB->rowCount();
}

int ResultModel::columnCount(const QModelIndex &parent) const
{
    return EnumSize;
}

QVariant ResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())       return {};

    auto idb = static_cast<ScanResultDB*>(index.internalPointer());
    if (!idb)      return {};
    auto item = idb->getItemByIndex (index.row());

    if (Qt::DisplayRole == role) {
        switch (index.column()) {
            case 0:             return index.row() + 1;
            case FileName:      return item->getFileName();
            case FilePath:      return item->getFilePath();
            case ScanTime:      return item->getFileTimeStr();
            default:            return {};
        }
    } else if (Qt::BackgroundRole == role) {
        if (0 == index.column()) {
            return QColor::fromRgb(mBackgroundR, mBackgroundG, mBackgroundB);
        }
    } else if (Qt::TextAlignmentRole == role) {
        return Qt::AlignCenter;
    }

    return {};
}

QVariant ResultModel::headerData(int section, Qt::Orientation orentation, int role) const
{
    if (Qt::DisplayRole == role && Qt::Horizontal == orentation) {
        switch (section) {
            case 0:
                return QString("序号");
            case 1:
                return QString("文件名");
            case 2:
                return QString("文件路径");
            case 3:
                return QString("时间");
            default:
                break;
        }

    } else if (Qt::BackgroundRole == role) {
        return QColor::fromRgb(mBackgroundR, mBackgroundG, mBackgroundB);
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

QModelIndex ResultModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (row < 0 || row > mScanResultDB->rowCount() - 1) {
            return {};
        }
        return createIndex(row, column, mScanResultDB);
    }

    return {};
}

Qt::ItemFlags ResultModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ResultModel::insertRows(int row, int count, const QModelIndex &parent)
{
    QAbstractTableModel::insertRows(row, count, parent);
}

bool ResultModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return QAbstractItemModel::removeRows (row, count, parent);
}

ResultModel::~ResultModel()
{
}

void ResultModel::delItem(const std::shared_ptr<ResultItem> &item)
{
    if (!item)      return;

//    qInfo() << "delete task: " << item->getFileName();

//    mScanResultDB->delItemById (item->getID());
}
