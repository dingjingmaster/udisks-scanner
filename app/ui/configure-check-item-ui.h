//
// Created by dingjing on 23-10-11.
//

#ifndef UDISKS_SCANNER_CONFIGURE_CHECK_ITEM_UI_H
#define UDISKS_SCANNER_CONFIGURE_CHECK_ITEM_UI_H
#include <QWidget>


class ConfigureCheckItemUI : public QWidget
{
    Q_OBJECT
public:
    ConfigureCheckItemUI()=delete;
    explicit ConfigureCheckItemUI(const QString& name, const QString& category, const QString& level, const QString& checkMethod, const QString& repairMethod, QWidget* parent=nullptr);
};


#endif //UDISKS_SCANNER_CONFIGURE_CHECK_ITEM_UI_H
