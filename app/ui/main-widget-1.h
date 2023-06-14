//
// Created by dingjing on 23-6-13.
//

#ifndef UDISKS_SCANNER_MAIN_WIDGET_1_H
#define UDISKS_SCANNER_MAIN_WIDGET_1_H
#include <QWidget>

class QComboBox;
class ScannerView;
class QVBoxLayout;
class QResizeEvent;
class ScannerTaskModel;
class MainWidget1 : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget1(QWidget* parent=nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;


private:
    QComboBox*              mComboBox = nullptr;
    QVBoxLayout*            mMainLayout = nullptr;

    ScannerView*            mView = nullptr;
    ScannerTaskModel*       mModel = nullptr;

    const int               mItemCheckSize = 40;
    const int               mItemIdxSize = 90;
    const int               mItemStartTimeSize = 180;
    const int               mItemStatusSize = 230;
    const int               mItemProgressSize = 140;
    const int               mItemDetailSize = 90;
};


#endif //UDISKS_SCANNER_MAIN_WIDGET_1_H
