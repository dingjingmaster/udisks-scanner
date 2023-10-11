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
    int getHeight();

    int getSuccessItem() const;
    int getWarningItem() const;

Q_SIGNALS:
    void stop();
    void pause();
    void start();

    void reset();

    void resizeUI();
    void updateItemCount(int success=0, int warning=0);

protected:
//    void resizeEvent(QResizeEvent* event) override;

private:
    int                         mSuccessItem = 0;
    int                         mWarningItem = 0;

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
