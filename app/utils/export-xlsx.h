//
// Created by dingjing on 23-10-13.
//

#ifndef UDISKS_SCANNER_EXPORT_XLSX_H
#define UDISKS_SCANNER_EXPORT_XLSX_H
#include <xlsxdocument.h>
#include "xlsxabstractsheet.h"

class ExportXlsx
{
public:
    ExportXlsx();

    bool addBaseInfo(const QString& taskID,
                     const QString& taskName,
                     const QString& computerName,
                     const QString& computerIP,
                     const QString& computerMAC,
                     const QString& scanStartTime,
                     const QString& scanStopTime,
                     const QString& checkItemCount,
                     const QString& successItemCount,
                     const QString& warningItemCount,
                     const QString& systemName,
                     const QString& systemLanguage,
                     const QString& systemManufacture,
                     const QString& systemModel,
                     const QString& bios,
                     const QString& cpu,
                     const QString& mem,
                     const QString& swap,
                     const QString& desktop
                     );
    bool addVnuInfo (const QString& idx,
                     const QString& name,   /*名称*/
                     const QString& number, /*号*/
                     const QString& desc,   /*描述*/
                     const QString& time,   /*时间*/
                     const QString& url     /*下载链接*/
                     );
    bool configInfo (const QString& idx,
                     const QString& category,
                     const QString& name,
                     const QString& level,
                     const QString& checkMethod,
                     const QString& repaireMethod,
                     const QString& checkResult     /* 合格  和  不合格*/
                     );

    bool softwareInfo (const QString& idx,
                       const QString& category,
                       const QString& name,
                       const QString& version,
                       const QString& installTime,
                       const QString& installPath);

    void save (const QString& path);

private:
    QXlsx::Document             mDoc;
};


#endif //UDISKS_SCANNER_EXPORT_XLSX_H
