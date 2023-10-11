//
// Created by dingjing on 23-10-11.
//

#include "configure-item.h"

#include <utility>

ConfigureItem::ConfigureItem(QString name, QString category, QString level, QString checkMethod, QString repairMethod, QString error)
    : mName(std::move(name)),
    mCategory(std::move(category)),
    mLevel(std::move(level)),
    mCheckMethod(std::move(checkMethod)),
    mRepairMethod(std::move(repairMethod)),
    mIsError(std::move(error))
{

}

QString ConfigureItem::getName()
{
    return ((nullptr == mName || mName.isEmpty()) ? "" : mName);
}

QString ConfigureItem::getLevel()
{
    return ((nullptr == mLevel || mLevel.isEmpty()) ? "" : mLevel);
}

QString ConfigureItem::getCategory()
{
    return ((nullptr == mCategory || mCategory.isEmpty()) ? "" : mCategory);
}

QString ConfigureItem::getCheckMethod()
{
    return ((nullptr == mCheckMethod || mCheckMethod.isEmpty()) ? "" : mCheckMethod);
}

QString ConfigureItem::getRepairMethod()
{
    return ((nullptr == mRepairMethod || mRepairMethod.isEmpty()) ? "" : mRepairMethod);
}

bool ConfigureItem::getIsError()
{
    // 0 正常, 1 不正常
    return ("0" == mIsError.trimmed());
}
