//
// Created by dingjing on 23-6-15.
//

#ifndef UDISKS_SCANNER_TASK_TRACE_H
#define UDISKS_SCANNER_TASK_TRACE_H
#include <QWidget>
#include <QVBoxLayout>


class TaskTrace : public QWidget
{
    Q_OBJECT
public:
    explicit TaskTrace(QWidget* parent=nullptr);

private:
    const int               mFixedWidth = 200;
    QVBoxLayout*            mMainLayout = nullptr;
};


#endif //UDISKS_SCANNER_TASK_TRACE_H
