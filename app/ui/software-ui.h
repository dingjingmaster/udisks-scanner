//
// Created by dingjing on 23-9-27.
//

#ifndef UDISKS_SCANNER_SOFTWARE_UI_H
#define UDISKS_SCANNER_SOFTWARE_UI_H
#include <QWidget>
#include <QVBoxLayout>


class QLabel;
class SoftwareView;
class SoftwareModel;
class SoftwareUI : public QWidget
{
    Q_OBJECT
public:
    explicit SoftwareUI(QWidget* parent=nullptr);

Q_SIGNALS:
    void load();
    void updateItemCount();

private:
    SoftwareView*               mView;
    SoftwareModel*              mModel;
    QLabel*                     mTitle;
    QVBoxLayout*                mMainLayout;
};


#endif //UDISKS_SCANNER_SOFTWARE_UI_H
