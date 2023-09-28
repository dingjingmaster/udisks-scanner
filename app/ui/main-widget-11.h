//
// Created by dingjing on 23-6-13.
//

#ifndef UDISKS_SCANNER_MAIN_WIDGET_11_H
#define UDISKS_SCANNER_MAIN_WIDGET_11_H
#include <QWidget>

class QComboBox;
class ScannerView;
class QVBoxLayout;
class QResizeEvent;
class ScannerTaskModel;
class MainWidget11 : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget11(QWidget* parent=nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

Q_SIGNALS:
    void checkedItem(bool c);
    void taskDetail(const QString& taskID);

private:
    QComboBox*              mComboBox = nullptr;
    QVBoxLayout*            mMainLayout = nullptr;

    ScannerView*            mView = nullptr;
    ScannerTaskModel*       mModel = nullptr;

    QStringList             mCombList;

    const int               mItemCheckSize = 40;
    const int               mItemIdxSize = 90;
    const int               mItemStartTimeSize = 180;
    const int               mItemStatusSize = 230;
    const int               mItemProgressSize = 140;
    const int               mItemDetailSize = 90;
};


#endif //UDISKS_SCANNER_MAIN_WIDGET_11_H
