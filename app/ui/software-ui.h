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
    int getHeight();
    explicit SoftwareUI(QWidget* parent=nullptr);

    int getSuccessItem() const;
    int getWarningItem() const;

Q_SIGNALS:
    void stop();
    void pause();
    void start();

    void reset ();
    void resizeUI();
    void updateItemCount(int success=0, int warning=0);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    int                         mSuccessItem = 0;
    int                         mWarningItem = 0;

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
