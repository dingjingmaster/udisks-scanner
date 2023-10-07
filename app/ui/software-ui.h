//
// Created by dingjing on 23-9-27.
//

#ifndef UDISKS_SCANNER_SOFTWARE_UI_H
#define UDISKS_SCANNER_SOFTWARE_UI_H
#include <QWidget>
#include <QVBoxLayout>


class QLabel;
class QPushButton;
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

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    const int                   mItemIdxWidth = 60;
    const int                   mItemCategoryWidth = 120;
    const int                   mItemInstallTimeWidth = 180;
    const int                   mItemInstallPathWidth = 100;

    SoftwareView*               mView;
    SoftwareModel*              mModel;
    QLabel*                     mTitle;
    QVBoxLayout*                mMainLayout;

    bool                        mIsChecked;
    QPushButton*                mShowDetail;
};


#endif //UDISKS_SCANNER_SOFTWARE_UI_H