//
// Created by dingjing on 23-9-27.
//

#ifndef UDISKS_SCANNER_MAIN_WIDGET_21_H
#define UDISKS_SCANNER_MAIN_WIDGET_21_H
#include <QWidget>
#include <QVBoxLayout>

class QLabel;

class PushButton;
class SoftwareUI;
class HardwareUI;
class QScrollArea;
class QProgressBar;
class MainWidget21 : public QWidget
{
    Q_OBJECT
    enum Status { Stop = 1, Running, Pause, Finished };

public:
    explicit MainWidget21(QWidget* parent=nullptr);
    void updateBaseInfo(qint64 startTime=0, qint64 stopTime=0);

protected:
    void changeStatus(Status status);
    void resizeEvent(QResizeEvent*) override;

Q_SIGNALS:
    void allFinished();

private:
    void run();

private Q_SLOTS:
    void resizeResultUI();

private:
    Status                  mStatus;
    QLabel*                 mBaseInfoLabel = nullptr;
    SoftwareUI*             mSoftwareUI = nullptr;
    HardwareUI*             mHardwareUI = nullptr;

    PushButton*             mDelBtn;
    PushButton*             mExpBtn;
    PushButton*             mPauBtn;
    PushButton*             mStpBtn;
    PushButton*             mChkBtn;

    QWidget*                mScrollWidget = nullptr;
    QScrollArea*            mScrollArea = nullptr;

    int                     mMaxProgress = 0;           // 假进度条的最大值， 最大是 100
    int                     mMinProgress = 0;           //
    QTimer*                 mProgressTimer = nullptr;
    QProgressBar*           mProgress = nullptr;
};


#endif //UDISKS_SCANNER_MAIN_WIDGET_21_H
