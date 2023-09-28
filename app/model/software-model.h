//
// Created by dingjing on 23-9-27.
//

#ifndef UDISKS_SCANNER_SOFTWARE_MODEL_H
#define UDISKS_SCANNER_SOFTWARE_MODEL_H
#include <QAbstractTableModel>


class SoftwareItem;
class SoftwareModel : public QAbstractTableModel
{
    Q_OBJECT
    enum { SIndex = 0, SCategory, SName, SVersion, SInstallTime, SInstallPath, EnumSize };
public:
    explicit SoftwareModel(QAbstractTableModel*parent=nullptr);

public Q_SLOTS:
    void loadApps();

Q_SIGNALS:
    void updateView();
    void addItem(SoftwareItem* item);

public:
    [[nodiscard]] int rowCount (const QModelIndex& parent=QModelIndex()) const override;
    [[nodiscard]] int columnCount (const QModelIndex& parent=QModelIndex()) const override;

    [[nodiscard]] QVariant data (const QModelIndex& index, int role=Qt::DisplayRole) const override;
    [[nodiscard]] QVariant headerData (int section, Qt::Orientation orentation, int role) const override;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] Qt::ItemFlags flags (const QModelIndex& index) const override;

    bool insertRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QList<SoftwareItem*>                    mData;
    QMap<QString, SoftwareItem*>            mDataIdx;
};


#endif //UDISKS_SCANNER_SOFTWARE_MODEL_H
