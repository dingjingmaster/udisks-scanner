//
// Created by dingjing on 23-6-19.
//
#include "tools.h"

#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include <glib.h>
#include <QDebug>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>


TaskDBLock*  TaskDBLock::gInstance = nullptr;

TaskDBLock::TaskDBLock()
    : mFile(nullptr), mIsLock(false)
{
    init();
}

void TaskDBLock::lock()
{
    unlock();
    while (!lock1()) { g_usleep (600); }
}

void TaskDBLock::unlock()
{
    while (!unlock1());
}

void TaskDBLock::init()
{
    if (!mFile) {
        mFile = fopen (TASK_DB_LOCKER, "a+");
        if (!mFile) {
            qWarning() << "lock file: " << TASK_DB_LOCKER;
        }
    }
}

TaskDBLock::~TaskDBLock()
{
    if (mIsLock) {
        unlock();
    }

    if (mFile) {
        fclose (mFile);
    }
}

bool TaskDBLock::lock1()
{
    init();
    g_return_val_if_fail(mFile, false);

    QMutexLocker l (&mLocker);
    if (0 == flock(mFile->_fileno, LOCK_EX | LOCK_NB)) {
        mIsLock = true;
    }

    return mIsLock;
}

bool TaskDBLock::unlock1()
{
    QMutexLocker l (&mLocker);
    if (0 == flock (mFile->_fileno, LOCK_UN)) {
        mIsLock = false;
    }

    return !mIsLock;
}

TaskDBLock *TaskDBLock::getInstance()
{
    if (!gInstance) {
        static QMutex lock;
        QMutexLocker l(&lock);
        if (!gInstance) {
            gInstance = new TaskDBLock;
        }
    }

    return gInstance;
}

QString getLocalIP()
{
    char buf[64] = {0};
    auto name = getLocalPCName();

    struct hostent* hp = gethostbyname(name.toUtf8().constData());
    if (nullptr == hp) {
        return "";
    }

    inet_ntop (hp->h_addrtype, hp->h_addr, buf, sizeof(buf) - 1);

    return buf;
}

QString getLocalPCName()
{
    char name[1024] = {0};

    gethostname (name, sizeof (name) - 1);

    return name;
}

QString getLocalMAC()
{
    QSet<QString> macs;

    int ifaceNum = 0;
    char ipT[33] = {0};
    struct ifreq ifrcopy{};
    struct ifconf ifc = {0};
    struct ifreq buf[10] = {0};
    char mac[48] = {0};

    int fd = socket (AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        close (fd);
        return "";
    }

    ifc.ifc_len = sizeof (buf);
    ifc.ifc_buf = (caddr_t) buf;
    if (!ioctl(fd, SIOCGIFCONF, (char*) & ifc)) {
        ifaceNum = ifc.ifc_len / sizeof (struct ifreq);
        while (ifaceNum -- > 0) {
            ifrcopy = buf[ifaceNum];
            if (ioctl (fd, SIOCGIFFLAGS, &ifrcopy)) {
                goto end;
            }

            if (!ioctl (fd, SIOCGIFHWADDR, (char*) (&buf[ifaceNum]))) {
                memset (mac, 0, sizeof (mac));
                snprintf (mac, sizeof(mac), "%02X-%02X-%02X-%02X-%02X-%02X",
                          (unsigned char) buf[ifaceNum].ifr_hwaddr.sa_data[0],
                          (unsigned char) buf[ifaceNum].ifr_hwaddr.sa_data[1],
                          (unsigned char) buf[ifaceNum].ifr_hwaddr.sa_data[2],
                          (unsigned char) buf[ifaceNum].ifr_hwaddr.sa_data[3],
                          (unsigned char) buf[ifaceNum].ifr_hwaddr.sa_data[4],
                          (unsigned char) buf[ifaceNum].ifr_hwaddr.sa_data[5]
                );
                if (g_strcmp0 ("00-00-00-00-00-00", mac)) macs << mac;
            }
        }
    }

end:
    if (fd > 0) close (fd);

    return ((macs.count() >= 1) ? macs.values().at (0) : "");
}
