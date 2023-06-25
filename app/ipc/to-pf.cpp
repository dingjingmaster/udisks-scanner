//
// Created by dingjing on 23-6-25.
//

#include "to-pf.h"

#include <glib.h>

#include <QDebug>

typedef struct _Message
{
    unsigned long messageType;        // 即消息索引号
    unsigned long messageLength;             // 消息头后面消息体长度
    char* data;
} Message;


ToPF* ToPF::gInstance = nullptr;

ToPF *ToPF::getInstance()
{
    static gsize gInit = 0;

    if (g_once_init_enter(&gInit)) {
        if (!gInstance) {
            gInstance = new ToPF;
        }
        g_once_init_leave(&gInit, 1);
    }

    return gInstance;
}

void ToPF::loadTask(const QString &taskFileJson)
{
    QString buf;
    if (nullptr == taskFileJson || taskFileJson.isEmpty()) {
        buf = QString("task_file:%1").arg (g_get_home_dir());
    }
    else {
        buf = QString("task_file:%1").arg (taskFileJson);
    }

    int len = buf.toUtf8().size();

    unsigned long lenA = sizeof (Message) + len + 1;
    g_autofree void* msg = (void*) malloc (lenA);
    if (nullptr == msg) {
        qCritical() << "malloc return null!";
        return;
    }
    memset (msg, 0, lenA);
    auto* msgBuf = (Message*) msg;

    msgBuf->messageType = LOAD_TASK;
    msgBuf->messageLength = len;
    memcpy (msgBuf->data, buf.toUtf8().constData(), len);

    writeToPF ((const char*) msg, (qint64) lenA);
}

ToPF::ToPF(QObject *parent)
    : QObject (parent)
{

}

void ToPF::init(QLocalSocket& socket)
{
    connect (&socket, &QLocalSocket::connected, this, [=] () {qDebug() << "connected!";});
    connect (&socket, &QLocalSocket::disconnected, this, [=] () {qDebug() << "disconnected!"; /*mLoop.exit(0);*/});
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    connect (&socket, &QLocalSocket::errorOccurred, this, [=] (QLocalSocket::LocalSocketError error) {
        bool skip = false;
        switch (error) {
            case QLocalSocket::ConnectionRefusedError: {
                qWarning() << "ConnectionRefusedError";
                break;
            }
            case QLocalSocket::PeerClosedError: {
                skip = true;
                qWarning() << "PeerClosedError";
                break;
            }
            case QLocalSocket::ServerNotFoundError: {
                qWarning() << "ServerNotFoundError";
                break;
            }
            case QLocalSocket::SocketAccessError: {
                qWarning() << "SocketAccessError";
                break;
            }
            case QLocalSocket::SocketResourceError: {
                qWarning() << "SocketResourceError";
                break;
            }
            case QLocalSocket::SocketTimeoutError: {
                qWarning() << "SocketTimeoutError";
                break;
            }
            case QLocalSocket::DatagramTooLargeError: {
                qWarning() << "DatagramTooLargeError";
                break;
            }
            case QLocalSocket::ConnectionError: {
                qWarning() << "ConnectionError";
                break;
            }
            case QLocalSocket::UnsupportedSocketOperationError: {
                qWarning() << "UnsupportedSocketOperationError";
                break;
            }
            case QLocalSocket::OperationError: {
                qWarning() << "OperationError";
                break;
            }
            case QLocalSocket::UnknownSocketError:
            default: {
                qWarning() << "UnknownSocketError";
                break;
            }
        }
        if (!skip) {
            qWarning() << "socket: " << IPC_PF_SOCKET << " error!";
//            mLoop.exit (-1);
        }
    });
#endif
    connect (&socket, &QLocalSocket::readyRead, this, [&] () {
        auto d = socket.readAll();
        qDebug() << "read:\n" << d;
    });
}

void ToPF::writeToPF(const char *buf, qint64 len)
{
    g_return_if_fail(buf && len > 0);

    qDebug() << QString("[TO PF]\n%1\n").arg (buf);
    QLocalSocket socket;

    socket.connectToServer (IPC_PF_SOCKET);

    if (!socket.waitForConnected ()) {
        qWarning() << "连接到PF失败!";
        return ;
    }

    init (socket);

    socket.write ((const char*) buf, (qint64) len);
    socket.flush();
    socket.close();
}

void ToPF::taskStart(const QString &taskID, const QString &scanDir, const QString &exceptDir)
{
    g_return_if_fail(!taskID.isNull() && !taskID.isEmpty());

    QString buf;

    buf = QString("%1:%2").arg (taskID).arg (TO_START);

    if (nullptr == scanDir || scanDir.isEmpty()) {
        buf += QString(",scan_dir:%1").arg (g_get_home_dir());
    }
    else {
        buf += QString(",scan_dir:%1").arg (scanDir);
    }

    if (nullptr != exceptDir && !exceptDir.isEmpty()) {
        buf += QString(",except_dir:%1").arg (exceptDir);
    }

    int len = buf.toUtf8().size();

    unsigned long lenA = sizeof (Message) + len + 1;
    g_autofree void* msg = (void*) malloc (lenA);
    if (nullptr == msg) {
        qCritical() << "malloc return null!";
        return;
    }
    memset (msg, 0, lenA);
    auto* msgBuf = (Message*) msg;

    msgBuf->messageType = TASK_OP;
    msgBuf->messageLength = len;
    memcpy (msgBuf->data, buf.toUtf8().constData(), len);

    writeToPF ((const char*) msg, (qint64) lenA);
}

void ToPF::taskStop(const QString &taskID)
{
    g_return_if_fail(!taskID.isNull() && !taskID.isEmpty());

    QString buf;

    buf = QString("%1:%2").arg (taskID).arg (TO_STOP);

    int len = buf.toUtf8().size();

    unsigned long lenA = sizeof (Message) + len + 1;
    g_autofree void* msg = (void*) malloc (lenA);
    if (nullptr == msg) {
        qCritical() << "malloc return null!";
        return;
    }
    memset (msg, 0, lenA);
    auto* msgBuf = (Message*) msg;

    msgBuf->messageType = TASK_OP;
    msgBuf->messageLength = len;
    memcpy (msgBuf->data, buf.toUtf8().constData(), len);

    writeToPF ((const char*) msg, (qint64) lenA);
}

void ToPF::taskPause(const QString &taskID)
{
    g_return_if_fail(!taskID.isNull() && !taskID.isEmpty());

    QString buf;

    buf = QString("%1:%2").arg (taskID).arg (TO_PAUSE);

    int len = buf.toUtf8().size();

    unsigned long lenA = sizeof (Message) + len + 1;
    g_autofree void* msg = (void*) malloc (lenA);
    if (nullptr == msg) {
        qCritical() << "malloc return null!";
        return;
    }
    memset (msg, 0, lenA);
    auto* msgBuf = (Message*) msg;

    msgBuf->messageType = TASK_OP;
    msgBuf->messageLength = len;
    memcpy (msgBuf->data, buf.toUtf8().constData(), len);

    writeToPF ((const char*) msg, (qint64) lenA);
}

void ToPF::taskResume(const QString &taskID)
{
    g_return_if_fail(!taskID.isNull() && !taskID.isEmpty());

    QString buf;

    buf = QString("%1:%2").arg (taskID).arg (TO_RESUME);

    int len = buf.toUtf8().size();

    unsigned long lenA = sizeof (Message) + len + 1;
    g_autofree void* msg = (void*) malloc (lenA);
    if (nullptr == msg) {
        qCritical() << "malloc return null!";
        return;
    }
    memset (msg, 0, lenA);
    auto* msgBuf = (Message*) msg;

    msgBuf->messageType = TASK_OP;
    msgBuf->messageLength = len;
    memcpy (msgBuf->data, buf.toUtf8().constData(), len);

    writeToPF ((const char*) msg, (qint64) lenA);
}

void ToPF::taskDelete(const QString &taskID)
{
    g_return_if_fail(!taskID.isNull() && !taskID.isEmpty());

    QString buf;

    buf = QString("%1:%2").arg (taskID).arg (TO_DEL);

    int len = buf.toUtf8().size();

    unsigned long lenA = sizeof (Message) + len + 1;
    g_autofree void* msg = (void*) malloc (lenA);
    if (nullptr == msg) {
        qCritical() << "malloc return null!";
        return;
    }
    memset (msg, 0, lenA);
    auto* msgBuf = (Message*) msg;

    msgBuf->messageType = TASK_OP;
    msgBuf->messageLength = len;
    memcpy (msgBuf->data, buf.toUtf8().constData(), len);

    writeToPF ((const char*) msg, (qint64) lenA);
}
