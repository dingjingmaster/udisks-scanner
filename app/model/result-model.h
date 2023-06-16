//
// Created by dingjing on 23-6-15.
//

#ifndef UDISKS_SCANNER_RESULT_MODEL_H
#define UDISKS_SCANNER_RESULT_MODEL_H
#include <QAbstractTableModel>
#include <QMutex>


class ResultItem;
class ResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum { FileName = 1, FilePath, ScanTime, EnumSize };
public:
    explicit ResultModel (QObject *parent = nullptr);
    ~ResultModel() override;

    void clearData();
    QModelIndex getIndexByItem (const ResultItem* it, int column=0);

    void test ();

private:
    void resetModel ();

    Q_SIGNALS:

public Q_SLOTS:
    void addItem (ResultItem* item);
    void delItem (ResultItem* item);
    void updateItem (ResultItem* item);
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
    QList<ResultItem*>                                  mData;
    QMutex                                              mDataLocker;
};

#endif //UDISKS_SCANNER_RESULT_MODEL_H
