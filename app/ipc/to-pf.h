//
// Created by dingjing on 23-6-25.
//

#ifndef UDISKS_SCANNER_TO_PF_H
#define UDISKS_SCANNER_TO_PF_H
#include <QObject>
#include <QEventLoop>
#include <QLocalSocket>


class ToPF : public QObject
{
    Q_OBJECT
    enum CommandID      {LOAD_TASK=10040, TASK_OP};
    enum TaskOperation  {TO_START=1, TO_PAUSE, TO_RESUME, TO_STOP, TO_DEL};
public:
    static ToPF* getInstance();

    void loadTask (const QString& taskFileJson);
    void taskStart(const QString& taskID, const QString& scanDir, const QString& exceptDir);
    void taskStop (const QString& taskID);
    void taskPause(const QString& taskID);
    void taskResume(const QString& taskID);
    void taskDelete(const QString& taskID);
    void taskDelete(const QStringList& taskID);

private:
    void init(QLocalSocket& socket);
    void writeToPF(const char* buf, qint64 len);
    explicit ToPF(QObject*parent=nullptr);

private:

    static ToPF*        gInstance;
};


#endif //UDISKS_SCANNER_TO_PF_H
