//
// Created by dingjing on 23-10-10.
//

#ifndef UDISKS_SCANNER_CONFIGURE_CHECK_UI_H
#define UDISKS_SCANNER_CONFIGURE_CHECK_UI_H
#include <QWidget>
#include <QVBoxLayout>

class QLabel;
class QPushButton;

class ConfigureCheckUI : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigureCheckUI(QWidget* parent = nullptr);

Q_SIGNALS:
    void stop();
    void pause();
    void start();

    void reset ();
    void resizeUI();
    void updateItemCount(int c=0);

private:
    QLabel*                     mTitle;
    QWidget*                    mDetailWidget;
    QVBoxLayout*                mMainLayout;

    bool                        mIsChecked;
    QPushButton*                mShowDetail;
};


#endif //UDISKS_SCANNER_CONFIGURE_CHECK_UI_H
