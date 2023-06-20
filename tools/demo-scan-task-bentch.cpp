#include "app/db/scan-task-db.h"
#include <QDebug>
#include <QApplication>

int main (int argc, char* argv[])
{
    QApplication app(argc, argv);

    if (ScanTaskDB::getInstance()) {
        ScanTaskDB::getInstance()->testInsertItem ();
    }

    return 0;
}
