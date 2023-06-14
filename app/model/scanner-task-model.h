#ifndef SCANNERTASKMODEL_H
#define SCANNERTASKMODEL_H

#include <QAbstractTableModel>
#include <QtCore/QMutex>

#include "scanner-task-item.h"

//class ScanTaskHelper;

class ScannerTaskModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum { TaskIndex = 1, TaskName, TaskPolicy, TaskScanPath, TaskStartTime, TaskStatus, TaskProgress, TaskDetail, EnumSize };
public:
    explicit ScannerTaskModel(QObject *parent = nullptr);
    ~ScannerTaskModel() override;

    void clearData();
    QModelIndex getIndexByItem (const ScannerTaskItem* it, int column=0);

    void test ();

private:
    void resetModel ();

Q_SIGNALS:

public Q_SLOTS:
    void addItem (ScannerTaskItem* item);
    void delItem (ScannerTaskItem* item);
    void updateItem (ScannerTaskItem* item);
    void onScrollbarMoved (float ratio);

public:
    [[nodiscard]] int rowCount (const QModelIndex& parent) const override;
    [[nodiscard]] int columnCount (const QModelIndex& parent) const override;

    [[nodiscard]] QVariant data (const QModelIndex& index, int role=Qt::DisplayRole) const override;
    [[nodiscard]] QVariant headerData (int section, Qt::Orientation orentation, int role) const override;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] Qt::ItemFlags flags (const QModelIndex& index) const override;

    bool insertRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    const int                                           mBackgroundR = 235;
    const int                                           mBackgroundG = 241;
    const int                                           mBackgroundB = 248;

    int                                                 mCurIndex = 0;

//    ScanTaskHelper*                                     mScanTaskHelper;

    // ScannerxxItem
    QList<ScannerTaskItem*>                             mData;
    QMutex                                              mDataLocker;
};

#endif // SCANNERTASKMODEL_H
