//
// Created by dingjing on 23-6-26.
//

#ifndef UDISKS_SCANNER_SCAN_RESULT_EXPORT_H
#define UDISKS_SCANNER_SCAN_RESULT_EXPORT_H
#include <QObject>

class ScanResultExport : public QObject
{
    Q_OBJECT
public:
    static ScanResultExport* getInstance ();

private:
    explicit ScanResultExport(QObject* parent=nullptr);

private:
    static ScanResultExport*        gInstance;

};


#endif //UDISKS_SCANNER_SCAN_RESULT_EXPORT_H
