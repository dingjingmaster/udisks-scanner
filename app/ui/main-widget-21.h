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
class MainWidget21 : public QWidget
{
    Q_OBJECT
    enum Status { Stop = 1, Running, Pause, Finished };

public:
    explicit MainWidget21(QWidget* parent=nullptr);
    void updateBaseInfo(qint64 startTime=0, qint64 stopTime=0);

protected:
    void changeStatus(Status status);

private:
    void run();

private:
    Status                  mStatus;
    QLabel*                 mBaseInfoLabel = nullptr;
    SoftwareUI*             mSoftwareUI = nullptr;

    PushButton*             mDelBtn;
    PushButton*             mExpBtn;
    PushButton*             mPauBtn;
    PushButton*             mStpBtn;
    PushButton*             mChkBtn;
};


#endif //UDISKS_SCANNER_MAIN_WIDGET_21_H
