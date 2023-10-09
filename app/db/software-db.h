//
// Created by dingjing on 23-10-9.
//

#ifndef UDISKS_SCANNER_SOFTWARE_DB_H
#define UDISKS_SCANNER_SOFTWARE_DB_H
#include <memory>

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QAtomicInt>

#include "../model/software-item.h"

class SoftwareItem;
class SoftwareDB : public QObject
{
    Q_OBJECT
public:
    static SoftwareDB* getInstance();

    // for model
    int rowCount ();
    std::shared_ptr<SoftwareItem> getItemByIndex(int row);
    void delItemById(const QString& id);

    int getRowByItemID(const QString& id);
//    int getRowByItem(std::shared_ptr<SoftwareItem> item);
    // end - for model

Q_SIGNALS:
    void addItem ();

    void stop ();
    void start ();
    void pause ();
    void loadItemFinished ();

public Q_SLOTS:
    void reset ();

private:
    explicit SoftwareDB(QObject* parent = nullptr);
    void onDBChanged ();

private:
    static SoftwareDB*                                  gInstance;

    QAtomicInt                                          mPause;     // 0 运行, 1 暂停, 2 停止

    QMutex                                              mLocker;
    QList<std::shared_ptr<SoftwareItem>>                mData;
    QMap<QString, std::shared_ptr<SoftwareItem>>        mDataIdx;

    Q_DISABLE_COPY(SoftwareDB)
};


#endif //UDISKS_SCANNER_SOFTWARE_DB_H
