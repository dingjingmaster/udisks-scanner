//
// Created by dingjing on 23-10-10.
//

#ifndef UDISKS_SCANNER_CONFIGURE_CHECK_UI_H
#define UDISKS_SCANNER_CONFIGURE_CHECK_UI_H
#include <memory>
#include <QWidget>
#include <QProcess>
#include <QVBoxLayout>

#include "model/configure-item.h"
#include "configure-check-item-ui.h"


class QLabel;
class QPushButton;

class Process;
class ConfigureCheckUI : public QWidget
{
    Q_OBJECT
public:
    enum Type { Success, Warning };
    ConfigureCheckUI()=delete;
    explicit ConfigureCheckUI(Type type, QWidget* parent = nullptr);

    int getSuccessItem() const;
    int getWarningItem() const;

Q_SIGNALS:
    void stop();
    void pause();
    void reset ();
    void start(bool b=true);

    void addItem(std::shared_ptr<ConfigureItem>);

    void resizeUI();
    void lazyUpdate(int pos=0);
    void updateItemCount(int c=0);

private:
    int                                             mSuccessItem = 0;
    int                                             mWarningItem = 0;

    Type                                            mType;

    Process*                                        mProgress;

    QLabel*                                         mTitle;
    QWidget*                                        mDetailWidget;
    QVBoxLayout*                                    mMainLayout;

    bool                                            mIsChecked;
    QPushButton*                                    mShowDetail;

    int                                             mShowedItem = 0;
    QList<std::shared_ptr<ConfigureCheckItemUI>>    mWaringItemWidget;
    QList<std::shared_ptr<ConfigureCheckItemUI>>    mSuccessItemWidget;
};


#endif //UDISKS_SCANNER_CONFIGURE_CHECK_UI_H
