//
// Created by dingjing on 23-10-10.
//

#ifndef UDISKS_SCANNER_CONFIGURE_REPORT_H
#define UDISKS_SCANNER_CONFIGURE_REPORT_H
#include <memory>

#include <QMap>
#include <QMutex>
#include <QObject>

#include <sqlite3.h>

#include "../model/configure-item.h"


class QTimer;
class ConfigureReport : public QObject
{
    Q_OBJECT
public:
    static ConfigureReport* getInstance();
    std::shared_ptr<ConfigureItem> getItemByKey(QString& name);
    std::shared_ptr<ConfigureItem> getItemByID (int id);

private:
    explicit ConfigureReport(QObject*parent = nullptr);


Q_SIGNALS:
    void addItem (QString& name);

    void stop ();
    void start ();
    void pause ();
    void loadItemFinished ();

public Q_SLOTS:
    void reset ();

private:
    void closeSqlite();
    bool openSqlite ();
    void onDBChanged ();

private:
    sqlite3*                                        mDB{};
    QString                                         mDBPath;

    QTimer*                                         mTimer{};

    QAtomicInt                                      mIsRunning;     // 1 运行, 0 运行完毕
    QAtomicInt                                      mPause;         // 0 运行, 1 暂停, 2 停止

    QMutex                                          mLocker;        // 多余的
    QList<std::shared_ptr<ConfigureItem>>           mData;
    QMap<QString, std::shared_ptr<ConfigureItem>>   mDataIdx;

    static ConfigureReport*                         gInstance;
};


#endif //UDISKS_SCANNER_CONFIGURE_REPORT_H
