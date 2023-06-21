//
// Created by dingjing on 23-6-19.
//

#ifndef UDISKS_SCANNER_SCAN_TASK_DB_H
#define UDISKS_SCANNER_SCAN_TASK_DB_H
#include <memory>
#include <QObject>


#define DB_PATH     "/tmp/scan_task.db"


class ScannerTaskItem;
class ScanTaskDBPrivate;
class ScanTaskDB : public QObject
{
    Q_OBJECT
public:
    static ScanTaskDB* getInstance();

    // start - for model
    int rowCount();
    std::shared_ptr<ScannerTaskItem> getItemByIndex(int row);
    int getRowByItem(std::shared_ptr<ScannerTaskItem> item);
    int getRowByItemID(const QString& id);
    void delItemById(const QString& id);
    void setSelectedAll(bool c);
    void setSelectedByRow(int row, bool c);
    bool isCheckAllItems();
    bool hasChecked();
    // end - for model

    // start - for test
    void testInsertItem();
    // end - for test

private:
    explicit ScanTaskDB(const QString& dbPath, QObject* parent = nullptr);

Q_SIGNALS:
    void delTasks (QList<QString>& ls);
    void addTasks (QList<QString>& ls);
    void loadTaskFinished ();

    void addOneTask ();
    void updateTask ();
    void delOneTask ();
    void resetTask ();

private:
    ScanTaskDBPrivate*          d_ptr;
    Q_DISABLE_COPY(ScanTaskDB)
    Q_DECLARE_PRIVATE(ScanTaskDB)

    static ScanTaskDB*          gInstance;
};


#endif //UDISKS_SCANNER_SCAN_TASK_DB_H
