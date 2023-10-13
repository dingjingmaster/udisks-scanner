//
// Created by dingjing on 23-10-12.
//

#ifndef UDISKS_SCANNER_PROCESS_H
#define UDISKS_SCANNER_PROCESS_H
#include <QThread>

#include <pthread.h>


class Process : public QThread
{
    Q_OBJECT
public:
    explicit Process (QObject* parent=nullptr);
    void setCommand (const QString& cmd);
    bool hasFinished() const;

Q_SIGNALS:
    void finished(int exitCode);


protected:
    void run () override;

private:
    bool                    mIsExit;
    QString                 mCommand = "";
};


#endif //UDISKS_SCANNER_PROCESS_H
