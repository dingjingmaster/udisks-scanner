//
// Created by dingjing on 23-10-12.
//

#include "process.h"

Process::Process(QObject *parent)
    : QThread(parent), mIsExit(false)
{

}

void Process::setCommand(const QString &cmd)
{
    if (!cmd.isEmpty()) {
        mCommand = cmd;
    }
}

void Process::run()
{
    mIsExit = false;
    int ret = system (mCommand.toUtf8().constData());
    mIsExit = true;

    Q_EMIT finished (ret);
}

bool Process::hasFinished() const
{
    return mIsExit;
}


