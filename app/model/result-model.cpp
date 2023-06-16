//
// Created by dingjing on 23-6-15.
//

#include "result-model.h"

#include <QFont>
#include <QColor>
#include <QObject>
#include <QApplication>

#include "model/result-item.h"


ResultModel::ResultModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

void ResultModel::clearData()
{

}

QModelIndex ResultModel::getIndexByItem(const ResultItem *item, int column)
{
    if (!item) {
        return {};
    }

    mDataLocker.lock();

    int rows = rowCount(QModelIndex());
    for (auto i = 0; i < rows; ++i) {
        QModelIndex ii = index(i, column);
        auto it = static_cast <const ResultItem*> (ii.internalPointer());
        if (it == item) {
            mDataLocker.unlock();
            return ii;
        }
    }

    mDataLocker.unlock();

    return {};
}

void ResultModel::test()
{
    beginResetModel();
    mDataLocker.lock();

    auto item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    item = new ResultItem("1", "file name", "/path/name/", 0);
    mData.append (item);

    endResetModel();
}

void ResultModel::resetModel()
{
    beginResetModel ();
    mDataLocker.lock();
    mData.clear ();
    mDataLocker.unlock();
    endResetModel ();
}

void ResultModel::addItem(ResultItem *item)
{
    if (!item)      return;

    mDataLocker.lock();
    mData.append(item);
    mDataLocker.unlock();
}

void ResultModel::delItem(ResultItem *item)
{
    if (!item)      return;

    QModelIndex idx = getIndexByItem (item);

    mDataLocker.lock();
    if (idx.isValid()) {
        removeRow (idx.row ());
    }
    if (mData.contains(item)) mData.removeOne (item);
    mDataLocker.unlock();
}

void ResultModel::updateItem(ResultItem *item)
{
    if (!item)      return;

    QModelIndex idx = getIndexByItem (item);
    if (idx.isValid () && (mCurIndex - 10 < idx.row()) && (mCurIndex + 30 > idx.row())) {
        QModelIndex idx1 = index (idx.row (), (int)(EnumSize) - 1);
        Q_EMIT dataChanged (idx, idx1);
    }
}

void ResultModel::onScrollbarMoved(float ratio)
{

}

int ResultModel::rowCount(const QModelIndex &parent) const
{
    return mData.count();
}

int ResultModel::columnCount(const QModelIndex &parent) const
{
    return EnumSize;
}

QVariant ResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())       return {};

    auto item = static_cast<ResultItem*>(index.internalPointer());
    if (!item)      return {};

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
        if (row < 0 || row > mData.count() - 1) {
            return {};
        }
        return createIndex(row, column, mData.at(row));
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
//    beginInsertRows(parent, row, row + count - 1);
//    endInsertRows();

    return QAbstractItemModel::removeRows (row, count, parent);
}

bool ResultModel::removeRows(int row, int count, const QModelIndex &parent)
{
//    beginRemoveRows(parent, row, row + count - 1);
//    endRemoveRows();

    return QAbstractItemModel::removeRows (row, count, parent);
}

ResultModel::~ResultModel()
{
    mDataLocker.lock();
    mData.clear();
    mDataLocker.unlock();
}
