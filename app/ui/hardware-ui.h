//
// Created by dingjing on 23-10-7.
//

#ifndef UDISKS_SCANNER_HARDWARE_UI_H
#define UDISKS_SCANNER_HARDWARE_UI_H

#include <QWidget>
#include <QVBoxLayout>


class QLabel;
class QPushButton;
class HardwareUI : public QWidget
{
    Q_OBJECT
public:
    explicit HardwareUI(QWidget* parent=nullptr);

Q_SIGNALS:
    void load();
    void updateItemCount(int c=0);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QLabel*                     mDate;
    QLabel*                     mComputerName;
    QLabel*                     mOperationSystemName;
    QLabel*                     mLanguage;
    QLabel*                     mManufacturer;
    QLabel*                     mSystemVersion;
    QLabel*                     mBIOS;
    QLabel*                     mCPU;
    QLabel*                     mMem;
    QLabel*                     mSwap;
    QLabel*                     mDesktop;

    QLabel*                     mTitle;
    QVBoxLayout*                mMainLayout;

    bool                        mIsChecked;
    QPushButton*                mShowDetail;
    QWidget*                    mDetailWidget;
};


#endif //UDISKS_SCANNER_HARDWARE_UI_H
