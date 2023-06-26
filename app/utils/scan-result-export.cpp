//
// Created by dingjing on 23-6-26.
//

#include "scan-result-export.h"

ScanResultExport *ScanResultExport::getInstance()
{
    return nullptr;
}

ScanResultExport::ScanResultExport(QObject *parent)
    : QObject (parent)
{

}
