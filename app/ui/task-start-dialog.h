//
// Created by dingjing on 23-6-25.
//

#ifndef UDISKS_SCANNER_TASK_START_DIALOG_H
#define UDISKS_SCANNER_TASK_START_DIALOG_H
#include <QDialog>


class QVBoxLayout;
class TaskStartDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TaskStartDialog(QWidget*parent=nullptr);
    QString getScanDir();
    QString getExceptDir();

private:
    QString                     mScanDir;
    QString                     mExceptDir;
    QVBoxLayout*                mMainLayout = nullptr;
};


#endif //UDISKS_SCANNER_TASK_START_DIALOG_H
