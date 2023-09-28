//
// Created by dingjing on 23-9-27.
//

#ifndef UDISKS_SCANNER_SOFTWARE_ITEM_H
#define UDISKS_SCANNER_SOFTWARE_ITEM_H
#include <QObject>


class SoftwareItem : public QObject
{
    Q_OBJECT
public:
    SoftwareItem()=delete;
    explicit SoftwareItem(QString  name, QString  version, QString  category, qint64 installTime, QString installPath = "/", QObject*parent=nullptr);

    QString getName() const;
    QString getVersion() const;
    QString getCategory() const;
    QString getInstallPath() const;
    QString getInstallTime() const;

private:
    QString                 mName;
    QString                 mCategory;
    QString                 mVersion;
    qint64                  mInstallTime;
    QString                 mInstallPath;
};


#endif //UDISKS_SCANNER_SOFTWARE_ITEM_H
