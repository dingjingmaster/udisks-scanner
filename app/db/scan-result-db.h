//
// Created by dingjing on 23-6-21.
//

#ifndef UDISKS_SCANNER_SCAN_RESULT_DB_H
#define UDISKS_SCANNER_SCAN_RESULT_DB_H
#include <memory>
#include <QObject>

class ResultItem;
class ScanResultDBPrivate;
class ScanResultDB : public QObject
{
    Q_OBJECT
public:
    static ScanResultDB* getInstance();

    // start - for model
    int rowCount();
    std::shared_ptr<ResultItem> getItemByIndex(int row);
    int getRowByItem(std::shared_ptr<ResultItem> item);
    int getRowByItemID(const QString& id);
    void setTaskID(const QString& id);
    // end - for model

    // for - export
    void exportResultByTaskID (const QString& file, const QStringList& ids);
    // for - export

    // start - for test
    void testInsertItem();
    // end - for test

private:
    explicit ScanResultDB (const QString& dbPath, QObject* parent=nullptr);

Q_SIGNALS:
    void delTaskResult (QList<QString>& ls);
    void addTasksResult (QList<QString>& ls);
    void loadTaskResultFinished ();

private:
    ScanResultDBPrivate*            d_ptr;
    static ScanResultDB*            gInstance;
    Q_DISABLE_COPY(ScanResultDB)
    Q_DECLARE_PRIVATE(ScanResultDB)
};


#endif //UDISKS_SCANNER_SCAN_RESULT_DB_H
