#include "scanner-task-model.h"

#include <QFont>
#include <QSize>
#include <QDebug>
#include <QColor>
#include <QApplication>

//#include "../db/db-manager.h"
//#include "../utils/scan-task-helper.h"

ScannerTaskModel::ScannerTaskModel(QObject *parent)
    : QAbstractTableModel{parent}//, mScanTaskHelper(DBManager::instance ()->getTaskHelper ())
{
    // 数据库与model连接
//    connect(mScanTaskHelper, &ScanTaskHelper::addNewTask, this, &ScannerTaskModel::addItem);
//    connect(mScanTaskHelper, &ScanTaskHelper::updateTask, this, &ScannerTaskModel::updateItem);
//    connect(mScanTaskHelper, qOverload<ScannerTaskItem*>(&ScanTaskHelper::delOldTask), this, &ScannerTaskModel::delItem);

//    Q_EMIT DBManager::instance ()->refreshScanTask();
}

ScannerTaskModel::~ScannerTaskModel()
{
    mDataLocker.lock();
    mData.clear();
    mDataLocker.unlock();
}

QModelIndex ScannerTaskModel::getIndexByItem(const ScannerTaskItem* item, int column)
{
    if (!item) {
        qDebug() << "item is null";
        return {};
    }

    mDataLocker.lock();

    int rows = rowCount(QModelIndex());
    for (auto i = 0; i < rows; ++i) {
        QModelIndex ii = index(i, column);
        auto it = static_cast <const ScannerTaskItem*> (ii.internalPointer());
        if (it == item) {
            mDataLocker.unlock();
            return ii;
        }
    }

    qDebug() << "item not found!";
    mDataLocker.unlock();

    return {};
}

void ScannerTaskModel::resetModel()
{
    beginResetModel ();
    mDataLocker.lock();
    mData.clear ();
    mDataLocker.unlock();
    endResetModel ();

//    mScanTaskHelper->reset();
}

void ScannerTaskModel::addItem(ScannerTaskItem* item)
{
    if (!item)      return;

    mDataLocker.lock();
    mData.append(item);
    mDataLocker.unlock();

    insertRows(mData.count() - 1, 1);
}

void ScannerTaskModel::delItem(ScannerTaskItem *item)
{
    if (!item)      return;

    qInfo() << "delete task: " << item->getName();
    QModelIndex idx = getIndexByItem (item);

    mDataLocker.lock();
    if (idx.isValid()) {
        removeRow (idx.row ());
    }
    if (mData.contains(item)) mData.removeOne (item);
    mDataLocker.unlock();
}

void ScannerTaskModel::updateItem(ScannerTaskItem *item)
{
    if (!item)      return;

    QModelIndex idx = getIndexByItem (item);
    if (idx.isValid () && (mCurIndex - 10 < idx.row()) && (mCurIndex + 30 > idx.row())) {
        QModelIndex idx1 = index (idx.row (), (int)(EnumSize) - 1);
        Q_EMIT dataChanged (idx, idx1);
    }
}

int ScannerTaskModel::rowCount(const QModelIndex &parent) const
{
    return mData.count();
}

int ScannerTaskModel::columnCount(const QModelIndex &parent) const
{
    return EnumSize;
}

QVariant ScannerTaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())       return {};

    auto item = static_cast<ScannerTaskItem*>(index.internalPointer());
    if (!item)      return {};

//    enum { TaskIndex = 1, TaskName, TaskPolicy, TaskScanPath, TaskStartTime, TaskStatus, TaskProgress, TaskDetail, EnumSize };
    if (Qt::DisplayRole == role) {
        switch (index.column()) {
            case TaskIndex:     return index.row() + 1;
            case TaskName:      return item->getName();
            case TaskPolicy:    return item->getPolicy().join (";");
            case TaskScanPath:  return item->getScanPath();
            case TaskStartTime: return item->getStartTimeStr();
            case TaskStatus:    return item->getStatus();
            case TaskProgress:  return item->getScanProgress();
            case TaskDetail:    return "详情";
            default:            return {};
        }
    } else if (Qt::BackgroundRole == role) {
        if (0 == index.column()) {
            return QColor::fromRgb(mBackgroundR, mBackgroundG, mBackgroundB);
        }
    } else if (Qt::TextAlignmentRole == role) {
        return Qt::AlignCenter;
    } else if (Qt::ForegroundRole == role) {
        switch (index.column()) {
        case TaskDetail: {
            return QColor(0, 0, 255);
        }
        default:
            break;
        }
    }
    /*
    else if (Qt::CheckStateRole == role) {
        if (0 == index.column()) {
            return false;
        }
    }
    else if (Qt::EditRole == role) {
        if (0 == index.column()) {
            return true;
        }
    }
     */

    return {};
}

QVariant ScannerTaskModel::headerData(int section, Qt::Orientation orentation, int role) const
{
    if (Qt::DisplayRole == role && Qt::Horizontal == orentation) {
        switch (section) {
        case 0:
            break;
        case 1:
            return QString("序号");
        case 2:
            return QString("任务名称");
        case 3:
            return QString("应用策略");
        case 4:
            return QString("检查路径");
        case 5:
            return QString("开始时间");
        case 6:
            return QString("任务状态");
        case 7:
            return QString("扫描进度");
        case 8:
            return QString("任务详情");
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

QModelIndex ScannerTaskModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (row < 0 || row > mData.count() - 1) {
            return {};
        }
        return createIndex(row, column, mData.at(row));
    }

    return {};
}

Qt::ItemFlags ScannerTaskModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ScannerTaskModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    endInsertRows();

    return true;
}

bool ScannerTaskModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    endRemoveRows();

    return true;
}

void ScannerTaskModel::onScrollbarMoved(float ratio)
{
    mCurIndex = ratio * rowCount(QModelIndex()) + 1;
}

void ScannerTaskModel::clearData()
{
    beginResetModel ();
    mDataLocker.lock();
    mData.clear ();
    mDataLocker.unlock();
    endResetModel ();

//    mScanTaskHelper->reset();
}

void ScannerTaskModel::test()
{
    beginResetModel();
    mDataLocker.lock();

    auto item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    item = new ScannerTaskItem("1", "name 1", QStringList()<<"1"<<"2", "path1", 0, ScannerTaskItem::Unknow, 0);
    mData.append(item);

    mDataLocker.unlock();
    endResetModel();
}
