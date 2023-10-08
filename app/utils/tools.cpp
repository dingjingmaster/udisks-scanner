//
// Created by dingjing on 23-6-19.
//
#include "tools.h"

#include <QFile>

#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/utsname.h>

#include <glib.h>
#include <QDebug>
#include <fcntl.h>
#include <unistd.h>
#include <glibtop.h>
#include <gio/gio.h>
#include <sys/file.h>
#include <glibtop/mem.h>
#include <glibtop/swap.h>
#include <glibtop/sysinfo.h>
#include <glibtop/sysdeps.h>

TaskDBLock::TaskDBLock()
    : mFile(nullptr), mIsLock(false)
{
}

void TaskDBLock::lock()
{
    while (!lock1()) g_usleep (600);
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

    mLocker.lock();
    if (0 == flock(mFile->_fileno, LOCK_EX | LOCK_NB)) {
        mIsLock = true;
    }
    mLocker.unlock();

    return mIsLock;
}

bool TaskDBLock::unlock1()
{
    mLocker.lock();
    if (0 == flock (mFile->_fileno, LOCK_UN)) {
        mIsLock = false;
    }
    mLocker.unlock();

    return !mIsLock;
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


QString getHostPropertyByDbus (const QString& propertyName)
{
    g_autoptr(GDBusProxy) hostnamedProxy = nullptr;
    g_autoptr(GVariant) propertyVariant = nullptr;
    g_autoptr(GError) error = nullptr;

    hostnamedProxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
                                                     G_DBUS_PROXY_FLAGS_NONE,
                                                     nullptr,
                                                     "org.freedesktop.hostname1",
                                                     "/org/freedesktop/hostname1",
                                                     "org.freedesktop.hostname1",
                                                     nullptr,
                                                     &error);
    if (hostnamedProxy == nullptr) {
        g_debug ("Couldn't get host named to start, bailing: %s", error->message);
        return "";
    }

    propertyVariant = g_dbus_proxy_get_cached_property (hostnamedProxy, propertyName.toUtf8().constData());
    if (!propertyVariant) {
        g_debug ("Unable to retrieve org.freedesktop.hostname1.%s property", propertyName.toUtf8().constData());
        return "";
    }

    g_autofree char* resStr = g_variant_dup_string (propertyVariant, nullptr);

    return resStr;
}


typedef struct
{
    QString     regex;
    QString     replacement;
} ReplaceStrings;

static char* prettify_info (const char *info)
{
    g_autofree char *escaped = NULL;
    g_autofree gchar *pretty = NULL;
    int   i;
    static const ReplaceStrings rs[] = {
        { "Mesa DRI ", ""},
        { "Mesa Intel", "Intel"},
        { "[(]R[)]", "\302\256"},
        { "[(](tm|TM)[)]", "\342\204\242"},
        { "(ATI|EPYC|AMD FX|Radeon|Ryzen|Threadripper|GeForce RTX) ", "\\1\342\204\242 "},
        { "Gallium \\d+\\.\\d+ on (.*)", "\\1"},
        { " CPU| Processor| \\S+-Core| @ \\d+\\.\\d+GHz", ""},
        { " x86|/MMX|/SSE2|/PCIe", ""},
        { " [(][^)]*(DRM|MESA|LLVM)[^)]*[)]?", ""},
        { "Graphics Controller", "Graphics"},
        { ".*llvmpipe.*", "Software Rendering"},
    };

    if (*info == '\0') {
        return nullptr;
    }

    escaped = g_markup_escape_text (info, -1);
    pretty = g_strdup (g_strstrip (escaped));

    for (i = 0; i < G_N_ELEMENTS (rs); i++) {
        g_autoptr(GError) error = nullptr;
        g_autoptr(GRegex) re = nullptr;
        g_autofree gchar* newStr = nullptr;

        re = g_regex_new (rs[i].regex.toUtf8().constData(), static_cast<GRegexCompileFlags>(0), static_cast<GRegexMatchFlags>(0), &error);
        if (re == nullptr) {
            g_warning ("Error building regex: %s", error->message);
            continue;
        }

        newStr = g_regex_replace (re, pretty, -1, 0, rs[i].replacement.toUtf8().constData(), static_cast<GRegexMatchFlags>(0), &error);
        if (error != nullptr) {
            g_warning ("Error replacing %s: %s", rs[i].regex.toUtf8().constData(), error->message);
            continue;
        }

        g_free (pretty);
        pretty = g_strdup(newStr);
    }

    return g_strdup(pretty);
}

static char* remove_duplicate_whitespace (const char *old)
{
    g_autofree gchar* newStr = nullptr;
    g_autoptr(GRegex) re = nullptr;
    g_autoptr(GError) error = nullptr;

    if (old == nullptr) {
        return nullptr;
    }

    re = g_regex_new ("[ \t\n\r]+", G_REGEX_MULTILINE, static_cast<GRegexMatchFlags>(0), &error);
    if (re == nullptr) {
        g_warning ("Error building regex: %s", error->message);
        return g_strdup (old);
    }
    newStr = g_regex_replace (re, old, -1, 0, " ", static_cast<GRegexMatchFlags>(0), &error);
    if (newStr == nullptr) {
        g_warning ("Error replacing string: %s", error->message);
        return g_strdup (old);
    }

    return g_strdup(newStr);
}

char* info_cleanup (const char *input)
{
    g_autofree char *pretty = nullptr;

    pretty = prettify_info (input);
    return remove_duplicate_whitespace (pretty);
}

QString getCpuInfo ()
{
    g_autoptr(GHashTable) counts = nullptr;
    g_autoptr(GString) cpu = nullptr;
    int            i;
    int            j;
    GHashTableIter iter;
    gpointer       key, value;
    const glibtop_sysinfo* info;

    counts = g_hash_table_new (g_str_hash, g_str_equal);
    info = glibtop_get_sysinfo ();

    /* count duplicates */
    for (i = 0; i != (int) info->ncpu; ++i) {
        const char * const keys[] = { "model name", "cpu", "Processor", "Model Name" };
        char *model;
        int  *count;

        model = nullptr;

        for (j = 0; model == nullptr && j != G_N_ELEMENTS (keys); ++j) {
            model = static_cast<char *>(g_hash_table_lookup ((GHashTable*)(info->cpuinfo[i].values), keys[j]));
        }

        if (model == nullptr) {
            continue;
        }

        count = static_cast<int *>(g_hash_table_lookup (counts, model));
        if (count == nullptr) {
            g_hash_table_insert (counts, model, GINT_TO_POINTER (1));
        }
        else {
            g_hash_table_replace (counts, model, GINT_TO_POINTER (GPOINTER_TO_INT (count) + 1));
        }
    }

    cpu = g_string_new (nullptr);
    g_hash_table_iter_init (&iter, counts);
    while (g_hash_table_iter_next (&iter, &key, &value)) {
        g_autofree char *cleanedup = nullptr;
        int count;

        count = GPOINTER_TO_INT (value);
        cleanedup = info_cleanup ((const char *) key);
        if (cpu->len != 0) {
            g_string_append_printf (cpu, " ");
        }
        if (count > 1) {
            g_string_append_printf (cpu, "%s \303\227 %d", cleanedup, count);
        }
        else {
            g_string_append_printf (cpu, "%s", cleanedup);
        }
    }

    return cpu->str;
}

QString getMemInfo()
{
    glibtop_mem mem;
    glibtop_get_mem (&mem);

    g_autofree char* memStr = g_format_size_full (mem.total, G_FORMAT_SIZE_IEC_UNITS);

    return memStr;
}

QString getSwapInfo()
{
    glibtop_swap swap;
    glibtop_get_swap (&swap);

    g_autofree char* swapStr = g_format_size_full (swap.total, G_FORMAT_SIZE_IEC_UNITS);

    return swapStr;
}

QString getDesktopInfo()
{
    const char* desktop = g_getenv("XDG_CURRENT_DESKTOP");
    if (nullptr == desktop) {
        desktop = g_getenv ("XDG_SESSION_DESKTOP");
        if (nullptr == desktop) {
            desktop = g_getenv("DESKTOP_SESSION");
        }
    }

    const char* uiSys = g_getenv ("XDG_SESSION_TYPE");

    return QString("%1 %2").arg((desktop ? desktop : ""), (uiSys ? uiSys : ""));
}

QString getOSName()
{
    struct utsname info{};

    auto osName = "";

    if (-1 != uname (&info)) {
        osName = info.nodename;
    }

    return osName;
}

QString getOSType()
{
    struct utsname info{};

    QString machine = "";

    if (-1 != uname (&info)) {
        machine = QString("%1 %2").arg (((strlen (info.sysname) > 0) ? info.sysname : ""),
                                        ((strlen (info.machine) > 0) ? info.machine : ""));
    }

    return machine;
}

QString getVendor()
{
    QString vendor = getHostPropertyByDbus ("HardwareVendor")
        + " " + getHostPropertyByDbus("HardwareModel");
    if (vendor.trimmed().isEmpty()) {
#define VENDOR_FILE         "/sys/class/dmi/id/sys_vendor"
        QFile f(VENDOR_FILE);
        if (f.exists() && f.open (QFile::ReadOnly)) {
            vendor = f.readAll().trimmed();
        }
    }
    return vendor;
}

QString getBIOS()
{
    QString bios = getHostPropertyByDbus ("FirmwareVendor")
        + " " + getHostPropertyByDbus ("FirmwareVersion");
    if (bios.trimmed().isEmpty()) {
#define BIOS_FILE1          "/sys/class/dmi/id/bios_vendor"
#define BIOS_FILE2          "/sys/class/dmi/id/bios_version"
        QFile f1(BIOS_FILE1);
        QFile f2(BIOS_FILE2);
        if (f1.exists() && f1.open(QFile::ReadOnly)) {
            bios = f1.readAll().trimmed();
        }
        if (f2.exists() && f2.open (QFile::ReadOnly)) {
            if (!bios.isEmpty()) {
                bios += "  ";
            }
            bios += f2.readAll().trimmed();
        }
    }
    return bios;
}
