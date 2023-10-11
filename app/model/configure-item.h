//
// Created by dingjing on 23-10-11.
//

#ifndef UDISKS_SCANNER_CONFIGURE_ITEM_H
#define UDISKS_SCANNER_CONFIGURE_ITEM_H
#include <QString>

class ConfigureItem
{
public:
    ConfigureItem()=delete;
    explicit ConfigureItem(QString name, QString category, QString level, QString checkMethod, QString repairMethod, QString hasError);

    bool getIsError();
    QString getName();
    QString getLevel();
    QString getCategory();
    QString getCheckMethod();
    QString getRepairMethod();

private:
    QString                 mName;
    QString                 mCategory;
    QString                 mLevel;
    QString                 mCheckMethod;
    QString                 mRepairMethod;
    QString                 mIsError;
};


#endif //UDISKS_SCANNER_CONFIGURE_ITEM_H
