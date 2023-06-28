#include "app/db/scan-task-db.h"

#include <glib.h>

#include <QDebug>
#include <QApplication>

#include "common/global.h"

char* gDBPath = nullptr;

int main (int argc, char* argv[])
{
    char* path1 = nullptr;
    char* pathDir = nullptr;
    if (g_str_has_prefix(argv[0], "/")) {
        path1 = g_strdup(argv[0]);
    }
    else {
        path1 = g_strdup_printf("%s/%s", g_get_current_dir(), argv[0]);
    }

    g_return_val_if_fail(path1, -1);
    pathDir = g_path_get_dirname (path1);
    g_return_val_if_fail(pathDir, -2);

    gDBPath = g_strdup_printf ("%s/../dat/db_task/EstDlpSEDataBase.db", pathDir);
    if (path1)      g_free (path1);
    if (pathDir)    g_free (pathDir);

    QApplication app(argc, argv);

    if (ScanTaskDB::getInstance()) {
        ScanTaskDB::getInstance()->testInsertItem ();
    }

    return 0;
}
